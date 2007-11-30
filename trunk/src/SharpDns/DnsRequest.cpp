#include "StdAfx.h"
#include "DnsAll.h"
#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

using namespace SharpDns;

DnsRequest::DnsRequest(void)
{
	_request = gcnew DnsMessage();
}

void DnsRequest::SetRequest(dns_request_t* pRequest)
{
	if(m_pRequest)
		throw gcnew InvalidOperationException();

	m_pRequest = pRequest;
}

void DnsRequest::OnRequestCompleted(isc_event_t *ev)
{
	dns_requestevent_t* e = (dns_requestevent_t *)ev;

	isc_result_t r = e->result;

	if(!r)
	{
		DnsResponse^ msg = gcnew DnsResponse();

		isc_result_t r = dns_request_getresponse(e->request, msg->Handle, 0);

		if(r)
			throw gcnew InvalidOperationException();

		_response = msg;
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

	dns_request_t *pRequest = m_pRequest;
	if(pRequest)
	{		
		dns_request_destroy(&pRequest);
		m_pRequest = pRequest = nullptr;
	}
}

DnsRequestManager^ DnsRequest::Manager::get()
{
	if(!_manager)
		_manager = gcnew DnsRequestManager();

	return _manager;
}

System::Collections::Generic::ICollection<System::Net::IPAddress^>^ DnsRequest::DnsServers::get()
{
	if(_dnsServers)
		return _dnsServers;

	PIP_ADAPTER_ADDRESSES pAdapterAddresses;
	DWORD dwRetVal = 0;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_ADDRESSES);

	pAdapterAddresses = (IP_ADAPTER_ADDRESSES *) malloc( sizeof(IP_ADAPTER_ADDRESSES) );

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersAddresses( AF_UNSPEC, 0, nullptr, pAdapterAddresses, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterAddresses);
		pAdapterAddresses = (IP_ADAPTER_ADDRESSES *) malloc (ulOutBufLen); 
	}

	if(!GetAdaptersAddresses( AF_UNSPEC, 0, nullptr, pAdapterAddresses, &ulOutBufLen))
	{
		System::Collections::Generic::List<IPAddress^>^ servers = gcnew System::Collections::Generic::List<IPAddress^>();

		PIP_ADAPTER_ADDRESSES i = pAdapterAddresses;

		while(i)
		{
			PIP_ADAPTER_DNS_SERVER_ADDRESS dns = i->FirstDnsServerAddress;

			while(dns)
			{
				int len = 0;
				switch(dns->Address.lpSockaddr->sa_family)
				{
				case AF_INET:
					len = 4;
					break;
				case AF_INET6:
					len = 16;
					break;
				default:
					dns = dns->Next;
					continue;
				}
				
				array<Byte>^ data = gcnew array<Byte>(len);
				pin_ptr<Byte> pData = &data[0];

				sockaddr* paddr = (sockaddr*)&dns->Address.lpSockaddr->sa_data;

				memcpy(pData, &paddr->sa_data, len);

				IPAddress^ dnsAddr = gcnew IPAddress(data);

				if(!servers->Contains(dnsAddr))
					servers->Add(dnsAddr);

				dns = dns->Next;
			}

			i = i->Next;
		}

		_dnsServers = servers->AsReadOnly();
	}

	return _dnsServers;
}

IPAddress^ DnsRequest::DefaultDnsServer::get()
{
	for each(IPAddress^ i in DnsServers)
	{
		return i;
	}
	return nullptr;
}
		

void DnsRequest::Send()
{
	Send(DefaultDnsServer);
}

void DnsRequest::Send(System::Net::IPEndPoint ^dnsServer)
{
	if(!dnsServer)
		throw gcnew ArgumentNullException("dnsServer");

	Manager->Send(this, dnsServer);
}

void DnsRequest::Send(System::Net::IPAddress ^dnsServer)
{
	if(!dnsServer)
		throw gcnew ArgumentNullException("dnsServer");

	Send(gcnew System::Net::IPEndPoint(dnsServer, 53)); // DNS servers communicate over port 53 udp/tcp
}
