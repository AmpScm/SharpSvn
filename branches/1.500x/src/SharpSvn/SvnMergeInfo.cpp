// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
	else if (!mergeInfo)
		throw gcnew ArgumentNullException("mergeInfo");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	_target = target;
	_appliedMerges = gcnew SvnMergeItemCollection(mergeInfo, pool);
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

