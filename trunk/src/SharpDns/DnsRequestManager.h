#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


namespace SharpDns 
{
	ref class DnsMessage;
	ref class DnsRequest;

	/// <summary>
	/// DnsRequestManager, manager of in-progress dns requests
	/// </summary>
	public ref class DnsRequestManager :  public System::Runtime::ConstrainedExecution::CriticalFinalizerObject
	{
	private:
		isc_mem_t* m_pMem;
		isc_timermgr_t* m_pTimerMgr;
		isc_socketmgr_t* m_pSocketMgr;
		isc_taskmgr_t* m_pTaskMgr;
		dns_dispatchmgr_t* m_pDispatchMgr;
		dns_dispatch_t* m_pDispIpv4;
		dns_dispatch_t* m_pDispIpv6;
		dns_requestmgr_t* m_pRequestMgr;
		isc_task_t* m_pTask;

		static DnsRequestManager();

	public:
		DnsRequestManager();

	public:
		property System::Net::IPAddress^ DefaultDnsServer
		{
			System::Net::IPAddress^ get()
			{
				return System::Net::IPAddress::Parse("194.109.6.66");
			}
		}	

		void Shutdown();

	private:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DnsRequestManager()
		{
			Close();
		}

		!DnsRequestManager()
		{
			Close();
		}

		void Close();

		dns_dispatch_t* CreateDispatch(int family);

		static void OnTaskCallback(isc_task_t *task, isc_event_t *ev);
		static void SetAddr(System::Net::EndPoint^ from, isc_sockaddr *to);

	public:
		DnsRequest^ Send(DnsMessage^ message);
		DnsRequest^ Send(DnsMessage^ message, System::Net::IPEndPoint^ dnsServer);
		DnsRequest^ Send(DnsMessage^ message, System::Net::IPAddress^ dnsServer);

		
	};
}
