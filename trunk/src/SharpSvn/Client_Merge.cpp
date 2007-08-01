#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Merge(SvnTarget^ mergeFrom, SvnTarget^ mergeTo, String^ targetPath)
{
	if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");
	else if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");

	Merge(mergeFrom, mergeTo, targetPath, gcnew SvnMergeArgs());
}

bool SvnClient::Merge(SvnTarget^ mergeFrom, SvnTarget^ mergeTo, String^ targetPath, SvnMergeArgs^ args)
{
	if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");
	else if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		apr_array_header_t *merge_options = nullptr;

		svn_opt_revision_t mergeFromRev = mergeFrom->Revision->ToSvnRevision();
		svn_opt_revision_t mergeToRev = mergeTo->Revision->ToSvnRevision();

		svn_error_t *r = svn_client_merge3(
			pool.AllocString(mergeFrom->TargetName),
			&mergeFromRev,
			pool.AllocString(mergeTo->TargetName),
			&mergeToRev,
			pool.AllocPath(targetPath),
			(svn_depth_t)args->Depth,
			args->IgnoreAncestry,
			args->Force,
			args->RecordOnly,
			args->DryRun,
			merge_options,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

void SvnClient::Merge(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, String^ targetPath)
{
	if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!from)
		throw gcnew ArgumentNullException("to");

	Merge(source, from, to, targetPath, gcnew SvnMergeArgs());
}

bool SvnClient::Merge(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, String^ targetPath, SvnMergeArgs^ args)
{
	if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!from)
		throw gcnew ArgumentNullException("to");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_opt_revision_t pegRev = source->Revision->ToSvnRevision();
		svn_opt_revision_t fromRev = from->ToSvnRevision();
		svn_opt_revision_t toRev = to->ToSvnRevision();

		svn_error_t *r = svn_client_merge_peg3(
			pool.AllocString(source->TargetName),
			&fromRev,
			&toRev,
			&pegRev,
			pool.AllocPath(targetPath),
			(svn_depth_t)args->Depth,
			args->IgnoreAncestry,
			args->Force,
			args->RecordOnly,
			args->DryRun,
			args->MergeArguments ? AllocArray(args->MergeArguments, %pool) : nullptr,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
