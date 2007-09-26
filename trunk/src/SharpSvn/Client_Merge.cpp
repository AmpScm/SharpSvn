#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Merge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");

	Merge(targetPath, mergeFrom, mergeTo, gcnew SvnMergeArgs());
}

bool SvnClient::Merge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo, SvnMergeArgs^ args)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

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

	return args->HandleResult(this, r);
}

void SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnRevision^ from, SvnRevision^ to)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");

	Merge(targetPath, source, from, to, gcnew SvnMergeArgs());
}

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, SvnMergeArgs^ args)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_opt_revision_t pegRev = source->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);
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

	return args->HandleResult(this, r);
}

void SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnMergeRange^ mergeRange)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");

	SvnMergeArgs^ args = gcnew SvnMergeArgs();

	// if(!mergeRange->Inheritable)
	//		args->Depth = ??

	Merge(targetPath, source, mergeRange, args);
}

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnMergeRange^ mergeRange, SvnMergeArgs^ args)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	
	return SvnClient::Merge(targetPath, source, gcnew SvnRevision(mergeRange->Start), gcnew SvnRevision(mergeRange->End), args);	
}