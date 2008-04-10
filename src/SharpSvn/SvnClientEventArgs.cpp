// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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

SvnCommittingEventArgs::SvnCommittingEventArgs(const apr_array_header_t *commitItems, AprPool^ pool)
{
	if (!commitItems)
		throw gcnew ArgumentNullException("commitItems");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	this->_commitItems = commitItems;
	this->_pool = pool;
}

void SvnCommittingEventArgs::Detach(bool keepProperties)
{
	if (!_commitItems)
		return;

	try
	{
		if (keepProperties)
		{
			if (Items)
				for each (SvnCommitItem^ item in Items)
				{
					item->Detach(true);
				}
		}
	}
	finally
	{
		_commitItems = nullptr;
		_pool = nullptr;
	}
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

		return gcnew SvnCommitItem(*ppcCommitItem);
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

SvnPropertyCollection^ SvnLogEventArgs::CustomProperties::get()
{
	if (!_customProperties && _entry && _entry->revprops)
	{
		_customProperties = gcnew SvnPropertyCollection();

		for (apr_hash_index_t* hi = apr_hash_first(_pool->Handle, _entry->revprops); hi; hi = apr_hash_next(hi))
		{
			const char* pKey;
			apr_ssize_t keyLen;
			svn_string_t *pValue;

			apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&pValue);

			SvnPropertyValue^ pv = SvnPropertyValue::Create(pKey, pValue, nullptr);

			_customProperties->Add(pv);
		}
	}
	return _customProperties;
}
