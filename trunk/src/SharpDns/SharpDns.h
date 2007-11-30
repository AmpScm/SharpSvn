// SharpSvn.Dns.h

#pragma once

using namespace System;

namespace SharpDns 
{
	namespace Implementation
	{

		public ref class SharpDnsBase : public MarshalByRefObject
		{
			static initonly bool m_bHaveIpv4;
			static initonly bool m_bHaveIpv6;
			static isc_mem_t* g_pMem;
			static isc_entropy_t* g_pEnt;
			static dns_view_t* g_pView;

			static SharpDnsBase();

		internal:
			static void InitializeResolver();

			static property isc_mem_t* MemoryManager
			{
				isc_mem_t* get()
				{
					if(!g_pMem)
						throw gcnew ObjectDisposedException("SharpDnsBase");

					return g_pMem;
				}
			}

			static property isc_entropy_t* EntropyManager
			{
				isc_entropy_t* get()
				{
					if(!g_pEnt)
						throw gcnew ObjectDisposedException("SharpDnsBase");

					return g_pEnt;
				}
			}

			static property bool HaveIpv4
			{
				bool get()
				{
					return m_bHaveIpv4;
				}
			}

			static property bool HaveIpv6
			{
				bool get()
				{
					return m_bHaveIpv6;
				}
			}
		};
	}


	public ref class SharpSvnDns : public Implementation::SharpDnsBase
	{
		// TODO: Add your methods for this class here.
	};
}