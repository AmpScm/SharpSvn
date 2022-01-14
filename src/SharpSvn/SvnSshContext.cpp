#include "Stdafx.h"

#define SECURITY_WIN32
#include <Security.h>
#include <WinCred.h>

#include <apr_portable.h>
#include "SvnSshContext.h"
#include "SvnAuthentication.h"
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

    //* tell libssh2 we want it all done blocking (=default for libssh2)
    libssh2_session_set_blocking(_session, 1);

    _apr_socket = NULL;
    _socket = INVALID_SOCKET;
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

static svn_error_t *get_session_error(LIBSSH2_SESSION *session)
{
  if (session)
  {
      char *msg = NULL;
      int err = libssh2_session_last_error(session, &msg, NULL, FALSE);

      if (err && msg)
      {
          apr_status_t status;
          if (err < 0 && err > - SVN_ERR_CATEGORY_SIZE)
              status = SSH2TOSVNERROR(err);
          else
              status = SVN_ERR_RA_SVN_IO_ERROR;
          return svn_error_createf(status, NULL,
                                   "SSH%03d: %s", -err, msg);
      }
  }

  return svn_error_create(SVN_ERR_RA_SVN_IO_ERROR, NULL, NULL);
}

svn_error_t *SshConnection::GetSessionError()
{
    return get_session_error(_session);
}

void SshConnection::ResolveAddress(AprPool^ scratchPool)
{
    const char *hostname = scratchPool->AllocString(_host->Host);

    apr_sockaddr_t *sa;
    apr_socket_t *sock;
    apr_status_t status;
    int family = APR_INET;

    /* Make sure we have IPV6 support first before giving apr_sockaddr_info_get
    APR_UNSPEC, because it may give us back an IPV6 address even if we can't
    create IPV6 sockets.  */

    status = apr_socket_create(&sock, APR_INET6, SOCK_STREAM, APR_PROTO_TCP,
        scratchPool->Handle);
    if (status == 0)
    {
        apr_socket_close(sock);
        family = APR_UNSPEC;
    }

    /* Resolve the hostname. */
    status = apr_sockaddr_info_get(&sa, hostname, family,
                                   (apr_port_t)_host->Port, 0, _pool.Handle);


    HKEY hkey1 = NULL, hkey2 = NULL;
    if (!RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\SimonTatham\\PuTTY\\Sessions", 0,
                       KEY_QUERY_VALUE, &hkey1))
    try
    {
        pin_ptr<const wchar_t> pName = PtrToStringChars(_host->Host);
        if (!RegOpenKeyExW(hkey1, pName, 0, KEY_QUERY_VALUE, &hkey2))
        {
            char username_buffer[256+1];
            char hostname_buffer[256 + 1];
            DWORD dwType;
            DWORD dwLen;
            DWORD dwPort;

            dwLen = sizeof(dwPort);
            if (!RegQueryValueExA(hkey2, "PortNumber", NULL, &dwType, (BYTE*)&dwPort, &dwLen)
                && dwType == REG_DWORD)
            {
            }
            else
                dwPort = _host->Port;

            dwLen = sizeof(username_buffer) - 1;
            if (!RegQueryValueExA(hkey2, "UserName", NULL, &dwType, (BYTE*)&username_buffer, &dwLen)
                && dwType == REG_SZ)
            {
                username_buffer[dwLen] = '\0';
            }
            else
                username_buffer[0] = '\0';

            dwLen = sizeof(hostname_buffer) - 1;
            if (!RegQueryValueExA(hkey2, "HostName", NULL, &dwType, (BYTE*)&hostname_buffer, &dwLen)
                && dwType == REG_SZ)
            {
                hostname_buffer[dwLen] = '\0';

                if (status)
                    status = apr_sockaddr_info_get(&sa, hostname_buffer, family,
                                                   (apr_port_t)dwPort, 0, _pool.Handle);

                if (!status && username_buffer[0])
                    _defaultUsername = gcnew String(username_buffer);
            }
        }
        else
        {
            hkey2 = NULL;
        }
    }
    finally
    {
        if (hkey2)
            RegCloseKey(hkey2);
        RegCloseKey(hkey1);
    }

    if (status)
        SVN_THROW(svn_error_wrap_apr(status, NULL));

    _sockAddr = sa;
}

void SshConnection::OpenSocket(AprPool^ scratchPool)
{
    const char *hostname = scratchPool->AllocString(_host->Host);
    apr_socket_t *sock = nullptr;
    apr_status_t status = APR_EINCOMPLETE;

    /* Iterate through the returned list of addresses attempting to
    * connect to each in turn. */
    while (status != APR_SUCCESS && _sockAddr)
    {
        /* Create the socket. */
        status = apr_socket_create(&sock, _sockAddr->family, SOCK_STREAM, APR_PROTO_TCP,
            _pool.Handle);

        if (_cancel_func)
          SVN_THROW(_cancel_func(_cancel_baton));

        if (status == APR_SUCCESS)
        {
            status = apr_socket_connect(sock, _sockAddr);
            if (status == APR_SUCCESS)
                break;

            apr_socket_close(sock);
        }
        _sockAddr = _sockAddr->next;
    }

    if (status || !_sockAddr)
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

static bool TryUserName(SvnUserNameEventArgs^ e)
{
    return !String::IsNullOrEmpty(e->UserName);
}

void SshConnection::OpenConnection(svn_cancel_func_t cancel_func, void * cancel_baton,
                                   AprPool^ scratchPool)
{
    _cancel_func = cancel_func;
    _cancel_baton = cancel_baton;

    ResolveAddress(scratchPool);
    OpenSocket(scratchPool);
    VerifySshHost(scratchPool);

    _username = _host->User;
    bool keepUserWhenNoPassword = false;
    bool saveUserWhenNoPassword = false;

    pin_ptr<const wchar_t> pRealm = PtrToStringChars(_host->RealmString);
    CREDENTIALW *pCred;

    // Allow overriding the username, even when using "user@server" connection
    if (CredReadW(pRealm, CRED_TYPE_GENERIC, 0, &pCred))
    {
        if (pCred->UserName && *pCred->UserName)
          _username = gcnew String(pCred->UserName);

        CredFree(pCred);
    }

    if (String::IsNullOrEmpty(_username) && !String::IsNullOrEmpty(_defaultUsername))
        _username = _defaultUsername; // From plink
    else if (String::IsNullOrEmpty(_username))
    {
        bool maySave = Environment::UserInteractive;

        SvnUserNameEventArgs ^ee = gcnew SvnUserNameEventArgs(L'<' + _host->RealmString + L'>', maySave);
        if (_ctx->Authentication->Run(ee, gcnew Predicate<SvnUserNameEventArgs^>(&TryUserName)))
        {
            _username = ee->UserName;
            keepUserWhenNoPassword = true;
            saveUserWhenNoPassword = ee->Save;
        }
        else if (ee->Cancel)
            throw gcnew SvnRepositoryIOException(
                  svn_error_create(SVN_ERR_RA_CANNOT_CREATE_TUNNEL, NULL, "Authentication failed"));
    }


    const char *username = scratchPool->AllocString(_username);
    {
        const char *authTypes = libssh2_userauth_list(_session, username, strlen(username));
        bool authenticated = FALSE;

        char *next = apr_pstrdup(scratchPool->Handle, authTypes);

        // NOTE: _session might change while authenticating to switch username.
        while (*next && !libssh2_userauth_authenticated(_session))
        {
            char *end = strchr(next, ',');

            if (end)
                *end = '\0';

            if (_cancel_func)
                SVN_THROW(_cancel_func(_cancel_baton));

            if (! strcmp(next, "publickey"))
                authenticated = DoPublicKeyAuth(keepUserWhenNoPassword, saveUserWhenNoPassword, scratchPool);
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

void SshConnection::SwitchUsername(String ^toUser, AprPool ^scratchPool)
{
    // SSH doesn't allow switching user after using a specific user... which
    // we already did when determining authentication methods...

    // This will log a failed authentication attempt on the server.
    // but then... the user shouldn't have switched users.

    if (_session)
    {
        libssh2_session_free(_session);
        _session = nullptr;
    }
    if (_apr_socket)
    {
        apr_socket_close(_apr_socket);
        _apr_socket = nullptr;
        _socket = INVALID_SOCKET;
    }

    _session = libssh2_session_init_ex(nullptr, nullptr, nullptr, _connBaton->Handle);

    //* tell libssh2 we want it all done blocking (=default for libssh2)
    libssh2_session_set_blocking(_session, 1);

    OpenSocket(scratchPool); // Create socket connection
    VerifySshHost(scratchPool); // Verify hostkey (shortcut for exact match like here)

    // If nothing failed by now, we properly switched users...
    _username = toUser;
}

bool SshConnection::IsConnected()
{
    if (_socket != INVALID_SOCKET)
      {
          int ret;
          int len = sizeof(ret);

          if (!getsockopt(_socket, SOL_SOCKET, SO_ERROR, (char*)&ret, &len))
          {
              return !ret; // No error
          }
      }

    return false;
}

static bool FingerPrintAccepted(SvnSshServerTrustEventArgs ^e)
{
    return !((int)e->Failures & ~(int)e->AcceptedFailures);
}

void SshConnection::VerifySshHost(AprPool ^scratchPool)
{
    const char *hostname = scratchPool->AllocString(_host->Host);

    if (_cancel_func)
        SVN_THROW(_cancel_func(_cancel_baton));

    int rc;
    rc = libssh2_session_handshake(_session, _socket);

    if (rc)
    {
        throw gcnew SvnSshException(GetSessionError());
    }

    if (_cancel_func)
        SVN_THROW(_cancel_func(_cancel_baton));

    bool hostMismatch = false;

    const char *hostkey;
    size_t key_sz;
    int key_type;

    hostkey = libssh2_session_hostkey(_session, &key_sz, &key_type);

    if (!hostkey)
        throw gcnew InvalidOperationException("SSH server didn't hand us an hostkey!");

    String ^hostKeyBase64;
    {
        array<Byte> ^hostKey = gcnew array<Byte>(key_sz);
        pin_ptr<Byte> pHostKey = &hostKey[0];
        memcpy(pHostKey, hostkey, key_sz);

        hostKeyBase64 = System::Convert::ToBase64String(hostKey);
    }

    if (hostKeyBase64 == _hostKeyBase64)
    {
        return; // Already verified! -> Reconnecting for authentication attempt with different user
    }
    else
    {
        if (_hostKeyBase64)
            hostMismatch = true;

        _hostKeyBase64 = hostKeyBase64;
    }

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

    if (puttyRealm && puttyValue)
    {
        System::Threading::Monitor::Enter(_processKeyCache);
        try
        {
            String ^key = puttyRealm;
            String ^value;

            if (_ctx->ConfigPath)
                key = _ctx->ConfigPath + ";:" + puttyRealm;

            if (_processKeyCache->TryGetValue(key, value))
            {
                if (value == puttyValue)
                  return; // Victory
            }
        }
        finally
        {
            System::Threading::Monitor::Exit(_processKeyCache);
        }
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
            if (puttyRealm && puttyValue)
            {
                System::Threading::Monitor::Enter(_processKeyCache);
                try
                {
                    String ^key = puttyRealm;

                    if (_ctx->ConfigPath)
                        key = _ctx->ConfigPath + ";:" + puttyRealm;

                    _processKeyCache[key] = puttyValue;
                }
                finally
                {
                    System::Threading::Monitor::Exit(_processKeyCache);
                }
            }

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

bool SshConnection::DoPublicKeyAuth(bool keepUser, bool storeUser, AprPool^ scratchPool)
{
    const char *username = scratchPool->AllocString(_username);

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
                {
                    if (keepUser || storeUser)
                    {
                        // Store just the username to avoid prompting again
                        CREDENTIALW cred;
                        memset(&cred, sizeof(cred), 0);

                        pin_ptr<const wchar_t> pRealm = PtrToStringChars(_host->RealmString);
                        pin_ptr<const wchar_t> pUserName = PtrToStringChars(_username);

                        cred.Type = CRED_TYPE_GENERIC;
                        cred.TargetName = const_cast<wchar_t*>(pRealm);
                        cred.UserName = const_cast<wchar_t*>(pUserName);

                        cred.Persist = storeUser ? CRED_PERSIST_ENTERPRISE : CRED_PERSIST_SESSION;

                        CredWriteW(&cred, 0); // Ignore failures
                    }
                    return true;
                }
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

static char *AllocCredentials(unsigned *length, LPBYTE credentialBlob, DWORD credentialBlobSize, AprPool ^pool)
{
  int needed = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)credentialBlob,
                                   credentialBlobSize / sizeof(wchar_t),
                                   NULL, 0, NULL, NULL);

  char *result;

  if (!pool)
      result = (char*)calloc(needed+1, 1); // will be freed with free()
  else
      result = (char*)pool->AllocCleared(needed+1);

  int written = WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)credentialBlob,
                                    credentialBlobSize / sizeof(wchar_t),
                                    result, needed, NULL, NULL);

  if (needed != written)
  {
      memset(result, needed, 0);
      length = 0;
  }
  else
      *length = needed;

  return result;
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
        responses[0].text = AllocCredentials(&responses[0].length, kbi.pCred->CredentialBlob, kbi.pCred->CredentialBlobSize, nullptr);
    }
}

bool SshConnection::DoKeyboardInteractiveAuth(AprPool^ scratchPool)
{
    struct ssh_keybint_t kbi;

    _kbi = &kbi;

    try
    {
        String ^realm = _host->RealmString;
        const char *username = scratchPool->AllocString(_username);
        CREDENTIALW *pCred;
        int rc;

        kbi.pool = scratchPool->Handle;
        
        kbi.nPrompts = -1;

        pin_ptr<const wchar_t> pRealm = PtrToStringChars(realm);
        if (CredReadW(pRealm, CRED_TYPE_GENERIC, 0, &pCred))
        {
            bool del;
            if (pCred->CredentialBlob)
            {
                kbi.pCred = pCred;
                rc = libssh2_userauth_keyboard_interactive_ex(_session, username, strlen(username),
                                                              sharpsvn_ssh_keyboard_interactive_from_cache);

                kbi.pCred = nullptr;
                del = (rc != 0);
            }
            else
            {
                rc = 1; // Trigger failure
                del = false;
            }

            CredFree(pCred);

            if (!rc)
                return true; // Succeeded
            else if (del)
            {
                // Authentication failed... Delete failed credentials
                CredDelete(pRealm, CRED_TYPE_GENERIC, 0);
            }
        }

        bool maySave = Environment::UserInteractive; // Or the CredStore is unavailable
        SvnUserNamePasswordEventArgs^ ee = gcnew SvnUserNamePasswordEventArgs(_username,
                                                                              L'<' + _host->RealmString + L'>',
                                                                              maySave);
        if (_ctx->Authentication->Run(ee, gcnew Predicate<SvnUserNamePasswordEventArgs^>(this, &SshConnection::TryKeyboardInteractive)))
        {
            // Let's store the credentials for the current 'Windows Session'
            // even when not requesting to save to avoid asking over and over again.
            CREDENTIALW cred;
            memset(&cred, sizeof(cred), 0);

            pin_ptr<const wchar_t> pHostRealm = PtrToStringChars(_host->RealmString);
            pin_ptr<const wchar_t> pUserName = PtrToStringChars(_username);
            pin_ptr<const wchar_t> pPassword = PtrToStringChars(ee->Password);

            cred.Type = CRED_TYPE_GENERIC;
            cred.TargetName = const_cast<wchar_t*>(pHostRealm);
            cred.UserName = const_cast<wchar_t*>(pUserName);

            cred.CredentialBlob = (BYTE*)const_cast<wchar_t*>(pPassword);
            cred.CredentialBlobSize = sizeof(wchar_t) * wcslen(pPassword);

            if (ee->Save && ee->MaySave)
                cred.Persist = CRED_PERSIST_ENTERPRISE;
            else
                cred.Persist = CRED_PERSIST_SESSION;

            CredWriteW(&cred, 0); // Ignore failures

            return true;
        }
        else if (ee->Cancel)
            return false;

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

        const char *password;

        if (kbi.current->Password)
            password = scratchPool.AllocString(kbi.current->Password);
        else
            password = "";

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

    AprPool scratchPool(%_pool);

    kbi.current = e;
    const char *username = scratchPool.AllocString(e->UserName);

    if (e->UserName != _username)
    {
        // Some (most?) OpenSSH versions don't support changing usernames once
        // one username is tried... Let's try once and then reconnect with the
        // new name

        if (!libssh2_userauth_keyboard_interactive_ex(_session, username,
                                                      strlen(username),
                                                      sharpsvn_ssh_keyboard_interactive))
        {
            _username = e->UserName;
            return true; // Success!
        }

        SwitchUsername(e->UserName, %scratchPool);
    }

    if (!libssh2_userauth_keyboard_interactive_ex(_session, username,
                                                  strlen(username),
                                                  sharpsvn_ssh_keyboard_interactive))
    {
        return true; // Success!
    }

    return false;
}

bool SshConnection::DoPasswordAuth(AprPool^ scratchPool)
{
      String ^realm = _host->RealmString;
      const char *username = scratchPool->AllocString(_username);
      CREDENTIALW *pCred;
      int rc;

      pin_ptr<const wchar_t> pRealm = PtrToStringChars(realm);
      if (CredReadW(pRealm, CRED_TYPE_GENERIC, 0, &pCred))
      {
          bool del;
          if (pCred->CredentialBlob)
          {
              unsigned len;
              const char *pw = AllocCredentials(&len, pCred->CredentialBlob, pCred->CredentialBlobSize, scratchPool);
              rc = libssh2_userauth_password_ex(_session,
                                                username, strlen(username),
                                                pw, len,
                                                NULL);

              del = (rc != 0);
          }
          else
          {
              rc = 1; // Trigger failure
              del = false;
          }

          CredFree(pCred);

          if (!rc)
              return true; // Succeeded
          else if (del)
          {
              // Authentication failed... Delete failed credentials
              CredDelete(pRealm, CRED_TYPE_GENERIC, 0);
          }
      }

      bool maySave = Environment::UserInteractive; // Or the CredStore is unavailable
      SvnUserNamePasswordEventArgs^ ee = gcnew SvnUserNamePasswordEventArgs(_username,
                                                                            L'<' + _host->RealmString + L'>',
                                                                            maySave);
      if (_ctx->Authentication->Run(ee, gcnew Predicate<SvnUserNamePasswordEventArgs^>(this, &SshConnection::TryPassword)))
      {
          // Let's store the credentials for the current 'Windows Session'
          // even when not requesting to save to avoid asking over and over again.
          CREDENTIALW cred;
          memset(&cred, sizeof(cred), 0);

          pin_ptr<const wchar_t> pUserName = PtrToStringChars(_username);
          pin_ptr<const wchar_t> pPassword = PtrToStringChars(ee->Password);

          cred.Type = CRED_TYPE_GENERIC;
          cred.TargetName = const_cast<wchar_t*>(pRealm);
          cred.UserName = const_cast<wchar_t*>(pUserName);

          cred.CredentialBlob = (BYTE*)const_cast<wchar_t*>(pPassword);
          cred.CredentialBlobSize = sizeof(wchar_t) * wcslen(pPassword);

          if (ee->Save && ee->MaySave)
              cred.Persist = CRED_PERSIST_ENTERPRISE;
          else
              cred.Persist = CRED_PERSIST_SESSION;

          CredWriteW(&cred, 0); // Ignore failures

          return true;
      }

      return false;
}

bool SshConnection::TryPassword(SvnUserNamePasswordEventArgs ^e)
{
    AprPool scratchPool(%_pool);

    const char *username = scratchPool.AllocString(e->UserName);
    const char *password = scratchPool.AllocString(e->Password);

    if (e->UserName != _username)
    {
        // Some (most?) OpenSSH versions don't support changing usernames once
        // one username is tried... Let's try once and then reconnect with the
        // new name

        if (!libssh2_userauth_password_ex(_session,
                                          username, strlen(username),
                                          password, strlen(password),
                                          NULL))
        {
            _username = e->UserName;
            return true; // Success!
        }

        SwitchUsername(e->UserName, %scratchPool);
    }

    if (!libssh2_userauth_password_ex(_session,
                                      username, strlen(username),
                                      password, strlen(password),
                                      NULL))
    {
        return true; // Success!
    }

    return false;
}


struct ssh_baton
{
    SOCKET socket;
    LIBSSH2_CHANNEL *channel;
    LIBSSH2_SESSION *session;

    svn_boolean_t read_once;
    void *connection_baton;

    svn_cancel_func_t cancel_func;
    void *cancel_baton;
};

static svn_error_t * ssh_read(void *baton, char *data, apr_size_t *len)
{
    ssh_baton *ssh = (ssh_baton*)baton;

    if (ssh->cancel_func)
    {
        FD_SET read_set, error_set;
        TIMEVAL tv = {0, 250000}; // 0.25 sec

        FD_ZERO(&read_set);
        FD_ZERO(&error_set);
        FD_SET(ssh->socket, &read_set);
        FD_SET(ssh->socket, &error_set);
        while (0 == select(1, &read_set, NULL, &error_set, &tv))
        {
            if (ssh->cancel_func)
              SVN_ERR(ssh->cancel_func(ssh->cancel_baton));

            FD_ZERO(&read_set);
            FD_ZERO(&error_set);
            FD_SET(ssh->socket, &read_set);
            FD_SET(ssh->socket, &error_set);
        }
    }

    ssize_t nBytes;
    int rc = 0;

    while (0 == (nBytes = libssh2_channel_read(ssh->channel, data, *len)))
    {
        rc = libssh2_channel_eof(ssh->channel);

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

                return svn_error_createf(SVN_ERR_RA_SVN_IO_ERROR,
                                         rc ? get_session_error(ssh->session) : NULL,
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

    if (rc)
        return svn_error_create(SVN_ERR_RA_SVN_IO_ERROR, get_session_error(ssh->session), NULL);
    else
        return svn_error_create(SVN_ERR_RA_SVN_IO_ERROR, NULL, NULL);
}

static svn_error_t * ssh_write(void *baton, const char *data, apr_size_t *len)
{
    ssh_baton *ssh = (ssh_baton*)baton;

    apr_size_t to_write = *len;

    while (to_write > 0)
    {
        ssize_t nBytes = libssh2_channel_write(ssh->channel, data, to_write);

        if (nBytes <= 0)
        {
            *len = *len - to_write;
            return svn_error_create(SVN_ERR_RA_SVN_IO_ERROR, get_session_error(ssh->session), NULL);
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

static svn_error_t * ssh_data_available(void *baton,
                                        svn_boolean_t *data_available)
{
    ssh_baton *ssh = (ssh_baton*)baton;

    FD_SET read_set;
    TIMEVAL tv = {0, 0 }; // Return directly

    FD_ZERO(&read_set);
    FD_SET(ssh->socket, &read_set);

    if (0 < select(1, &read_set, NULL, NULL, &tv))
    {
        // Yes this function is deprecated, but there is no alternative
        // And we do call select() first
        *data_available = (0 != libssh2_poll_channel_read(ssh->channel, 0));
    }
    else
        *data_available = FALSE;

    return SVN_NO_ERROR;
}

void SshConnection::OpenTunnel(svn_stream_t *&channel,
                               svn_cancel_func_t cancel_func, void * cancel_baton,
                               AprPool ^resultPool)
{
    LIBSSH2_CHANNEL *ssh_channel = libssh2_channel_open_session(_session);

    if (!ssh_channel)
        throw gcnew SvnSshException(GetSessionError());

    int rc = libssh2_channel_exec(ssh_channel, "svnserve -t");

    if (rc)
        throw gcnew SvnSshException(GetSessionError());

    ssh_baton *ch = (ssh_baton*)resultPool->AllocCleared(sizeof(*ch));
    ch->socket = _socket;
    ch->channel = ssh_channel;
    ch->session = _session;
    ch->connection_baton = _connBaton->Handle;
    ch->cancel_func = cancel_func;
    ch->cancel_baton = cancel_baton;

    channel = svn_stream_create(ch, resultPool->Handle);

    svn_stream_set_write(channel, ssh_write);
    svn_stream_set_read2(channel, ssh_read, NULL);
    svn_stream_set_data_available(channel, ssh_data_available);
    svn_stream_set_close(channel, ssh_close);

    resultPool->KeepAlive(this);

    _nTunnels++;
}

void SshConnection::ClosedTunnel()
{
    _nTunnels--;

    if (!_nTunnels && _closeOnIdle)
    {
        if (_session)
            libssh2_session_disconnect_ex(_session, SSH_DISCONNECT_BY_APPLICATION, "Thanks!", "");

        delete this;
    }
}

void SshConnection::OperationCompleted(bool keepSessions)
{
    UNUSED_ALWAYS(keepSessions);
    _closeOnIdle = true;

    if (!_nTunnels)
    {
        if (_session)
            libssh2_session_disconnect_ex(_session, SSH_DISCONNECT_BY_APPLICATION, "Thanks!", "");
        delete this;
    }
}

void SvnSshContext::OpenTunnel(svn_stream_t *&channel,
                               String ^user, String ^hostname, int port,
                               svn_cancel_func_t cancel_func, void *cancel_baton,
                               AprPool ^resultPool, AprPool ^scratchPool)
{
    SshHost ^host = gcnew SshHost(user, hostname, port);
    SshConnection ^connection;

    if (!_connections->TryGetValue(host, connection) || !connection->IsConnected())
    {
        connection = gcnew SshConnection(this, host, %_pool);

        connection->OpenConnection(cancel_func, cancel_baton, scratchPool);

        _connections[host] = connection;
    }

    return connection->OpenTunnel(channel, cancel_func, cancel_baton, resultPool);
}

void SvnSshContext::OperationCompleted(bool keepSessions)
{
    if (keepSessions)
        return;

    List<SshConnection^> ^conns = gcnew List<SshConnection^>(_connections->Values);
    _connections->Clear();

    for each (SshConnection^ c in conns)
      {
          c->OperationCompleted(keepSessions);
      }
}
