#pragma once

#include "SvnBase.h"
#include "AprPool.h"

namespace SharpSvn {
    namespace Implementation {

        using System::Collections::Generic::Dictionary;


        private ref class SshHost sealed : IEquatable<SshHost^>
        {
            initonly String ^_user;
            initonly String ^_host;
            initonly int _port;

        public:
            SshHost(String ^user, String ^host, int port)
            {
                if (String::IsNullOrEmpty(user))
                    throw gcnew ArgumentNullException("user");
                else if (String::IsNullOrEmpty(host))
                    throw gcnew ArgumentNullException("host");

                _user = user;
                _host = host;
                _port = port;
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

            virtual int GetHashCode() override
            {
                return Host->GetHashCode()
                    ^ 7 * User->GetHashCode()
                    ^ 13 * Port;
            }
        };

        private ref class SshConnection sealed : public SvnBase
        {
            initonly SvnSshContext ^_ctx;
            initonly SshHost ^_host;
            AprPool _pool;
            initonly AprBaton<SshConnection^>^ _connBaton;
            apr_socket_t *_apr_socket;
            SOCKET _socket;
            LIBSSH2_SESSION *_session;
            LIBSSH2_KNOWNHOSTS *_knownHosts;

        public:
            SshConnection(SvnSshContext ^ctx, SshHost ^host, AprPool ^pool);
            ~SshConnection();
            !SshConnection();

        public:
            void OpenTunnel(svn_stream_t *&channel,
                            svn_ra_close_tunnel_func_t &close_func,
                            void *&close_baton,
                            AprPool^ pool);

        public:
            void OpenConnection(AprPool^ scratchPool);

        private:
            bool DoPublicKeyAuth(AprPool^ scratchPool);
            bool DoKeyboardInteractiveAuth(AprPool^ scratchPool);
            bool DoPasswordAuth(AprPool^ scratchPool);
        };

        private ref class SvnSshContext sealed : public SvnBase
        {
            AprPool _pool;
            initonly SvnClientContext ^_ctx;
            initonly Dictionary<SshHost^, SshConnection^>^ _connections;

        public:
            SvnSshContext(SvnClientContext^ ctx)
                : _pool(ctx->ContextPool)
            {
                if (!ctx)
                    throw gcnew ArgumentNullException("ctx");
                _ctx = ctx;

                _connections = gcnew Dictionary<SshHost^, SshConnection^>();
            }


        public:
            void OpenTunnel(svn_stream_t *&channel,
                            svn_ra_close_tunnel_func_t &close_func,
                            void *&close_baton,
                            String^ user,
                            String^ hostname,
                            int port,
                            AprPool^ pool);
        };

    }
}
