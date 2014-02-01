#pragma once

#include "SvnBase.h"
#include "AprPool.h"

namespace SharpSvn {
    namespace Security {
        ref class SvnAuthentication;
        ref class SvnUserNamePasswordEventArgs;
    }
    namespace Implementation {

        using System::Collections::Generic::Dictionary;
        using System::Collections::Generic::List;
        using SharpSvn::Security::SvnAuthentication;

        private ref class SshHost sealed : IEquatable<SshHost^>
        {
            initonly String ^_user;
            initonly String ^_host;
            initonly int _port;
            String ^_realmString;

        public:
            SshHost(String ^user, String ^host, int port)
            {
                if (String::IsNullOrEmpty(host))
                    throw gcnew ArgumentNullException("host");

                _user = String::IsNullOrEmpty(user) ? "" : user;
                _host = host->ToLowerInvariant(); // DNS is case insensitive and defaults to lowercase
                _port = port ? port : 22;
            }

            virtual bool Equals(Object^ other) override
            {
                SshHost ^otherSsh = dynamic_cast<SshHost^>(other);

                if ((Object^)otherSsh)
                    return Equals(otherSsh);

                return false;
            }

            virtual bool Equals(SshHost^ other)
            {
                if (!other)
                    return false;

                return other->Host->Equals(Host)
                    && other->User->Equals(User)
                    && other->Port.Equals(Port);
            }

            property String^ User
            {
                String^ get() { return _user; }
            }

            property String^ Host
            {
                String^ get() { return _host; }
            }

            property int Port
            {
                int get() { return _port; }
            }

            property String^ RealmString
            {
                String^ get()
                {
                    if (!_realmString)
                    {
                        if ((Port != 22) && !String::IsNullOrEmpty(User))
                            _realmString = String::Format("ssh://{0}@{1}:{2}", User, Host, Port);
                        else if (!String::IsNullOrEmpty(User))
                            _realmString = String::Format("ssh://{0}@{1}", User, Host);
                        else if (Port != 22)
                            _realmString = String::Format("ssh://{0}:{1}", Host, Port);
                        else
                            _realmString = String::Format("ssh://{0}", Host);
                    }
                    return _realmString;
                }
            }

            virtual String ^ToString() override
            {
                return RealmString;
            }

            virtual int GetHashCode() override
            {
                return Host->GetHashCode()
                    ^ 7 * User->GetHashCode()
                    ^ 13 * Port;
            }
        };

        struct ssh_keybint_t;

        private ref class SshConnection sealed : public SvnBase
        {
            initonly SvnSshContext ^_ctx;
            initonly SshHost ^_host;
            AprPool _pool;
            initonly AprBaton<SshConnection^>^ _connBaton;
            apr_sockaddr_t *_sockAddr;
            apr_socket_t *_apr_socket;
            SOCKET _socket;
            LIBSSH2_SESSION *_session;
            String^ _hostKeyBase64;
            String^ _username;
            bool _saveuserWhenNoPassword;
        internal:
            ssh_keybint_t *_kbi;

        public:
            SshConnection(SvnSshContext ^ctx, SshHost ^host, AprPool ^pool);
            ~SshConnection();
            !SshConnection();

        public:
            property SshHost ^ Host
            {
                SshHost ^ get() { return _host; }
            }

        public:
            void OpenTunnel(svn_stream_t *&channel,
                            AprPool^ resultPool);

        public:
            void OpenConnection(AprPool^ scratchPool);

        private:
            void ResolveAddress(AprPool^ scratchPool);
            void OpenSocket(AprPool^ scratchPool);
            void VerifySshHost(AprPool ^scratchPool);
            bool DoPublicKeyAuth(AprPool ^scratchPool);
            bool DoKeyboardInteractiveAuth(AprPool ^scratchPool);
            bool DoPasswordAuth(AprPool ^scratchPool);

            void SwitchUsername(String ^toUser, AprPool ^scratchPool);

        internal:
            void PerformKeyboardInteractive(String ^name, String ^instructions,
                                            int num_prompts, const LIBSSH2_USERAUTH_KBDINT_PROMPT* prompts,
                                            LIBSSH2_USERAUTH_KBDINT_RESPONSE* responses);

            bool TryKeyboardInteractive(SvnUserNamePasswordEventArgs ^e);
            void ClosedTunnel();
        };

        private ref class SvnSshContext sealed : public SvnBase
        {
            AprPool _pool;
            initonly SvnClientContext ^_ctx;
            initonly Dictionary<SshHost^, SshConnection^>^ _connections;
            initonly List<SshConnection^>^ _conns;

        public:
            SvnSshContext(SvnClientContext^ ctx)
                : _pool(ctx->ContextPool)
            {
                if (!ctx)
                    throw gcnew ArgumentNullException("ctx");
                _ctx = ctx;

                _connections = gcnew Dictionary<SshHost^, SshConnection^>();
                _conns = gcnew List<SshConnection^>();
            }

        public:
            void OpenTunnel(svn_stream_t *&channel,
                            String^ user,
                            String^ hostname,
                            int port,
                            AprPool^ resultPool,
                            AprPool^ scratchPool);

        public:
            property String^ ConfigPath
            {
                String^ get() { return _ctx->_configPath; }
            }

            property SvnAuthentication^ Authentication
            {
                SvnAuthentication^ get() { return _ctx->Authentication; }
            }

        public:
            void Unhook(SshConnection ^connection, bool full)
            {
                SshConnection^ other;

                if (_connections->TryGetValue(connection->Host, other))
                {
                    if (other == connection)
                    {
                        _connections->Remove(connection->Host);
                        // But don't remove connection from _conns until
                        // all streams are closed!
                    }
                }

                if (full)
                    _conns->Remove(connection);
            }
        };
    }
}
