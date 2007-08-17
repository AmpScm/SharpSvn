#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Apr;
using namespace SharpSvn;


SvnException^ SvnNotifyEventArgs::Error::get()
{
	if(!_exception && _notify && _notify->err)
		_exception = SvnException::Create(_notify->err);

	return _exception;
}

SvnCommittingEventArgs::SvnCommittingEventArgs(const apr_array_header_t *commitItems, AprPool^ pool)
{
	if(!commitItems)
		throw gcnew ArgumentNullException("commitItems");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	this->_commitItems = commitItems;
	this->_pool = pool;
}

void SvnCommittingEventArgs::Detach(bool keepProperties)
{
	if(!_commitItems)
		return;

	try
	{
		if(keepProperties)
		{
			if(Items)
				for each(SvnCommitItem^ item in Items)
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

SvnCommitInfo::SvnCommitInfo(const svn_commit_info_t *commitInfo, AprPool^ pool)
{
	if(!commitInfo)
		throw gcnew ArgumentNullException("commitInfo");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");


	_revision = commitInfo->revision;

	apr_time_t when = 0;
	svn_error_t *err = svn_time_from_cstring(&when, commitInfo->date, pool->Handle); // pool is not used at this time (might be for errors in future versions)

	if(!err)
		_date = SvnBase::DateTimeFromAprTime(when);
	else
		_date = DateTime::MinValue;

	_author = SvnBase::Utf8_PtrToString(commitInfo->author);
	_postCommitError = commitInfo->post_commit_err ? SvnBase::Utf8_PtrToString(commitInfo->post_commit_err) : nullptr;
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



IList<SvnCommitItem^>^ SvnCommittingEventArgs::Items::get()
{
	if(!_items && _commitItems)
	{
		AprArray<SvnCommitItem^, SvnCommitItemMarshaller^>^ aprItems = gcnew AprArray<SvnCommitItem^, SvnCommitItemMarshaller^>(_commitItems, _pool);

		array<SvnCommitItem^>^ items = gcnew array<SvnCommitItem^>(aprItems->Count);

		aprItems->CopyTo(items, 0);

		_items = safe_cast<IList<SvnCommitItem^>^>(items);
	}

	return _items;
}
