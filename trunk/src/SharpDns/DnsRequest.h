#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


namespace SharpDns 
{
	public ref class DnsRequest : public Implementation::SharpDnsBase
	{
		dns_request_t* m_pRequest;
		gcroot<DnsRequest^>* pTag;

	public:
		DnsRequest(void);

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

		void OnRequestCompleted(isc_task_t *task, isc_event_t *ev);

	internal:
		void SetRequest(dns_request_t* pRequest);
		void Clear();
	};
}