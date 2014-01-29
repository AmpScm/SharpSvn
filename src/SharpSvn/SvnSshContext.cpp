#include "Stdafx.h"
#include "SvnSshContext.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

svn_error_t *SvnSshContext::OpenTunnel(svn_stream_t *&channel,
                                       svn_ra_close_tunnel_func_t &close_func, void *&close_baton,
                                       String^ user, String^ hostname, int port, AprPool^ pool)
{
    close_func = NULL;

    return svn_error_create(SVN_ERR_RA_NOT_IMPLEMENTED, NULL, NULL);
}