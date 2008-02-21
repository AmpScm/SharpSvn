// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details
#include "StdAfx.h"
#include "DnsAll.h"

using namespace SharpDns;
using namespace SharpDns::Implementation;
using System::Runtime::InteropServices::Marshal;

typedef void (__stdcall pInvokeCompleted_t)(isc_event_t *ev);
static pInvokeCompleted_t *pInvokeCompleted = nullptr;

delegate void mInvokeTask(isc_event_t *ev);


static DnsRequestManager::DnsRequestManager()
{
	SharpDns::Implementation::SharpDnsBase::InitializeResolver();

	pInvokeCompleted = (pInvokeCompleted_t*)Marshal::GetFunctionPointerForDelegate(gcnew mInvokeTask(OnTaskCallback)).ToPointer();
}

DnsRequestManager::DnsRequestManager()
{
	isc_timermgr_t* pTimerMgr = nullptr;
	isc_socketmgr_t* pSocketMgr = nullptr;
	isc_taskmgr_t* pTaskMgr = nullptr;
	dns_dispatchmgr_t* pDispatchMgr = nullptr;
	isc_task_t* pTask = nullptr;
	dns_requestmgr_t* pRequestMgr = nullptr;

	bool ok = false;
	try
	{
		if(isc_timermgr_create(SharpDnsBase::MemoryManager, &pTimerMgr))
			return;

		m_pTimerMgr = pTimerMgr;

		if(isc_socketmgr_create(SharpDnsBase::MemoryManager, &pSocketMgr))
			return;

		m_pSocketMgr = pSocketMgr;

		if(isc_taskmgr_create(SharpDnsBase::MemoryManager, 1, 0, &pTaskMgr))
			return;

		m_pTaskMgr = pTaskMgr;

		if(dns_dispatchmgr_create(SharpDnsBase::MemoryManager, SharpDnsBase::EntropyManager, &pDispatchMgr))
			return;

		m_pDispatchMgr = pDispatchMgr;

		if(!pTaskMgr || isc_task_create(pTaskMgr, 0, &pTask))
			return;

		dns_dispatch_t* pDispIpv4 = SharpDnsBase::HaveIpv4 ? CreateDispatch(AF_INET) : nullptr;
		dns_dispatch_t* pDispIpv6= SharpDnsBase::HaveIpv6 ? CreateDispatch(AF_INET6) : nullptr;

		m_pTask = pTask;

		if(dns_requestmgr_create(SharpDnsBase::MemoryManager, pTimerMgr, pSocketMgr, pTaskMgr, pDispatchMgr, pDispIpv4, pDispIpv6, &pRequestMgr))
			return;

		m_pRequestMgr = pRequestMgr;

		ok = true;
	}
	finally
	{
		if(!ok || !pTask || !pTaskMgr || !pSocketMgr || !pTimerMgr)
		{
			Close();
			throw gcnew InvalidOperationException("Failed to initialize");
		}
	}

}

void DnsRequestManager::Shutdown()
{
	dns_requestmgr_t* pRequestMgr = m_pRequestMgr;
	if(pRequestMgr)
	{
		dns_requestmgr_shutdown(pRequestMgr);
	}
}

void DnsRequestManager::Close()
{
	Shutdown();

	dns_requestmgr_t* pRequestMgr = m_pRequestMgr;
	if(pRequestMgr)
	{
		dns_requestmgr_detach(&pRequestMgr);
		m_pRequestMgr = pRequestMgr = nullptr;
	}

	isc_task_t* pTask = m_pTask;
	if(pTask)
	{
		isc_task_destroy(&pTask);
		m_pTask = pTask = nullptr;
	}
	
	dns_dispatchmgr_t* pDispatchMgr = m_pDispatchMgr;
	if(pDispatchMgr)
	{
		dns_dispatchmgr_destroy(&pDispatchMgr);
		m_pDispatchMgr = pDispatchMgr = nullptr;
	}

	isc_timermgr_t* pTimerMgr = m_pTimerMgr;
	if(pTimerMgr)
	{
		isc_timermgr_destroy(&pTimerMgr);
		m_pTimerMgr = pTimerMgr;
	}

	isc_taskmgr_t* pTaskMgr = m_pTaskMgr;
	if(pTaskMgr)
	{
		//isc_taskmgr_destroy(&pTaskMgr);
		m_pTaskMgr = pTaskMgr = nullptr;
	}
}

static void OnRequestCompleted(isc_task_t *task, isc_event_t *ev)
{
	UNUSED_ALWAYS(task);
	pInvokeCompleted(ev);
}

void DnsRequestManager::OnTaskCallback(isc_event_t *ev)
{
	DnsRequest^ rq = DnsRequest::FromArg(ev->ev_arg);

	rq->OnRequestCompleted(ev);
}

void DnsRequestManager::SetAddr(System::Net::EndPoint^ from, isc_sockaddr *to)
{
	if(!from || !to)
		throw gcnew ArgumentNullException();

	System::Net::SocketAddress^ saddr = from->Serialize();

	memset(to, 0, sizeof(isc_sockaddr));

	if(saddr->Size > sizeof(to->type))
		throw gcnew InvalidOperationException();

	char* pTo = (char*)&to->type;

	for(int i = 0; i < saddr->Size; i++)
	{
		pTo[i] = saddr[i];
	}
	to->length = saddr->Size;
}
dns_dispatch_t* DnsRequestManager::CreateDispatch(int family)
{
	isc_sockaddr localAddr;

	unsigned attrs = DNS_DISPATCHATTR_UDP | DNS_DISPATCHATTR_MAKEQUERY;
	unsigned attrmask = DNS_DISPATCHATTR_IPV4 | DNS_DISPATCHATTR_IPV6 | DNS_DISPATCHATTR_TCP | DNS_DISPATCHATTR_UDP | DNS_DISPATCHATTR_MAKEQUERY;

	if(family == AF_INET)
	{
		isc_sockaddr_any(&localAddr);
		attrs |= DNS_DISPATCHATTR_IPV4;
	}
	else if(family == AF_INET6)
	{
		isc_sockaddr_any6(&localAddr);
		attrs |= DNS_DISPATCHATTR_IPV6;
	}
	else
		throw gcnew ArgumentException();

	dns_dispatch_t* pDispatch = nullptr;

	isc_result_t err = dns_dispatch_getudp(m_pDispatchMgr, m_pSocketMgr, m_pTaskMgr, &localAddr,
		4096, 1000, 32768, 16411, 16433, // From server.c, probably to big		
		attrs, attrmask, &pDispatch);

	if(!err)
		return pDispatch;
	else
	{
		GC::KeepAlive(err);
		throw gcnew InvalidOperationException();
	}
}

DnsRequest^ DnsRequestManager::Send(DnsRequest^ request, System::Net::IPEndPoint ^dnsServer)
{
	if(!request)
		throw gcnew ArgumentNullException("request");
	else if(!dnsServer)
		throw gcnew ArgumentNullException("dnsServer");	

	DnsMessage^ message = request->Request;

	//System::Net::SocketAddress^ saddr = dnsServer->Serialize();
	isc_sockaddr addr;
	SetAddr(dnsServer, &addr);


	dns_tsigkey_t *key = nullptr;
	dns_request_t* pRequest = nullptr;	

	int opts = 0;//DNS_REQUESTOPT_TCP; // !DNS_REQUESTOPT_TCP

	isc_result_t err = dns_request_createvia3(m_pRequestMgr, message->Handle, 
		nullptr, &addr, 
		opts, key, 15, 10, 3,
		m_pTask, OnRequestCompleted, request->AllocArg(), &pRequest);
	if(!err)
	{
		request->SetRequest(pRequest);

		return request;
	}
	else
	{
		request->Clear();
		GC::KeepAlive(err);
		throw gcnew InvalidOperationException();
	}
}