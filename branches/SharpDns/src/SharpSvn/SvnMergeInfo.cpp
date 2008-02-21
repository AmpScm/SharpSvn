// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using System::Collections::Generic::List;

SvnAppliedMergeInfo::SvnAppliedMergeInfo(SvnTarget^ target, svn_mergeinfo_t mergeInfo, AprPool^ pool)
{
	// Pool is only available during the constructor
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!mergeInfo)
		throw gcnew ArgumentNullException("mergeInfo");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	_target = target;
	_appliedMerges = gcnew SvnMergeItemCollection(mergeInfo, pool);
}

SvnAvailableMergeInfo::SvnAvailableMergeInfo(SvnTarget^ target, apr_array_header_t* mergeInfo, AprPool^ pool)
{
	// Pool is only available during the constructor
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!mergeInfo)
		throw gcnew ArgumentNullException("mergeInfo");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	_target = target;
	_mergeRanges = SvnMergeRangeCollection::Create(mergeInfo);
}

SvnMergeRangeCollection^ SvnMergeRangeCollection::Create(apr_array_header_t *rangeList)
{
	if (!rangeList)
		throw gcnew ArgumentNullException("rangeList");

	array<SvnMergeRange^>^ ranges = gcnew array<SvnMergeRange^>(rangeList->nelts);

	const svn_merge_range_t** mrgRange = (const svn_merge_range_t**)rangeList->elts;

	for (int i = 0; i < rangeList->nelts; i++)
		ranges[i] = gcnew SvnMergeRange(mrgRange[i]->start, mrgRange[i]->end, 0 != mrgRange[i]->inheritable);

	return gcnew SvnMergeRangeCollection(safe_cast<IList<SvnMergeRange^>^>(ranges));
}

SvnMergeItemCollection::SvnMergeItemCollection(svn_mergeinfo_t mergeInfo, AprPool^ pool)
{
	if (!mergeInfo)
		throw gcnew ArgumentNullException("mergeInfo");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	for (apr_hash_index_t *hi = apr_hash_first(pool->Handle, mergeInfo); hi; hi = apr_hash_next(hi))
	{
		const char *pUri;
		apr_ssize_t klen;
		apr_array_header_t *ranges;
		apr_hash_this(hi, (const void **) &pUri, &klen, (void**)&ranges);

		if (pUri && ranges)
		{
			Add(gcnew SvnMergeItem(SvnBase::Utf8_PtrToUri(pUri, SvnNodeKind::Unknown), SvnMergeRangeCollection::Create(ranges)));
		}
	}
}

bool SvnMergeItemCollection::TryParse(String^ input, [Out] SvnMergeItemCollection^% items)
{
	if(String::IsNullOrEmpty(input))
		throw gcnew ArgumentNullException("input");

	AprPool pool;

	svn_mergeinfo_t mergeInfo = nullptr;

	svn_error_t* r = svn_mergeinfo_parse(&mergeInfo, pool.AllocString(input), pool.Handle);

	if(r)
	{
		svn_error_clear(r);
		return false;
	}

	items = gcnew SvnMergeItemCollection(mergeInfo, %pool);
	return true;
}

Uri^ SvnMergeItemCollection::GetKeyForItem(SvnMergeItem^ item)
{
	return item ? item->Uri : nullptr;
}
