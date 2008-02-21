// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details
#include "StdAfx.h"
#include "DnsAll.h"

using namespace SharpDns;
using namespace SharpDns::Implementation;
using System::Runtime::InteropServices::Marshal;

DnsResponse::DnsResponse(void) : DnsMessage(DNS_MESSAGE_INTENTPARSE)
{
}
