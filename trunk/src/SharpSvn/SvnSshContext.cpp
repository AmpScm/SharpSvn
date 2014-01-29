#include "Stdafx.h"
#if SVN_VER_MINOR >= 9
#include <apr_portable.h>
#include "SvnSshContext.h"
#include "UnmanagedStructs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SshConnection::SshConnection(SvnSshContext ^ctx, SshHost ^host, AprPool ^inPool)
    : _pool(inPool)
{
    if (!ctx)
        throw gcnew ArgumentNullException("ctx");
    else if (!host)
        throw gcnew ArgumentNullException("host");

    _connBaton = gcnew AprBaton<SshConnection^>(this);

    _ctx = ctx;
    _host = host;

    _session = libssh2_session_init_ex(nullptr, nullptr, nullptr, _connBaton->Handle);

    _knownHosts = libssh2_knownhost_init(_session);

    //* tell libssh2 we want it all done blocking
    libssh2_session_set_blocking(_session, 1);
}

SshConnection::~SshConnection()
{
    delete _connBaton;

    if (_knownHosts)
    {
        libssh2_knownhost_free(_knownHosts);
        _knownHosts = nullptr;
    }
    if (_session)
    {
        libssh2_session_free(_session);
        _session = nullptr;
    }
}

SshConnection::!SshConnection()
{
    if (_knownHosts)
    {
        libssh2_knownhost_free(_knownHosts);
        _knownHosts = nullptr;
    }
    if (_session)
    {
        libssh2_session_free(_session);
        _session = nullptr;
    }
}

void SshConnection::OpenConnection(AprPool^ scratchPool)
{
    const char *hostname = scratchPool->AllocString(_host->Host);
    const char *username = scratchPool->AllocString(_host->User);

    {
        apr_sockaddr_t *sa;
        apr_status_t status;
        int family = APR_INET;
        apr_socket_t *sock;

        /* Make sure we have IPV6 support first before giving apr_sockaddr_info_get
        APR_UNSPEC, because it may give us back an IPV6 address even if we can't
        create IPV6 sockets.  */

        status = apr_socket_create(&sock, APR_INET6, SOCK_STREAM, APR_PROTO_TCP,
            _pool.Handle);
        if (status == 0)
        {
            apr_socket_close(sock);
            family = APR_UNSPEC;
        }

        /* Resolve the hostname. */
        status = apr_sockaddr_info_get(&sa, hostname, family,
            (apr_port_t)_host->Port, 0, _pool.Handle);
        if (status)
            SVN_THROW(svn_error_wrap_apr(status, NULL));

        /* Iterate through the returned list of addresses attempting to
        * connect to each in turn. */
        do
        {
            /* Create the socket. */
            status = apr_socket_create(&sock, sa->family, SOCK_STREAM, APR_PROTO_TCP,
                _pool.Handle);

            if (status == APR_SUCCESS)
            {
                status = apr_socket_connect(sock, sa);
                if (status != APR_SUCCESS)
                    apr_socket_close(sock);
            }
            sa = sa->next;
        } while (status != APR_SUCCESS && sa);

        if (status)
            SVN_THROW(
            svn_error_wrap_apr(status, "Can't connect to host '%s'", hostname));

        /* Enable TCP keep-alives on the socket so we time out when
         * the connection breaks due to network-layer problems.
         * If the peer has dropped the connection due to a network partition
         * or a crash, or if the peer no longer considers the connection
         * valid because we are behind a NAT and our public IP has changed,
         * it will respond to the keep-alive probe with a RST instead of an
         * acknowledgment segment, which will cause svn to abort the session
         * even while it is currently blocked waiting for data from the peer.
         * See issue #3347. */
        status = apr_socket_opt_set(sock, APR_SO_KEEPALIVE, 1);
        if (status)
        {
            /* It's not a fatal error if we cannot enable keep-alives. */
        }

        _apr_socket = sock;
        SOCKET win_socket;
        apr_os_sock_get(&win_socket, sock);
        _socket = win_socket;
    }

    {
        int rc;
        rc = libssh2_session_handshake(_session, _socket);

        if (rc) {
            throw gcnew SvnRepositoryIOException(
                svn_error_create(SVN_ERR_RA_CANNOT_CREATE_TUNNEL, NULL, NULL));
        }

        /* read all hosts from here */
        //libssh2_knownhost_readfile(nh, "known_hosts", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

        /* store all known hosts to here */
        //libssh2_knownhost_writefile(nh, "dumpfile", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

        size_t fp_len;
        int fp_type;

        const char * fingerprint = libssh2_session_hostkey(_session, &fp_len, &fp_type);

        if (fingerprint) {
            struct libssh2_knownhost *host;
            int check = libssh2_knownhost_checkp(_knownHosts,
                hostname, _host->Port,
                fingerprint, fp_len,
                LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_RAW,
                &host);

            // TODO: ### Handle check via callback!
        }
    }

    {
        const char *authTypes = libssh2_userauth_list(_session, username, strlen(username));
        bool authenticated;

        char *next = apr_pstrdup(scratchPool->Handle, authTypes);

        while (*next && !libssh2_userauth_authenticated(_session))
        {
            char *end = strchr(next, ',');
            int len = end ? (end - next) : strlen(next);

            if (end)
                *end = '\0';

            if (! strcmp(next, "publickey"))
                authenticated = DoPublicKeyAuth(scratchPool);
            else if (! strcmp(next, "keyboard-interactive"))
                authenticated = DoKeyboardInteractiveAuth(scratchPool);
            else if (! strcmp(next, "password")) // Verify
                authenticated = DoPasswordAuth(scratchPool);

            if (authenticated || !end)
                break;

            next = end + 1;
        }

        if (libssh2_userauth_authenticated(_session))
            return; // SUCCESS!!!
    }

    throw gcnew SvnRepositoryIOException(
        svn_error_create(SVN_ERR_RA_CANNOT_CREATE_TUNNEL, NULL, "Authentication failed"));
}

bool SshConnection::DoPublicKeyAuth(AprPool^ scratchPool)
{
    const char *username = scratchPool->AllocString(_host->User);

    LIBSSH2_AGENT *agent = libssh2_agent_init(_session);
    bool connected = false;

    try
    {
        if (!libssh2_agent_connect(agent))
            connected = TRUE;

        if (connected
            && !libssh2_agent_list_identities(agent))
        {
            struct libssh2_agent_publickey *identity = NULL;

            while (1)
            {
                int rc = libssh2_agent_get_identity(agent, &identity, identity);

                if (rc == 1)
                    break; // End of list reached

                if (rc < 0) {
                    break; // Error
                }

                if (!libssh2_agent_userauth(agent, username, identity))
                    return true;
            }
        }

        // ### TODO: Use libssh2_userauth_publickey() with callbacks

        return false; // Failed :(
    }
    finally
    {
        if (connected)
            libssh2_agent_disconnect(agent);

        libssh2_agent_free(agent);
    }
}

bool SshConnection::DoKeyboardInteractiveAuth(AprPool^ scratchPool)
{
    return false;
}

bool SshConnection::DoPasswordAuth(AprPool^ scratchPool)
{
    // ### TODO: Use libssh2_userauth_password_ex
    return false;
}

struct ssh_baton
{
    LIBSSH2_CHANNEL *channel;
    svn_boolean_t read_once;
};

static svn_error_t * ssh_read(void *baton, char *data, apr_size_t *len)
{
    ssh_baton *ssh = (ssh_baton*)baton;

    ssize_t nBytes;

    while (0 == (nBytes = libssh2_channel_read(ssh->channel, data, *len)))
    {
        int rc = libssh2_channel_eof(ssh->channel);

        if (rc != 0)
            break;

        if (!ssh->read_once)
        {
            char buffer[1024];

            nBytes = libssh2_channel_read_stderr(ssh->channel, buffer, sizeof(buffer));

            if (nBytes > 0 && libssh2_channel_eof(ssh->channel))
            {
                if (nBytes < sizeof(buffer))
                    buffer[nBytes] = 0;
                else
                    buffer[sizeof(buffer)-1] = 0;

                return svn_error_createf(APR_EOF, NULL,
                                         "libssh2 exec failed:\n%s",
                                         buffer);
            }
        }

        if (rc < 0)
            break;

        // Loop
    }

    if (nBytes >= 0)
    {
        ssh->read_once = TRUE;
        *len = nBytes;
        return SVN_NO_ERROR;
    }

    return svn_error_create(APR_EOF, NULL, NULL);
}

static svn_error_t * ssh_write(void *baton, const char *data, apr_size_t *len)
{
    ssh_baton *ssh = (ssh_baton*)baton;

    apr_size_t to_write = *len;

    while (to_write > 0)
    {
        ssize_t nBytes = libssh2_channel_write(ssh->channel, data, to_write);

        if (nBytes < 0)
        {
            *len = *len - to_write;
            return svn_error_create(APR_EOF, NULL, NULL);
        }

        to_write -= nBytes;
        data += nBytes;
    }

    return SVN_NO_ERROR;
}

static svn_error_t * ssh_close(void *baton)
{
    ssh_baton *ssh = (ssh_baton*)baton;

    if (!libssh2_channel_close(ssh->channel))
        libssh2_channel_wait_closed(ssh->channel);

    return SVN_NO_ERROR;
}

void SshConnection::OpenTunnel(svn_stream_t *&channel,
                               svn_ra_close_tunnel_func_t &close_func,
                               void *&close_baton,
                               AprPool^ pool)
{
    LIBSSH2_CHANNEL *ssh_channel = libssh2_channel_open_session(_session);

    if (!ssh_channel)
        throw gcnew InvalidOperationException("Can't create channel");

    int rc = libssh2_channel_exec(ssh_channel, "svnserve -t");

    if (rc)
        throw gcnew InvalidOperationException("Exec failed");

    ssh_baton *ch = (ssh_baton*)pool->AllocCleared(sizeof(*ch));
    ch->channel = ssh_channel;

    channel = svn_stream_create(ch, pool->Handle);

    svn_stream_set_write(channel, ssh_write);
    svn_stream_set_read2(channel, ssh_read, NULL);
    svn_stream_set_close(channel, ssh_close);
}

void SvnSshContext::OpenTunnel(svn_stream_t *&channel,
                               svn_ra_close_tunnel_func_t &close_func, void *&close_baton,
                               String^ user, String^ hostname, int port, AprPool^ pool)
{
    close_func = NULL;

    SshHost ^host = gcnew SshHost(user ? user : "", hostname, port ? port : 22);
    SshConnection ^connection;

    if (!_connections->TryGetValue(host, connection))
    {
        connection = gcnew SshConnection(this, host, %_pool);

        connection->OpenConnection(pool);

        _connections[host] = connection;
    }

    return connection->OpenTunnel(channel, close_func, close_baton, pool);
}
#endif
