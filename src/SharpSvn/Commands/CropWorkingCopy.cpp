#include "stdafx.h"
#include "SvnAll.h"
#include "Args/CropWorkingCopy.h"

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

bool SvnClient::CropWorkingCopy(System::String ^path, SvnDepth toDepth, SvnCropWorkingCopyArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (toDepth < SvnDepth::Exclude || toDepth >= SvnDepth::Files)
		throw gcnew ArgumentOutOfRangeException("toDepth", toDepth, SharpSvnStrings::CropToValidDepth);
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_client_ctx_t* ctx = CtxHandle;
	svn_wc_adm_access_t *adm_access = nullptr;
	const svn_wc_entry_t *entry = nullptr;
	const char* pPath = pool.AllocDirent(path);

	svn_error_t *r = svn_wc_adm_probe_open3(
							&adm_access,
							nullptr,
							pPath,
							true,
							-1,
							ctx->cancel_func,
							ctx->cancel_baton,
							pool.Handle);

	if (r)
		return args->HandleResult(this, r, path);

	r = svn_wc_entry(&entry, pPath, adm_access, FALSE, pool.Handle);

	if (r)
		return args->HandleResult(this, r, path);

	r = svn_wc_crop_tree(
			adm_access,
			entry->name,
			(svn_depth_t)toDepth,
			ctx->notify_func2,
			ctx->notify_baton2,
			ctx->cancel_func,
			ctx->cancel_baton,
			pool.Handle);

	return args->HandleResult(this, r, path);
}