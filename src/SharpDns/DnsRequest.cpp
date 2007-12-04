#include "StdAfx.h"
#include "DnsAll.h"
#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

using namespace SharpDns;
using namespace System::Threading;
using System::Text::StringBuilder;

DnsRequest::DnsRequest(void)
{
	_request = gcnew DnsMessage();
	_timeout = 15;
	_lock = gcnew Object();
}

void DnsRequest::Clear()
{
	// TODO: Clear args
	if(pTag)
	{
		delete pTag;
		pTag = nullptr;
	}

	Monitor::Enter(_lock);
	try
	{
		dns_request_t *pRequest = m_pRequest;
		if(pRequest)
		{		
			dns_request_destroy(&pRequest);
			m_pRequest = pRequest = nullptr;
		}

		if(_sync)
		{
			_sync->Set();
			System::Threading::Thread::Sleep(10);

			delete _sync;
			_sync = nullptr;
		}
	}
	finally
	{
		Monitor::Exit(_lock);
	}
}


void DnsRequest::SetRequest(dns_request_t* pRequest)
{
	if(m_pRequest)
		throw gcnew InvalidOperationException();

	m_pRequest = pRequest;
}

void DnsRequest::OnRequestCompleted(isc_event_t *ev)
{
	try
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

			Completed(this, EventArgs::Empty);
		}
		else
		{
			GC::KeepAlive(r);
		}
	}
	finally
	{
		Monitor::Enter(_lock);
		try
		{
			if(_sync)
				_sync->Set();

			_completed = true;
		}
		finally
		{
			Monitor::Exit(_lock);
		}
	}
}

void DnsRequest::WaitForComplete()
{
	Monitor::Enter(_lock);
	try
	{
		if(_completed)
			return;

		_sync = gcnew EventWaitHandle(false, System::Threading::EventResetMode::ManualReset);
	}
	finally
	{
		Monitor::Exit(_lock);
	}

	if(!_sync->WaitOne(Timeout + TimeSpan(0,0, 3), false))
		throw gcnew InvalidOperationException("Timeout exceeded within .Net");
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

void DnsRequest::AddQuestion(String^ name, DnsDataClass dataClass, DnsDataType dataType)
{
	if(!name)
		throw gcnew ArgumentNullException("name");

	if(!name->EndsWith("."))
		name += ".";

	Request->AddQuestion(name, dataClass, dataType);
}

void DnsRequest::AddQuestion(IPAddress^ address)
{
	if(!address)
		throw gcnew ArgumentNullException("address");

	array<Byte>^ bytes = address->GetAddressBytes();

	if(bytes->Length == 4)
	{
		AddQuestion(String::Format("{0}.{1}.{2}.{3}.in-addr.arpa", bytes[3], bytes[2], bytes[1], bytes[0]), DnsDataType::Ptr);
	}
	else
	{
		StringBuilder^ sb = gcnew StringBuilder();

		for(int i = bytes->Length-1; i >= 0; i--)
		{
			sb->AppendFormat("{0:x}.", bytes[i] & 0xF);
			sb->AppendFormat("{0:x}.", bytes[i] >> 4);
		}
		sb->Append("ip6.arpa.");

		AddQuestion(sb->ToString(), DnsDataType::Ptr);
	}
}
