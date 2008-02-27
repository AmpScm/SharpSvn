// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using System::Collections::Generic::List;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnMergeRangeCollection.#TryDiff(System.Collections.Generic.ICollection`1<SharpSvn.SvnMergeRange>,SharpSvn.SvnMergeRangeCollection&,SharpSvn.SvnMergeRangeCollection&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnMergeRangeCollection.#TryDiff(System.Collections.Generic.ICollection`1<SharpSvn.SvnMergeRange>,SharpSvn.SvnMergeDiffArgs,SharpSvn.SvnMergeRangeCollection&,SharpSvn.SvnMergeRangeCollection&)", MessageId="2#")];

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

namespace SharpSvn {
	namespace Implementation {
		ref class SvnMergeRangeMarshaller  : public IItemMarshaller<SvnMergeRange^>
		{
		public:
			property int ItemSize
			{
				virtual int get()
				{
					return sizeof(svn_merge_range_t*);
				}
			}

			virtual void Write(SvnMergeRange^ value, void* ptr, AprPool^ pool)
			{
				const svn_merge_range_t** ppStr = (const svn_merge_range_t**)ptr;

				*ppStr = value->AllocMergeRange(pool);
			}

			virtual SvnMergeRange^ Read(const void* ptr, AprPool^ pool)
			{
				UNUSED_ALWAYS(ptr);
				UNUSED_ALWAYS(pool);
				throw gcnew InvalidOperationException();
			}
		};
	}
}


apr_array_header_t* SvnMergeRangeCollection::AllocMergeRange(AprPool^ pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	AprArray<SvnMergeRange^, SvnMergeRangeMarshaller^>^ arr = gcnew AprArray<SvnMergeRange^, SvnMergeRangeMarshaller^>(this, pool);

	return arr->Handle;
}

// TODO: Most of these methods could be implemented in 100% managed code. 
//		 The implementation of this methods is critical for merging functions, so should be tested


bool SvnMergeRangeCollection::TryDiff(ICollection<SvnMergeRange^>^ to, [Out] SvnMergeRangeCollection^% added, [Out] SvnMergeRangeCollection^% removed)
{
	if(!to)
		throw gcnew ArgumentNullException("to");
	
	return TryDiff(to, gcnew SvnMergeDiffArgs(), added, removed);
}

bool SvnMergeRangeCollection::TryDiff(ICollection<SvnMergeRange^>^ to, SvnMergeDiffArgs^ args, [Out] SvnMergeRangeCollection^% added, [Out] SvnMergeRangeCollection^% removed)
{
	if(!to)
		throw gcnew ArgumentNullException("to");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	added = nullptr;
	removed = nullptr;

	AprPool pool(SvnBase::SmallThreadPool);

	SvnMergeRangeCollection^ toCol = dynamic_cast<SvnMergeRangeCollection^>(to);

	if(!toCol)
		toCol = gcnew SvnMergeRangeCollection(to);

	apr_array_header_t* addedPtr = nullptr;
	apr_array_header_t* removedPtr = nullptr;

	svn_error_t* r = svn_rangelist_diff(&removedPtr, &addedPtr, AllocMergeRange(%pool), toCol->AllocMergeRange(%pool), args->ConsiderInheritance, pool.Handle);

	if(r)
	{
		svn_error_clear(r);		
		return false;
	}

	if (addedPtr)
		added = SvnMergeRangeCollection::Create(addedPtr);
	else
		added = gcnew SvnMergeRangeCollection();

	if(removedPtr)
		removed = SvnMergeRangeCollection::Create(removedPtr);
	else
		removed = gcnew SvnMergeRangeCollection();

	return true;
}

bool SvnMergeRangeCollection::TryRemove(ICollection<SvnMergeRange^>^ items, [Out] SvnMergeRangeCollection^% rest)
{
	if (!items)
		throw gcnew ArgumentNullException("items");

	return TryRemove(items, gcnew SvnMergeRemoveArgs(), rest);
}

bool SvnMergeRangeCollection::TryRemove(ICollection<SvnMergeRange^>^ items, SvnMergeRemoveArgs^ args, [Out] SvnMergeRangeCollection^% rest)
{
	if (!items)
		throw gcnew ArgumentNullException("items");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnMergeRangeCollection^ coll = dynamic_cast<SvnMergeRangeCollection^>(items);
	if(!coll)
		coll = gcnew SvnMergeRangeCollection(items);

	rest = nullptr;

	AprPool pool(SvnBase::SmallThreadPool);

	apr_array_header_t* result = nullptr;

	svn_error_t* r = svn_rangelist_remove(&result, coll->AllocMergeRange(%pool), AllocMergeRange(%pool), args->ConsiderInheritance, pool.Handle);

	if(r)
	{
		svn_error_clear(r);		
		return false;
	}

	if(result)
		rest = SvnMergeRangeCollection::Create(result);
	else
		rest = gcnew SvnMergeRangeCollection();

	return true;
}

bool SvnMergeRangeCollection::TryIntersect(ICollection<SvnMergeRange^>^ to, [Out] SvnMergeRangeCollection^% intersected)
{
	if (!to)
		throw gcnew ArgumentNullException("to");

	SvnMergeRangeCollection^ coll = dynamic_cast<SvnMergeRangeCollection^>(to);

	if(!coll)
		coll = gcnew SvnMergeRangeCollection(to);

	intersected = nullptr;

	AprPool pool(SvnBase::SmallThreadPool);

	apr_array_header_t* result = nullptr;

	svn_error_t* r = svn_rangelist_intersect(&result, AllocMergeRange(%pool), coll->AllocMergeRange(%pool), pool.Handle);

	if(r)
	{
		svn_error_clear(r);		
		return false;
	}

	if(result)
		intersected = SvnMergeRangeCollection::Create(result);
	else
		intersected = gcnew SvnMergeRangeCollection();

	return true;
}

String^ SvnMergeRangeCollection::ToString()
{
	AprPool pool(SvnBase::SmallThreadPool);

	svn_string_t* result = nullptr;

	svn_error_t* r = svn_rangelist_to_string(&result, this->AllocMergeRange(%pool), pool.Handle);

	if(r)
	{
		svn_error_clear(r);
		return "";
	}
	else if(!result || !result->data)
		return "";
	
	return SvnBase::Utf8_PtrToString(result->data, result->len);
}

