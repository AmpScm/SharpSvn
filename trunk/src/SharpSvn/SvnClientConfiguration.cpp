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

