#include "StdAfx.h"
#include "DnsAll.h"

using namespace SharpDns;

DnsRequest::DnsRequest(void)
{
}

void DnsRequest::SetRequest(dns_request_t* pRequest)
{
	if(m_pRequest)
		throw gcnew InvalidOperationException();

	m_pRequest = pRequest;
}

void DnsRequest::OnRequestCompleted(isc_task_t *task, isc_event_t *ev)
{
	dns_requestevent_t* e = (dns_requestevent_t *)ev;

	isc_result_t r = e->result;

	if(!r)
	{
		DnsMessage^ msg = gcnew DnsMessage(DNS_MESSAGE_INTENTPARSE);

		isc_result_t r = dns_request_getresponse(e->request, msg->Handle, 0);

		if(r)
		{
			throw gcnew InvalidOperationException();
		}
	}
	else
	{
		GC::KeepAlive(r);
	}
}

void DnsRequest::Clear()
{
	// TODO: Clear args
	if(pTag)
	{
		delete pTag;
		pTag = nullptr;
	}
}