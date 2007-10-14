// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Merge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");

	return Merge(targetPath, mergeFrom, mergeTo, gcnew SvnMergeArgs());
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

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnRevisionRange^ mergeRange)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");
	
	return Merge(targetPath, source, NewSingleItemCollection(mergeRange), gcnew SvnMergeArgs());
}

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnRevisionRange^ mergeRange, SvnMergeArgs^ args)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	
	return Merge(targetPath, source, NewSingleItemCollection(mergeRange), args);
}

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, ICollection<SvnRevisionRange^>^ mergeRange)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");
	
	return Merge(targetPath, source, mergeRange, gcnew SvnMergeArgs());
}

ref class RevisionRangeMarshaller sealed : public IItemMarshaller<SvnRevisionRange^>
{
public:
	property int ItemSize
	{
		virtual int get()
		{
			return sizeof(svn_opt_revision_range_t*);
		}
	}

	virtual void Write(SvnRevisionRange^ value, void* ptr, AprPool^ pool)
	{
		svn_opt_revision_range_t** ppRange = (svn_opt_revision_range_t**)ptr;
		*ppRange = (svn_opt_revision_range_t*)pool->Alloc(sizeof(svn_opt_revision_range_t));

		(*ppRange)->start = value->StartRevision->ToSvnRevision();
		(*ppRange)->end = value->EndRevision->ToSvnRevision();
	}

	virtual SvnRevisionRange^ Read(const void* ptr, AprPool^ pool)
	{
		UNUSED_ALWAYS(ptr);
		UNUSED_ALWAYS(pool);

		throw gcnew NotImplementedException();
	}
};

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, ICollection<SvnRevisionRange^>^ mergeRange, SvnMergeArgs^ args)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<SvnRevisionRange^, RevisionRangeMarshaller^>^ mergeList = gcnew AprArray<SvnRevisionRange^, RevisionRangeMarshaller^>(mergeRange, %pool);
	svn_opt_revision_t pegRev = source->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);
	
	svn_error_t *r = svn_client_merge_peg3(
		pool.AllocString(source->TargetName),
		mergeList->Handle,
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

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, ICollection<SvnMergeRange^>^ mergeRange, SvnMergeArgs^ args)
{
	if(!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");

	array<SvnRevisionRange^>^ revs = gcnew array<SvnRevisionRange^>(mergeRange->Count);

	mergeRange->CopyTo(safe_cast<array<SvnMergeRange^>^>(revs), 0);

	return Merge(targetPath, source, safe_cast<ICollection<SvnRevisionRange^>^>(revs), args);
}

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, ICollection<SvnMergeRange^>^ mergeRange)
{
	return Merge(targetPath, source, mergeRange, gcnew SvnMergeArgs());
}
