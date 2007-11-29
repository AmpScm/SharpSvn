#include "StdAfx.h"
#include "DnsAll.h"

using namespace SharpDns;
using namespace SharpDns::Implementation;
using System::Runtime::InteropServices::Marshal;

typedef void (__stdcall pInvokeCompleted_t)(isc_task_t *task, isc_event_t *ev);
static pInvokeCompleted_t *pInvokeCompleted = nullptr;

delegate void mInvokeTask(isc_task_t* task, isc_event_t *ev);


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
/*

		unsigned attrs = 0;
		attrs |= DNS_DISPATCHATTR_UDP;
		switch (isc_sockaddr_pf(addr)) {
		case AF_INET:
			attrs |= DNS_DISPATCHATTR_IPV4;
			break;
		case AF_INET6:
			attrs |= DNS_DISPATCHATTR_IPV6;
			break;
		default:
			result = ISC_R_NOTIMPLEMENTED;
			goto cleanup;
		}
		unsigned attrmask = 0;
		attrmask |= DNS_DISPATCHATTR_UDP;
		attrmask |= DNS_DISPATCHATTR_TCP;
		attrmask |= DNS_DISPATCHATTR_IPV4;
		attrmask |= DNS_DISPATCHATTR_IPV6;

		dns_dispatch_t *pDisp = nullptr;

		if(dns_dispatch_getudp(pDispatchMgr, pSocketMgr, pTaskMgr, nullptr, 
			4096, 1000, 32768, 16411, 16433, // From server.c, probably to big
			attrs, attrmask, &pDisp))
		{
			throw gcnew InvalidOperationException();
		}
		*/

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

	isc_taskmgr_t* pTaskMgr = m_pTaskMgr;
	if(pTaskMgr)
	{
		isc_taskmgr_destroy(&pTaskMgr);
		m_pTaskMgr = pTaskMgr = nullptr;
	}

	

	isc_timermgr_t* pTimerMgr = m_pTimerMgr;
	if(pTimerMgr)
	{
		isc_timermgr_destroy(&pTimerMgr);
		m_pTimerMgr = pTimerMgr;
	}
}

DnsRequest^ DnsRequestManager::Send(DnsMessage^ message)
{
	if(!message)
		throw gcnew ArgumentNullException("message");

	return Send(message, DefaultDnsServer);	
}

static void OnRequestCompleted(isc_task_t *task, isc_event_t *ev)
{
	pInvokeCompleted(task, ev);
}

void DnsRequestManager::OnTaskCallback(isc_task_t *task, isc_event_t *ev)
{
	DnsRequest^ rq = DnsRequest::FromArg(ev->ev_arg);

	rq->OnRequestCompleted(task, ev);
}

void DnsRequestManager::SetAddr(System::Net::EndPoint^ from, isc_sockaddr *to)
{
	if(!from || !to)
		throw gcnew ArgumentNullException();

	System::Net::SocketAddress^ saddr = from->Serialize();
	isc_sockaddr addr;

	if(saddr->Size > sizeof(addr))
		throw gcnew InvalidOperationException();

	char* pTo = (char*)to;

	for(int i = 0; i < saddr->Size; i++)
	{
		pTo[i] = saddr[i];
	}
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

	//SetAddr(gcnew System::Net::IPEndPoint(System::Net::IPAddress::Parse("172.20.3.11"), 33003), &localAddr);

	//localAddr.type.sin.sin_port = System::Net::IPAddress::HostToNetworkOrder(33033);

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

DnsRequest^ DnsRequestManager::Send(DnsMessage^ message, System::Net::IPEndPoint ^dnsServer)
{
	if(!message)
		throw gcnew ArgumentNullException("message");
	else if(!dnsServer)
		throw gcnew ArgumentNullException("dnsServer");	

	//System::Net::SocketAddress^ saddr = dnsServer->Serialize();
	isc_sockaddr addr;
	SetAddr(dnsServer, &addr);


	dns_tsigkey_t *key = nullptr;

	DnsRequest^ rq = gcnew DnsRequest();
	dns_request_t* pRequest = nullptr;	

	int opts = DNS_REQUESTOPT_TCP; // !DNS_REQUESTOPT_TCP

	isc_result_t err = dns_request_createvia3(m_pRequestMgr, message->Handle, 
		nullptr, &addr, 
		opts, key, 15, 10, 3,
		m_pTask, OnRequestCompleted, rq->AllocArg(), &pRequest);
	if(!err)
	{
		rq->SetRequest(pRequest);

		return rq;
	}
	else
	{
		rq->Clear();
		GC::KeepAlive(err);
		throw gcnew InvalidOperationException();
	}
}

DnsRequest^ DnsRequestManager::Send(DnsMessage^ message, System::Net::IPAddress ^dnsServer)
{
	if(!message)
		throw gcnew ArgumentNullException("message");
	else if(!dnsServer)
		throw gcnew ArgumentNullException("dnsServer");

	return Send(message, gcnew System::Net::IPEndPoint(dnsServer, 53)); // DNS servers communicate over port 53 udp/tcp
}