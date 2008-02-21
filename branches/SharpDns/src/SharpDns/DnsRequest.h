// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details
#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Net;
using System::Collections::Generic::ICollection;
using System::Threading::EventWaitHandle;

namespace SharpDns 
{
	ref class DnsResponse;

	/// <summary>Represents a request to a dns server</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class DnsRequest sealed : public Implementation::SharpDnsBase
	{
		dns_request_t* m_pRequest;
		gcroot<DnsRequest^>* pTag;
		DnsMessage^ _request;
		DnsResponse^ _response;
		int _timeout;
		bool _completed;
		bool _sent;
		initonly Object^ _lock;
		EventWaitHandle^ _sync;

	public:
		event EventHandler^ Completed;

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
		void AddQuestion(String^ name, DnsDataClass dataClass, DnsDataType dataType);
		void AddQuestion(String^ name, DnsDataType dataType)
		{
			AddQuestion(name, DnsDataClass::In, dataType);
		}

		void AddQuestion(String^ name)
		{
			AddQuestion(name, DnsDataClass::In, DnsDataType::Any);
		}

		/// <summary>Perform a PTR query for the specified address</summary>
		void AddQuestion(IPAddress^ address);

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

		property TimeSpan Timeout
		{
			TimeSpan get()
			{
				return TimeSpan(0, 0, 0, _timeout);
			}
			void set(TimeSpan value)
			{
				_timeout = (int)value.TotalSeconds;
				if(_timeout < 3)
					_timeout = 3;
			}
		}
		DnsResponse^ GetResponse()
		{
			WaitForComplete();

			return Response;
		}

	private:
		static DnsRequestManager^ _manager;
		static System::Collections::Generic::ICollection<System::Net::IPAddress^>^ _dnsServers;

		void WaitForComplete();

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