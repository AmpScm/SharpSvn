// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using System::Collections::Generic::List;

SvnAppliedMergeInfo::SvnAppliedMergeInfo(SvnTarget^ target, apr_hash_t* mergeInfo, AprPool^ pool)
{
	// Pool is only available during the constructor
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!mergeInfo)
		throw gcnew ArgumentNullException("mergeInfo");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	_target = target;

	// mergeInfo: hash mapping <const char *> source URLs to an <apr_array_header_t *> list of <svn_merge_range_t>

	AppliedMergesList^ list = gcnew AppliedMergesList();

	for (apr_hash_index_t *hi = apr_hash_first(pool->Handle, mergeInfo); hi; hi = apr_hash_next(hi))
	{
		const char *pUri;
		apr_ssize_t klen;
		apr_array_header_t *ranges;
		apr_hash_this(hi, (const void **) &pUri, &klen, (void**)&ranges);

		if(pUri && ranges)
		{
			Uri^ uri;

			if(Uri::TryCreate(SvnBase::Utf8_PtrToString(pUri, (int)klen), UriKind::Absolute, uri))
			{
				list->Add(gcnew SvnAppliedMergeItem(uri, SvnAppliedMergeItem::CreateRangeList(ranges)));
			}
		}		
	}
	_appliedMerges = list;
}

SvnAvailableMergeInfo::SvnAvailableMergeInfo(SvnTarget^ target, apr_array_header_t* mergeInfo, AprPool^ pool)
{
	// Pool is only available during the constructor
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!mergeInfo)
		throw gcnew ArgumentNullException("mergeInfo");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	_target = target;
	_mergeRanges = SvnAppliedMergeItem::CreateRangeList(mergeInfo);
}

Collection<SvnMergeRange^>^ SvnAppliedMergeItem::CreateRangeList(apr_array_header_t *rangeList)
{
	if(!rangeList)
		throw gcnew ArgumentNullException("rangeList");

	array<SvnMergeRange^>^ ranges = gcnew array<SvnMergeRange^>(rangeList->nelts);

	const svn_merge_range_t** mrgRange = (const svn_merge_range_t**)rangeList->elts;

	for(int i = 0; i < rangeList->nelts; i++)
		ranges[i] = gcnew SvnMergeRange(mrgRange[i]->start, mrgRange[i]->end, 0 != mrgRange[i]->inheritable);

	return gcnew Collection<SvnMergeRange^>(safe_cast<IList<SvnMergeRange^>^>(ranges));
}