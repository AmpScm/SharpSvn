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


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnMergeItemCollection.#TryDiff(System.Collections.Generic.ICollection`1<SharpSvn.SvnMergeItem>,SharpSvn.SvnMergeItemCollection&,SharpSvn.SvnMergeItemCollection&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnMergeItemCollection.#TryDiff(System.Collections.Generic.ICollection`1<SharpSvn.SvnMergeItem>,SharpSvn.SvnMergeDiffArgs,SharpSvn.SvnMergeItemCollection&,SharpSvn.SvnMergeItemCollection&)", MessageId="2#")];

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

svn_mergeinfo_t SvnMergeItemCollection::AllocMergeInfo(AprPool^ pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	// See svn_mergeinfo.h for the formal definition of the svn_mergeinfo_t type

	svn_mergeinfo_t result = apr_hash_make(pool->Handle);

	for each (SvnMergeItem^ i in this)
	{
		String^ uriValue = SvnBase::UriToCanonicalString(i->Uri);

		const svn_string_t* uri = pool->AllocSvnString(uriValue);
		apr_array_header_t* range = i->MergeRanges->AllocMergeRange(pool);

		apr_hash_set(result, uri, sizeof(svn_string_t), range);
	}

	return result;
}


// TODO: Most of these methods could be implemented in 100% managed code.
//		 The implementation of this methods is critical for merging functions, so should be tested

bool SvnMergeItemCollection::TryParse(String^ input, [Out] SvnMergeItemCollection^% items)
{
	if (String::IsNullOrEmpty(input))
		throw gcnew ArgumentNullException("input");

	AprPool pool(SvnBase::SmallThreadPool);

	svn_mergeinfo_t mergeInfo = nullptr;

	svn_error_t* r = svn_mergeinfo_parse(&mergeInfo, pool.AllocString(input), pool.Handle);

	if (r)
	{
		svn_error_clear(r);
		return false;
	}

	items = gcnew SvnMergeItemCollection(mergeInfo, %pool);
	return true;
}

bool SvnMergeItemCollection::TryDiff(ICollection<SvnMergeItem^>^ to, [Out] SvnMergeItemCollection^% added, [Out] SvnMergeItemCollection^% removed)
{
	if (!to)
		throw gcnew ArgumentNullException("to");

	return TryDiff(to, gcnew SvnMergeDiffArgs(), added, removed);
}

bool SvnMergeItemCollection::TryDiff(ICollection<SvnMergeItem^>^ to, SvnMergeDiffArgs^ args, [Out] SvnMergeItemCollection^% added, [Out] SvnMergeItemCollection^% removed)
{
	if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	added = nullptr;
	removed = nullptr;

	AprPool pool(SvnBase::SmallThreadPool);

	SvnMergeItemCollection^ toCol = dynamic_cast<SvnMergeItemCollection^>(to);

	if (!toCol)
		toCol = gcnew SvnMergeItemCollection(to);

	svn_mergeinfo_t addedPtr = nullptr;
	svn_mergeinfo_t removedPtr = nullptr;

	svn_error_t* r = svn_mergeinfo_diff2(&removedPtr, &addedPtr,
										 AllocMergeInfo(%pool),
										 toCol->AllocMergeInfo(%pool),
										 args->ConsiderInheritance,
										 pool.Handle, pool.Handle);

	if (r)
	{
		svn_error_clear(r);
		return false;
	}

	if (addedPtr)
		added = gcnew SvnMergeItemCollection(addedPtr, %pool);
	else
		added = gcnew SvnMergeItemCollection();

	if (removedPtr)
		removed = gcnew SvnMergeItemCollection(removedPtr, %pool);
	else
		removed = gcnew SvnMergeItemCollection();

	return true;
}

bool SvnMergeItemCollection::TryRemove(ICollection<SvnMergeItem^>^ items, [Out] SvnMergeItemCollection^% rest)
{
	if (!items)
		throw gcnew ArgumentNullException("items");

	return TryRemove(items, gcnew SvnMergeRemoveArgs(), rest);
}

bool SvnMergeItemCollection::TryRemove(ICollection<SvnMergeItem^>^ items, SvnMergeRemoveArgs^ args, [Out] SvnMergeItemCollection^% rest)
{
	if (!items)
		throw gcnew ArgumentNullException("items");

	GC::KeepAlive(args);

	SvnMergeItemCollection^ coll = dynamic_cast<SvnMergeItemCollection^>(items);
	if (!coll)
		coll = gcnew SvnMergeItemCollection(items);

	rest = nullptr;

	AprPool pool(SvnBase::SmallThreadPool);

	svn_mergeinfo_t result = nullptr;

	svn_error_t* r = svn_mergeinfo_remove2(&result,
                                           coll->AllocMergeInfo(%pool),
                                           AllocMergeInfo(%pool),
                                           args->ConsiderInheritance,
                                           pool.Handle,
                                           pool.Handle);

	if (r)
	{
		svn_error_clear(r);
		return false;
	}

	if (result)
		rest = gcnew SvnMergeItemCollection(result, %pool);
	else
		rest = gcnew SvnMergeItemCollection();

	return true;
}

bool SvnMergeItemCollection::TryIntersect(ICollection<SvnMergeItem^>^ to, [Out] SvnMergeItemCollection^% intersected)
{
	return TryIntersect(to, gcnew SvnMergeIntersectArgs(), intersected);
}

bool SvnMergeItemCollection::TryIntersect(ICollection<SvnMergeItem^>^ to, SvnMergeIntersectArgs^ args, [Out] SvnMergeItemCollection^% intersected)
{
	if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnMergeItemCollection^ coll = dynamic_cast<SvnMergeItemCollection^>(to);

	if (!coll)
		coll = gcnew SvnMergeItemCollection(to);

	intersected = nullptr;

	AprPool pool(SvnBase::SmallThreadPool);

	svn_mergeinfo_t result = nullptr;

	svn_error_t* r = svn_mergeinfo_intersect2(&result,
											  AllocMergeInfo(%pool),
											  coll->AllocMergeInfo(%pool),
											  args->ConsiderInheritance,
											  pool.Handle, pool.Handle);

	if (r)
	{
		svn_error_clear(r);
		return false;
	}

	if (result)
		intersected = gcnew SvnMergeItemCollection(result, %pool);
	else
		intersected = gcnew SvnMergeItemCollection();

	return true;
}

String^ SvnMergeItemCollection::ToString()
{
	AprPool pool(SvnBase::SmallThreadPool);

	svn_string_t* result = nullptr;

	svn_error_t* r = svn_mergeinfo_to_string(&result, AllocMergeInfo(%pool), pool.Handle);

	if (r)
	{
		svn_error_clear(r);
		return "";
	}
	else if (!result || !result->data)
		return "";

	return SvnBase::Utf8_PtrToString(result->data, (int)result->len);
}


Uri^ SvnMergeItemCollection::GetKeyForItem(SvnMergeItem^ item)
{
	return item ? item->Uri : nullptr;
}