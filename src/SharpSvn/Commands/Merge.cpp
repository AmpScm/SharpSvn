// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Merge.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnRevisionRange^ mergeRange)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");

	return Merge(targetPath, source, NewSingleItemCollection(mergeRange), gcnew SvnMergeArgs());
}

bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, SvnRevisionRange^ mergeRange, SvnMergeArgs^ args)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return Merge(targetPath, source, NewSingleItemCollection(mergeRange), args);
}

generic<typename TRevisionRange> where TRevisionRange : SvnRevisionRange
bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, ICollection<TRevisionRange>^ mergeRange)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");

	return Merge<TRevisionRange>(targetPath, source, mergeRange, gcnew SvnMergeArgs());
}

generic<typename TRevisionRange> where TRevisionRange : SvnRevisionRange
ref class RevisionRangeMarshaller sealed : public IItemMarshaller<TRevisionRange>
{
public:
	property int ItemSize
	{
		virtual int get()
		{
			return sizeof(svn_opt_revision_range_t*);
		}
	}

	virtual void Write(TRevisionRange value, void* ptr, AprPool^ pool)
	{
		svn_opt_revision_range_t** ppRange = (svn_opt_revision_range_t**)ptr;
		*ppRange = (svn_opt_revision_range_t*)pool->Alloc(sizeof(svn_opt_revision_range_t));

		(*ppRange)->start = value->StartRevision->ToSvnRevision();
		(*ppRange)->end = value->EndRevision->ToSvnRevision();
	}

	virtual TRevisionRange Read(const void* ptr, AprPool^ pool)
	{
		UNUSED_ALWAYS(ptr);
		UNUSED_ALWAYS(pool);

		throw gcnew NotImplementedException();
	}
};

generic<typename TRevisionRange> where TRevisionRange : SvnRevisionRange
bool SvnClient::Merge(String^ targetPath, SvnTarget^ source, ICollection<TRevisionRange>^ mergeRange, SvnMergeArgs^ args)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!mergeRange)
		throw gcnew ArgumentNullException("mergeRange");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<TRevisionRange, RevisionRangeMarshaller<TRevisionRange>^>^ mergeList 
		= gcnew AprArray<TRevisionRange, RevisionRangeMarshaller<TRevisionRange>^>(mergeRange, %pool);

	svn_error_t *r = svn_client_merge_peg3(
		pool.AllocString(source->SvnTargetName),
		mergeList->Handle,
		source->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(%pool),
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
