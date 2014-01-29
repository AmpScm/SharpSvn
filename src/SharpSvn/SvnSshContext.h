#pragma once

#include "SvnBase.h"

namespace SharpSvn {
    namespace Implementation {

        private ref class SvnSshContext sealed : public SvnBase
        {
            initonly SvnClientContext ^_ctx;

        public:
            SvnSshContext(SvnClientContext^ ctx)
            {
                if (!ctx)
                    throw gcnew ArgumentNullException("ctx");
                _ctx = ctx;

            }


        public:
            svn_error_t *OpenTunnel(svn_stream_t *&channel,
                                    svn_ra_close_tunnel_func_t &close_func,
                                    void *&close_baton,
                                    String^ user,
                                    String^ hostname,
                                    int port,
                                    AprPool^ pool);
        };

    }
}
