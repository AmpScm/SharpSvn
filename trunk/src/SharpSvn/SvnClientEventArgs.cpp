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
#include <svn_config.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;


SvnException^ SvnNotifyEventArgs::Error::get()
{
	if (!_exception && _notify && _notify->err)
		_exception = static_cast<SvnException^>(SvnException::Create(_notify->err, false));

	return _exception;
}

//Fxcop bug:
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.SvnCommitItemMarshaller")];

ref class SvnCommitItemMarshaller sealed : public IItemMarshaller<SvnCommitItem^>
{
public:
	SvnCommitItemMarshaller()
	{}

	property int ItemSize
	{
		virtual int get()
		{
			return sizeof(svn_client_commit_item3_t*);
		}
	}

	virtual void Write(SvnCommitItem^ value, void* ptr, AprPool^ pool)
	{
		UNUSED_ALWAYS(value);
		UNUSED_ALWAYS(ptr);
		UNUSED_ALWAYS(pool);
		throw gcnew NotImplementedException();
	}

	virtual SvnCommitItem^ Read(const void* ptr, AprPool^ pool)
	{
		UNUSED_ALWAYS(pool);
		const svn_client_commit_item3_t** ppcCommitItem = (const svn_client_commit_item3_t**)ptr;

		return gcnew SvnCommitItem(*ppcCommitItem, pool);
	}
};



SvnCommitItemCollection^ SvnCommittingEventArgs::Items::get()
{
	if (!_items && _commitItems)
	{
		AprArray<SvnCommitItem^, SvnCommitItemMarshaller^>^ aprItems = gcnew AprArray<SvnCommitItem^, SvnCommitItemMarshaller^>(_commitItems, _pool);

		array<SvnCommitItem^>^ items = gcnew array<SvnCommitItem^>(aprItems->Count);

		aprItems->CopyTo(items, 0);

		_items = gcnew SvnCommitItemCollection(safe_cast<IList<SvnCommitItem^>^>(items));
	}

	return _items;
}

SvnMergeRange^ SvnNotifyEventArgs::MergeRange::get()
{
	if (!_mergeRange && _notify && _notify->merge_range)
		_mergeRange = gcnew SvnMergeRange(_notify->merge_range->start, _notify->merge_range->end, _notify->merge_range->inheritable != 0);

	return _mergeRange;
}
