#include "StdAfx.h"
#include "DnsAll.h"

using namespace SharpDns;
using namespace SharpDns::Implementation;
using System::Runtime::InteropServices::Marshal;

DnsResponse::DnsResponse(void) : DnsMessage(DNS_MESSAGE_INTENTPARSE)
{
}
