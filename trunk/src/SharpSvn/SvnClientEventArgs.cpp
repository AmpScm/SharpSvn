#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Apr;
using namespace SharpSvn;


SvnException^ SvnClientNotifyEventArgs::Error::get()
{
	if(!_exception && _notify && _notify->err)
		_exception = SvnException::Create(_notify->err);

	return _exception;
}

SvnClientBeforeCommitEventArgs::SvnClientBeforeCommitEventArgs(const apr_array_header_t *commitItems, AprPool^ pool)
{
	if(!commitItems)
		throw gcnew ArgumentNullException("commitItems");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	this->_commitItems = commitItems;
	this->_pool = pool;
}

void SvnClientBeforeCommitEventArgs::Detach(bool keepProperties)
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

SvnCommitInfo::SvnCommitInfo(const svn_commit_info_t *commitInfo)
{
	if(!commitInfo)
		throw gcnew ArgumentNullException("commitInfo");

	_revision = commitInfo->revision;
	_date = DateTime::Parse(SvnBase::Utf8_PtrToString(commitInfo->date));
	_author = SvnBase::Utf8_PtrToString(commitInfo->author);
	_postCommitError = commitInfo->post_commit_err ? SvnBase::Utf8_PtrToString(commitInfo->post_commit_err) : nullptr;
}

IList<SvnCommitItem^>^ SvnClientBeforeCommitEventArgs::Items::get()
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
