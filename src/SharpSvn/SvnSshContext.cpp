#include "Stdafx.h"
#if SVN_VER_MINOR >= 9
#include <apr_portable.h>
#include "SvnSshContext.h"
#include "SvnAuthentication.h"
#include "UnmanagedStructs.h"
#include <WinCred.h>

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

    //* tell libssh2 we want it all done blocking (=default for libssh2)
    libssh2_session_set_blocking(_session, 1);
}

SshConnection::~SshConnection()
{
    delete _connBaton;

    if (_session)
    {
        libssh2_session_free(_session);
        _session = nullptr;
    }
}

SshConnection::!SshConnection()
{
    if (_session)
    {
        libssh2_session_free(_session);
        _session = nullptr;
    }
}

#define GET_32BIT_MSB_FIRST(cp) \
  (((unsigned long)(unsigned char)(cp)[0] << 24) | \
  ((unsigned long)(unsigned char)(cp)[1] << 16) | \
  ((unsigned long)(unsigned char)(cp)[2] << 8) | \
  ((unsigned long)(unsigned char)(cp)[3]))

#define GET_32BIT(cp) GET_32BIT_MSB_FIRST(cp)

static void next_key_string(const char *&data, size_t &datalen, const char *&p, size_t &length)
{
    p = nullptr;
    length = 0;

    if (datalen < 4)
        return;
    length = (size_t)(GET_32BIT(data));
    if (length < 0)
        return;
    datalen -= 4;
    data += 4;
    if (datalen < length)
        return;
    p = data;
    data += length;
    datalen -= length;
}

static String^ HashToString(const char *data, size_t length)
{
    StringBuilder ^sb = gcnew StringBuilder();

    for (size_t i = 0; i < length; i++)
    {
        sb->AppendFormat("{0:x2}", *(unsigned char*)(data + i));
    }

    return sb->ToString();
}

static String^ BigNumToString(const char *data, size_t length, int *bits=NULL)
{
    StringBuilder ^sb = gcnew StringBuilder();

    size_t i = 0;

    while (i < length && data[i] == 0)
      i++;

    if (bits)
    {
        *bits = -8;

        unsigned u = *(unsigned char*)(data+i);
        while (u > 0)
        {
            *bits += 1;
            u >>= 1;
        }
    }

    if (*(unsigned char *)(data+i) <= 0xF)
    {
        sb->AppendFormat("{0:x}", *(unsigned char*)(data + i));
        i++;

        if (bits)
            *bits += 8;
    }

    for (; i < length; i++)
    {
        sb->AppendFormat("{0:x2}", *(unsigned char*)(data + i));
        if (bits)
          *bits += 8;
    }

    return sb->ToString();
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

    VerifySshHost(scratchPool);

    {
        const char *authTypes = libssh2_userauth_list(_session, username, strlen(username));
        bool authenticated = FALSE;

        char *next = apr_pstrdup(scratchPool->Handle, authTypes);

        while (*next && !libssh2_userauth_authenticated(_session))
        {
            char *end = strchr(next, ',');

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

static bool FingerPrintAccepted(SvnSshServerTrustEventArgs ^e)
{
    return !((int)e->Failures & ~(int)e->AcceptedFailures);
}

void SshConnection::VerifySshHost(AprPool ^scratchPool)
{
    const char *hostname = scratchPool->AllocString(_host->Host);

    int rc;
    rc = libssh2_session_handshake(_session, _socket);

    if (rc)
    {
        throw gcnew SvnRepositoryIOException(
                svn_error_create(SVN_ERR_RA_CANNOT_CREATE_TUNNEL, NULL, NULL));
    }

    bool hostMismatch = false;

    const char *hostkey;
    size_t key_sz;
    int key_type;

    hostkey = libssh2_session_hostkey(_session, &key_sz, &key_type);

    if (!hostkey)
        throw gcnew InvalidOperationException("SSH server didn't hand us an hostkey!");

    String ^puttyRealm;
    String ^puttyValue;
    int bits = -1;

    if (key_type == LIBSSH2_HOSTKEY_TYPE_RSA)
    {
        const char *fp, *p;
        size_t fp_sz, p_sz;

        fp = hostkey;
        fp_sz = key_sz;

        next_key_string(fp, fp_sz, p, p_sz);
        // "ssh-rsa"

        next_key_string(fp, fp_sz, p, p_sz);

        if (p_sz > 0 && p_sz <= 8)
        {
            String ^mod = BigNumToString(p, p_sz);
            next_key_string(fp, fp_sz, p, p_sz);

            puttyRealm = String::Format("rsa2@{1}:{0}", _host->Host, _host->Port);
            puttyValue = String::Format("0x{0},0x{1}", mod, BigNumToString(p, p_sz, &bits));
        }
    }
    else if (key_type == LIBSSH2_HOSTKEY_TYPE_DSS)
    {
        const char *fp, *p;
        size_t fp_sz, p_sz;

        fp = hostkey;
        fp_sz = key_sz;

        next_key_string(fp, fp_sz, p, p_sz);
        // "ssh-rsa"
        StringBuilder ^sb = gcnew StringBuilder();
        for (int i = 0; i < 4; i++)
        {
            next_key_string(fp, fp_sz, p, p_sz);

            sb->Append(i ? ",0x" : "0x");

            String ^v = BigNumToString(p, p_sz);

            // Putty logic?
            if (v->Length % 4)
                sb->Append(L'0', 4-(v->Length % 4));

            sb->Append(v);
        }

        puttyRealm = String::Format("dss@{1}:{0}", _host->Host, _host->Port);
        puttyValue = sb->ToString();
    }
    else
    {
        puttyRealm = puttyValue = nullptr;
    }

    if (!_ctx->ConfigPath && puttyRealm && puttyValue)
    {
        HKEY hk;

        if (ERROR_SUCCESS == RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\SimonTatham\\PuTTY\\SshHostKeys", 0, KEY_QUERY_VALUE, &hk))
        try
        {
            pin_ptr<const wchar_t> pRealm = PtrToStringChars(puttyRealm);
            DWORD nData = sizeof(wchar_t) * (2 + puttyValue->Length);
            DWORD vType;
            BYTE *pData = (BYTE*)alloca(nData);

            LONG rr = RegQueryValueEx(hk, pRealm, NULL, &vType, pData, &nData);

            // We don't need a bigger buffer than this, as that would imply no match.
            if (rr == ERROR_SUCCESS)
            {
                pin_ptr<const wchar_t> pValue = PtrToStringChars(puttyValue);
                if (vType == REG_SZ && ! wcscmp(pValue, (wchar_t*)pData))
                {
                    return; // Victory... We trust this host!
                }
                else
                    hostMismatch = true;
            }
            else if (rr == ERROR_MORE_DATA)
                hostMismatch = true;
        }
        finally
        {
            RegCloseKey(hk);
        }
    }

    LIBSSH2_KNOWNHOSTS *knownHosts = libssh2_knownhost_init(_session);
    try
    {
        const char *knownHostsFile = nullptr;
        const char *svnCfgDir;

        if (_ctx->ConfigPath)
            svnCfgDir = scratchPool->AllocAbsoluteDirent(_ctx->ConfigPath);
        else
        {
            svn_error_t *err = svn_config_get_user_config_path(&svnCfgDir, NULL, "", scratchPool->Handle);

            if (err)
            {
                svn_error_clear(err);
                svnCfgDir = NULL;
            }
        }

        if (svnCfgDir)
        {
            const char *utf8KnownFile = svn_dirent_join(svnCfgDir, "auth/ssh-known_hosts",
                                                        scratchPool->Handle);

            // Yuck.. libssh2 wants ansi encoding
            IntPtr p = System::Runtime::InteropServices::Marshal::StringToCoTaskMemAnsi(SvnBase::Utf8_PathPtrToString(utf8KnownFile, scratchPool));
            knownHostsFile = apr_pstrdup(scratchPool->Handle, (const char *)(void*)p);
            System::Runtime::InteropServices::Marshal::FreeCoTaskMem(p);
        }

        if (knownHostsFile)
        {
            // Ignore errors
            libssh2_knownhost_readfile(knownHosts, knownHostsFile,
                                       LIBSSH2_KNOWNHOST_FILE_OPENSSH);
        }

        if (!hostMismatch)
        {
            int check = libssh2_knownhost_checkp(knownHosts,
                                                 hostname, _host->Port,
                                                 hostkey, key_sz,
                                                 LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                                 LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                                 NULL);

            if (check == LIBSSH2_KNOWNHOST_CHECK_MATCH)
            {
                return; // Victory
            }
            else if (check == LIBSSH2_KNOWNHOST_CHECK_MISMATCH)
            {
                hostMismatch = true;
            }
            // else not found
        }

        const char *hash = libssh2_hostkey_hash(_session, LIBSSH2_HOSTKEY_HASH_MD5);
        String ^fingerprint = hash ? HashToString(hash, 16) : nullptr;

        SvnSshTrustFailures failures = SvnSshTrustFailures::UnknownServerKey
                                       | (hostMismatch ? SvnSshTrustFailures::ServerKeyMismatch : SvnSshTrustFailures::None);

        SvnSshServerKeyType keyType;

        switch (key_type)
        {
            case LIBSSH2_HOSTKEY_TYPE_RSA:
                keyType = SvnSshServerKeyType::Rsa;
                break;
            case LIBSSH2_HOSTKEY_TYPE_DSS:
                keyType = SvnSshServerKeyType::Dss;
                break;
            default:
                keyType = SvnSshServerKeyType::None;
       }

        bool maySave = (knownHostsFile != nullptr) && (keyType != SvnSshServerKeyType::None);

        SvnSshServerTrustEventArgs^ ee = gcnew SvnSshServerTrustEventArgs(keyType, bits, fingerprint,
                                                                          failures,
                                                                          L'<' + _host->RealmString + L'>',
                                                                          maySave);
        if (_ctx->Authentication->Run(ee, gcnew Predicate<SvnSshServerTrustEventArgs^>(&FingerPrintAccepted)))
        {
            // Victory... The user accepted the fingerprint
            if (maySave && ee->Save)
            {
                // Avoid race conditions... read it again...

                libssh2_knownhost_free(knownHosts);
                knownHosts = libssh2_knownhost_init(_session);

                libssh2_knownhost_readfile(knownHosts, knownHostsFile,
                                           LIBSSH2_KNOWNHOST_FILE_OPENSSH);

                int typemask = LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW;

                switch (key_type)
                  {
                    case LIBSSH2_HOSTKEY_TYPE_RSA:
                      typemask |= LIBSSH2_KNOWNHOST_KEY_SSHRSA;
                      break;
                    case LIBSSH2_HOSTKEY_TYPE_DSS:
                      typemask |= LIBSSH2_KNOWNHOST_KEY_SSHDSS;
                      break;
                    default:
                      // ?? -> will fail when saving
                      break;
                  }

                // If there is an old item, remove it
                {
                    struct libssh2_knownhost *kh = NULL;
                    int check = libssh2_knownhost_checkp(knownHosts,
                                                         hostname, _host->Port,
                                                         hostkey, key_sz,
                                                         LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                                         LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                                         &kh);

                    if (kh && (check == LIBSSH2_KNOWNHOST_CHECK_MISMATCH
                                || check == LIBSSH2_KNOWNHOST_CHECK_MATCH))
                    {
                        libssh2_knownhost_del(knownHosts, kh);
                    }
                }

                if (!libssh2_knownhost_addc(knownHosts,
                                            hostname, /* _host->Port, ?? */
                                            NULL /* salt */,
                                            hostkey, key_sz,
                                            NULL, 0, /* comment */
                                            typemask,
                                            NULL))
                  {
                    libssh2_knownhost_writefile(knownHosts, knownHostsFile,
                                                LIBSSH2_KNOWNHOST_FILE_OPENSSH);
                  }

                if (!_ctx->ConfigPath && puttyRealm && puttyValue)
                {
                    HKEY hk;
                    if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\SimonTatham\\PuTTY\\SshHostKeys", 0, NULL, 0, KEY_SET_VALUE, NULL, &hk, NULL))
                    try
                    {
                        pin_ptr<const wchar_t> pRealm = PtrToStringChars(puttyRealm);
                        pin_ptr<const wchar_t> pValue = PtrToStringChars(puttyValue);

                        RegSetValueEx(hk, pRealm, NULL, REG_SZ, (BYTE*)(void*)pValue, sizeof(*pValue) * (puttyValue->Length + 1));
                    }
                    finally
                    {
                        RegCloseKey(hk);
                    }
                }
            }
        }
        else
            throw gcnew SvnRepositoryIOException(
                    svn_error_create(SVN_ERR_RA_CANNOT_CREATE_TUNNEL, NULL, "Authentication failed: SSH server not trusted"));
    }
    finally
    {
        libssh2_knownhost_free(knownHosts);
    }
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

struct SharpSvn::Implementation::ssh_keybint_t
{
    apr_pool_t *pool;

    CREDENTIALW *pCred; // For first attempt
    int nPrompts;
    bool withEcho;

    gcroot<SvnUserNamePasswordEventArgs^> current;
};

static LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC(sharpsvn_ssh_keyboard_interactive)
  // void sharpsvn_ssh_keyboard_interactive(const char* name, int name_len,
  //                                        const char* instruction, int instruction_len,
  //                                        int num_prompts,
  //                                        const LIBSSH2_USERAUTH_KBDINT_PROMPT* prompts,
  //                                        LIBSSH2_USERAUTH_KBDINT_RESPONSE* responses, void **abstract)
{
    SshConnection ^conn = AprBaton<SshConnection^>::Get((IntPtr)*abstract);

    struct ssh_keybint_t &kbi = *conn->_kbi;
    kbi.nPrompts = num_prompts;

    if (num_prompts > 1 && prompts[0].echo)
        kbi.withEcho = true;

    conn->PerformKeyboardInteractive(SvnBase::Utf8_PtrToString(name, name_len),
                                     SvnBase::Utf8_PtrToString(instruction, instruction_len),
                                     num_prompts, prompts, responses);
}

static LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC(sharpsvn_ssh_keyboard_interactive_from_cache)
  // void sharpsvn_ssh_keyboard_interactive(const char* name, int name_len,
  //                                        const char* instruction, int instruction_len,
  //                                        int num_prompts,
  //                                        const LIBSSH2_USERAUTH_KBDINT_PROMPT* prompts,
  //                                        LIBSSH2_USERAUTH_KBDINT_RESPONSE* responses, void **abstract)
{
    UNUSED_ALWAYS(name);
    UNUSED_ALWAYS(name_len);
    UNUSED_ALWAYS(instruction);
    UNUSED_ALWAYS(instruction_len);
    SshConnection ^conn = AprBaton<SshConnection^>::Get((IntPtr)*abstract);

    struct ssh_keybint_t &kbi = *conn->_kbi;
    kbi.nPrompts = num_prompts;

    if (num_prompts > 1 && prompts[0].echo)
        kbi.withEcho = true;

    if (num_prompts == 1) // We guess that this is a password prompt...
    {
        int needed = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)kbi.pCred->CredentialBlob,
                                          kbi.pCred->CredentialBlobSize / sizeof(wchar_t),
                                          NULL, 0, NULL, NULL);

        responses[0].length = needed;
        responses[0].text = (char*)calloc(needed+1, 1);

        int written = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)kbi.pCred->CredentialBlob,
                                          kbi.pCred->CredentialBlobSize / sizeof(wchar_t),
                                          responses[0].text, responses[0].length, NULL, NULL);

        if (needed != written)
          memset(&responses[0], sizeof(responses[0]), 0);
    }
}

bool SshConnection::DoKeyboardInteractiveAuth(AprPool^ scratchPool)
{
    struct ssh_keybint_t kbi;

    _kbi = &kbi;

    try
    {
        String ^realm = _host->RealmString;
        const char *username = scratchPool->AllocString(_host->User);
        CREDENTIALW *pCred;
        int rc;

        kbi.pool = scratchPool->Handle;
        
        kbi.nPrompts = -1;

        pin_ptr<const wchar_t> pRealm = PtrToStringChars(realm);
        if (CredReadW(pRealm, CRED_TYPE_GENERIC, 0, &pCred))
        {
            kbi.pCred = pCred;

            rc = libssh2_userauth_keyboard_interactive_ex(_session, username, strlen(username),
                                                          sharpsvn_ssh_keyboard_interactive_from_cache);

            kbi.pCred = nullptr;

            CredFree(pCred);

            if (!rc)
                return true; // Succeeded

            // Authentication failed... Delete failed credentials

            //CredDelete(pRealm, CRED_TYPE_GENERIC, 0);
        }

        SvnUserNamePasswordEventArgs^ ee = gcnew SvnUserNamePasswordEventArgs(_host->User,
                                                                              L'<' + _host->RealmString + L'>',
                                                                              true, false);
        if (_ctx->Authentication->Run(ee, gcnew Predicate<SvnUserNamePasswordEventArgs^>(this, &SshConnection::TryKeyboardInteractive)))
        {
            return true;
        }

        return false;
    }
    finally
    {
        _kbi = nullptr;
    }
}

void SshConnection::PerformKeyboardInteractive(String ^name, String ^instructions,
                                               int num_prompts, const LIBSSH2_USERAUTH_KBDINT_PROMPT* prompts,
                                               LIBSSH2_USERAUTH_KBDINT_RESPONSE* responses)
{
    struct ssh_keybint_t &kbi = *_kbi;

    if (kbi.current && num_prompts >= 1 && !prompts[0].echo)
    {
        AprPool scratchPool(%_pool);

        const char *password = scratchPool.AllocString(kbi.current->Password);

        responses[0].text = _strdup(password);
        responses[0].length = strlen(password);
    }
}


bool SshConnection::TryKeyboardInteractive(SvnUserNamePasswordEventArgs ^e)
{
    struct ssh_keybint_t &kbi = *_kbi;

    if (kbi.nPrompts >= 0 && kbi.nPrompts != 1)
    {
        e->Cancel = true;
        return false;
    }

    if (! e->Password)
    {
        e->UserName = e->InitialUserName;
        e->Password = "";
        return false;
    }

    AprPool scratchPool(%_pool);

    kbi.current = e;
    const char *username = scratchPool.AllocString(e->UserName);

    if (!libssh2_userauth_keyboard_interactive_ex(_session, username,
                                                  strlen(username),
                                                  sharpsvn_ssh_keyboard_interactive))
    {
        // Success!

        // Let's store the credentials for the current 'Windows Session'
        // even when not requesting to save to avoid asking over and over again.
        CREDENTIALW cred;
        memset(&cred, sizeof(cred), 0);

        pin_ptr<const wchar_t> pRealm = PtrToStringChars(_host->RealmString);
        pin_ptr<const wchar_t> pPassword = PtrToStringChars(e->Password);

        cred.Type = CRED_TYPE_GENERIC;
        cred.TargetName = const_cast<wchar_t*>(pRealm);
        cred.CredentialBlob = (BYTE*)const_cast<wchar_t*>(pPassword);
        cred.CredentialBlobSize = sizeof(wchar_t) * wcslen(pPassword);

        if (e->Save && e->MaySave)
            cred.Persist = CRED_PERSIST_ENTERPRISE;
        else
            cred.Persist = CRED_PERSIST_SESSION;

        CredWriteW(&cred, 0); // Ignore failures

        return true;
    }

    e->UserName = e->InitialUserName ? e->InitialUserName : "";
    e->Password = "";
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
    void *connection_baton;
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

        {
            char buffer[1024];

            nBytes = libssh2_channel_read_stderr(ssh->channel, buffer, sizeof(buffer));

            if (nBytes < 0)
                break;

            if (!ssh->read_once && nBytes > 0 && libssh2_channel_eof(ssh->channel))
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

    SshConnection ^conn = AprBaton<SshConnection^>::Get((IntPtr)ssh->connection_baton);

    conn->ClosedTunnel();

    return SVN_NO_ERROR;
}

void SshConnection::OpenTunnel(svn_stream_t *&channel,
                               AprPool ^resultPool)
{
    LIBSSH2_CHANNEL *ssh_channel = libssh2_channel_open_session(_session);

    if (!ssh_channel)
        throw gcnew InvalidOperationException("Can't create channel");

    int rc = libssh2_channel_exec(ssh_channel, "svnserve -t");

    if (rc)
        throw gcnew InvalidOperationException("Exec failed");

    ssh_baton *ch = (ssh_baton*)resultPool->AllocCleared(sizeof(*ch));
    ch->channel = ssh_channel;
    ch->connection_baton = _connBaton->Handle;

    channel = svn_stream_create(ch, resultPool->Handle);

    svn_stream_set_write(channel, ssh_write);
    svn_stream_set_read2(channel, ssh_read, NULL);
    svn_stream_set_close(channel, ssh_close);
}

void SshConnection::ClosedTunnel()
{
}

void SvnSshContext::OpenTunnel(svn_stream_t *&channel,
                               String ^user, String ^hostname, int port,
                               AprPool ^resultPool, AprPool ^scratchPool)
{
    SshHost ^host = gcnew SshHost(user ? user : "", hostname, port);
    SshConnection ^connection;

    if (!_connections->TryGetValue(host, connection))
    {
        connection = gcnew SshConnection(this, host, %_pool);

        connection->OpenConnection(scratchPool);

        _connections[host] = connection;
    }

    return connection->OpenTunnel(channel, resultPool);
}
#endif
