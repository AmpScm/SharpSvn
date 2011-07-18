// $Id: AprArray.h 534 2008-04-14 22:52:59Z rhuijben $
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

#pragma once
#include "stdafx.h"
#include "SvnAll.h"
#include "SvnNames.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using System::Collections::Generic::List;

[module: SuppressMessage("Microsoft.Naming", "CA1704:IdentifiersShouldBeSpelledCorrectly", Scope="member", Target="SharpSvn.SvnPropertyNames.#SvnEolStyle", MessageId="Eol")];
[module: SuppressMessage("Microsoft.Naming", "CA1704:IdentifiersShouldBeSpelledCorrectly", Scope="member", Target="SharpSvn.SvnPropertyNames.#SvnSyncFromUuid", MessageId="Uuid")];
[module: SuppressMessage("Microsoft.Naming", "CA1709:IdentifiersShouldBeCasedCorrectly", Scope="member", Target="SharpSvn.SvnKeywords.#HeadURL", MessageId="URL")];
[module: SuppressMessage("Microsoft.Naming", "CA1709:IdentifiersShouldBeCasedCorrectly", Scope="member", Target="SharpSvn.SvnKeywords.#URL", MessageId="URL")];

[module: SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Scope="member", Target="SharpSvn.SvnPropertyNames.#AllSvnRevisionProperties")]
[module: SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Scope="member", Target="SharpSvn.SvnPropertyNames.#TortoiseSvnDirectoryProperties")]
[module: SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Scope="member", Target="SharpSvn.SvnKeywords.#PredefinedKeywords")]
[module: SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes", Scope="member", Target="SharpSvn.SvnConfigNames.#SvnDefaultGlobalIgnores")];

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