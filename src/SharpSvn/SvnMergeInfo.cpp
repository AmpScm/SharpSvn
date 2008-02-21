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

svn_merge_range_t* SvnMergeRange::AllocMergeRange(AprPool^ pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	svn_merge_range_t* range = (svn_merge_range_t*)pool->AllocCleared(sizeof(svn_merge_range_t));

	range->start = (svn_revnum_t)Start;
	range->end = (svn_revnum_t)End;
	range->inheritable = Inheritable;

	return range;
}

