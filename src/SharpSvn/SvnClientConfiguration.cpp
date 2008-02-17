// $Id: SvnClient.cpp 272 2008-02-14 17:46:34Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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