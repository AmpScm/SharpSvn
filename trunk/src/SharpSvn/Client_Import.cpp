#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


bool SvnClient::Import(String^ path, Uri^ target)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");

	return Import(path, target, gcnew SvnImportArgs());
}

bool SvnClient::Import(String^ path, Uri^ target, [Out] SvnCommitInfo^% commitInfo)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");

	return Import(path, target, gcnew SvnImportArgs(), commitInfo);
}

bool SvnClient::Import(String^ path, Uri^ target, SvnImportArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo;

	return Import(path, target, args, commitInfo);
}

bool SvnClient::Import(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	if(RemoteImport(path, target, args, commitInfo))
	{
		SvnCheckOutArgs^ aa = gcnew SvnCheckOutArgs();
		aa->ThrowOnError = args->ThrowOnError;
		aa->Depth = args->Depth;

		aa->AllowUnversionedObstructions = true; // This is the trick
		aa->SvnError += gcnew EventHandler<SvnErrorEventArgs^>(args, &SvnImportArgs::HandleOnSvnError);

		try
		{
			return CheckOut(target, path, aa);
		}
		finally
		{
			args->Exception = aa->Exception;
		}
	}
	else
		return false;
}

bool SvnClient::RemoteImport(String^ path, Uri^ target)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");

	return RemoteImport(path, target, gcnew SvnImportArgs());
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, [Out] SvnCommitInfo^% commitInfo)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");

	return RemoteImport(path, target, gcnew SvnImportArgs(), commitInfo);
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo;

	return RemoteImport(path, target, args, commitInfo);
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	commitInfo = nullptr;

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t *commit_info = nullptr;

	svn_error_t *r = svn_client_import3(
		&commit_info,
		pool.AllocPath(path),
		pool.AllocCanonical(target->ToString()),
		(svn_depth_t)args->Depth,
		args->NoIgnore,
		args->IgnoreUnknownNodeTypes,
		CtxHandle,
		pool.Handle);

	if(commit_info)
		commitInfo = gcnew SvnCommitInfo(commit_info, %pool);

	return args->HandleResult(this, r);
}
