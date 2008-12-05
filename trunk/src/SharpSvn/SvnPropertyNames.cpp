// $Id: SvnMergeRangeCollection.cpp 631 2008-07-28 21:40:00Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "SvnNames.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using System::Collections::Generic::List;


bool SvnPropertyNames::TryParseDate(String^ date, [Out] DateTime% value)
{
	if (String::IsNullOrEmpty(date))
		throw gcnew ArgumentNullException("date");

	AprPool pool(SvnBase::SmallThreadPool);

	apr_time_t tm = 0;
	svn_error_t* r = svn_time_from_cstring(&tm, pool.AllocString(date), pool.Handle);

	if (r)
	{
		value = DateTime::MinValue;
		svn_error_clear(r);
		return false;
	}
	else
	{
		value = SvnBase::DateTimeFromAprTime(tm);

		return true;
	}
}

String^ SvnPropertyNames::FormatDate(DateTime value)
{
	AprPool pool(SvnBase::SmallThreadPool);

	apr_time_t tm = SvnBase::AprTimeFromDateTime(value);
	const char* rslt = svn_time_to_cstring(tm, pool.Handle);

	return SvnBase::Utf8_PtrToString(rslt);
}