#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Net;
using System::Collections::Generic::ICollection;

namespace SharpDns 
{
	ref class DnsResponse;

	public ref class DnsRequest sealed : public Implementation::SharpDnsBase
	{
		dns_request_t* m_pRequest;
		gcroot<DnsRequest^>* pTag;
		DnsMessage^ _request;
		DnsResponse^ _response;

	private:
		DnsRequest(void);

	private:
		~DnsRequest() { Clear(); }
		!DnsRequest() { Clear(); }

	internal:
		void* AllocArg()
		{
			if(!pTag)
				pTag = new gcroot<DnsRequest^>(this);

			return pTag;
		}

		static DnsRequest^ FromArg(void* arg)
		{
			gcroot<DnsRequest^>* r = (gcroot<DnsRequest^>*)arg;

			return (*r);
		}

		void OnRequestCompleted(isc_event_t *ev);

	public:
		property DnsMessage^ Request
		{
			DnsMessage^ get()
			{
				return _request;
			}
		}

		property DnsResponse^ Response
		{
			DnsResponse^ get()
			{
				return _response;
			}
		}

	private:
		static DnsRequestManager^ _manager;
		static System::Collections::Generic::ICollection<System::Net::IPAddress^>^ _dnsServers;

	internal:
		property DnsRequestManager^ Manager
		{
			DnsRequestManager^ get();
		}

	public:
		static DnsRequest^ Create()
		{
			return gcnew DnsRequest();
		}

		static property System::Collections::Generic::ICollection<System::Net::IPAddress^>^ DnsServers
		{
			System::Collections::Generic::ICollection<System::Net::IPAddress^>^ get();
		}

		static property IPAddress^ DefaultDnsServer
		{
			IPAddress^ get();
		}

	public:
		void Send();
		void Send(IPAddress^ dnsServer);
		void Send(IPEndPoint^ dnsServer);

	internal:
		void SetRequest(dns_request_t* pRequest);
		void Clear();
	};
}