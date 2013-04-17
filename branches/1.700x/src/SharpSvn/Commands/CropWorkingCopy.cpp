#include "stdafx.h"
#include "SvnAll.h"
#include "Args/CropWorkingCopy.h"
#include <private/svn_wc_private.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::CropWorkingCopy(System::String ^path, SvnDepth toDepth)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(toDepth < SvnDepth::Empty || toDepth >= SvnDepth::Files)
		throw gcnew ArgumentOutOfRangeException("toDepth", toDepth, SharpSvnStrings::CropToValidDepth);
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return CropWorkingCopy(path, toDepth, gcnew SvnCropWorkingCopyArgs());
}

struct do_crop_baton
{
  svn_client_ctx_t *ctx;
  const char *target_abspath;
  svn_depth_t depth;
};

static svn_error_t * __cdecl
perform_crop(void *baton,
             apr_pool_t *result_pool,
             apr_pool_t *scratch_pool)
{
    struct do_crop_baton *dcb = (struct do_crop_baton*)baton;

    UNUSED_ALWAYS(result_pool);

    if (dcb->depth == svn_depth_exclude)
        SVN_ERR(svn_wc_exclude(dcb->ctx->wc_ctx,
                               dcb->target_abspath,
                               dcb->ctx->cancel_func, dcb->ctx->cancel_baton,
                               dcb->ctx->notify_func2, dcb->ctx->notify_baton2,
                               scratch_pool));
    else
        SVN_ERR(svn_wc_crop_tree2(dcb->ctx->wc_ctx,
                                  dcb->target_abspath,
                                  dcb->depth,
                                  dcb->ctx->cancel_func, dcb->ctx->cancel_baton,
                                  dcb->ctx->notify_func2, dcb->ctx->notify_baton2,
                                  scratch_pool));

    return SVN_NO_ERROR;
}

bool SvnClient::CropWorkingCopy(System::String ^path, SvnDepth toDepth, SvnCropWorkingCopyArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
    else if ((toDepth < SvnDepth::Exclude || toDepth >= SvnDepth::Files) && toDepth != SvnDepth::Exclude)
		throw gcnew ArgumentOutOfRangeException("toDepth", toDepth, SharpSvnStrings::CropToValidDepth);
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

    do_crop_baton dcb;
    dcb.ctx = CtxHandle;
    dcb.target_abspath = pool.AllocAbsoluteDirent(path);
    dcb.depth = (svn_depth_t)toDepth;

    svn_error_t *r = svn_wc__call_with_write_lock(
            perform_crop, &dcb,
            dcb.ctx->wc_ctx,
            dcb.target_abspath,
            TRUE,
            pool.Handle, pool.Handle);

	return args->HandleResult(this, r, path);
}