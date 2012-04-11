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

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnClientConfiguration::LogMessageRequired::get()
{
	return !_client->_noLogMessageRequired;
}
void SvnClientConfiguration::LogMessageRequired::set(bool value)
{
	_client->_noLogMessageRequired = !value;
}

bool SvnClientConfiguration::LoadSvnMimeTypes::get()
{
	return !_client->_dontLoadMimeFile;
}

void SvnClientConfiguration::LoadSvnMimeTypes::set(bool value)
{
	_client->_dontLoadMimeFile = !value;
}

bool SvnClientConfiguration::EnableSharpPlink::get()
{
	return !_client->_dontEnablePlink;
}

void SvnClientConfiguration::EnableSharpPlink::set(bool value)
{
	_client->_dontEnablePlink = !value;
}

bool SvnClientConfiguration::UseSubversionDiff::get()
{
	return !_client->_useUserDiff;
}

void SvnClientConfiguration::UseSubversionDiff::set(bool value)
{
	_client->_useUserDiff = !value;
}

SvnOverride SvnClientConfiguration::KeepAllExtensionsOnConflict::get()
{
    return _client->_keepAllExtensionsOnConflict;
}

void SvnClientConfiguration::KeepAllExtensionsOnConflict::set(SvnOverride value)
{
    _client->_keepAllExtensionsOnConflict = value;
}

System::Collections::Generic::IEnumerable<String^>^ SvnClientConfiguration::GlobalIgnorePattern::get()
{
	_client->EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(SmallThreadPool);

	apr_array_header_t *ignores;

	svn_wc_get_default_ignores(&ignores, _client->CtxHandle->config, pool.Handle);
	List<String^>^ ignList = gcnew List<String^>();

	for (int i = 0; i < ignores->nelts; i++)
	{
		const char *ign = APR_ARRAY_IDX(ignores, i, const char*);

		ignList->Add(Utf8_PtrToString(ign));
	}

	return ignList->AsReadOnly();
}

void SvnClientConfiguration::SetOption(String^ file, String^ section, String^ option, String^ value)
{
    if (!file)
		throw gcnew ArgumentNullException("file");
	else if (!section)
		throw gcnew ArgumentNullException("section");
	else if (!option)
		throw gcnew ArgumentNullException("option");
	else if (!value)
		throw gcnew ArgumentNullException("value");

    _client->SetConfigurationOption(file, section, option, value);
}

