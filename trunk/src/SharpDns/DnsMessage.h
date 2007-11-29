#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


namespace SharpDns 
{
	public enum class DnsDataClass
	{
		In = dns_rdataclass_in,
		Any = dns_rdataclass_any,
	};

	public enum class DnsDataType
	{
		A = dns_rdatatype_a,
		CName = dns_rdatatype_cname,
		AAAA = dns_rdatatype_aaaa,
		Any = dns_rdatatype_any
	};

	[Flags]
	public enum class DnsMessageFlags
	{
		Qr = DNS_MESSAGEFLAG_QR,
		Aa = DNS_MESSAGEFLAG_AA,
		Tc = DNS_MESSAGEFLAG_TC,
		Rd = DNS_MESSAGEFLAG_RD,
		Ra = DNS_MESSAGEFLAG_RA,
		Ad = DNS_MESSAGEFLAG_AD,
		Cd = DNS_MESSAGEFLAG_CD,


		Recursive = Rd
	};

	public ref class DnsMessage : public Implementation::SharpDnsBase
	{
		dns_message_t* m_pMessage;
		isc_buffer_t* m_pNameBuffer;
		DnsMessageFlags m_Flags;
		unsigned m_uIntent;
	public:
		DnsMessage(void);

	internal:
		DnsMessage(unsigned intent);


	public:
		DnsRequest^ Send(DnsRequestManager^ requestManager);
		DnsRequest^ Send(DnsRequestManager^ requestManager, System::Net::IPEndPoint ^dnsServer);
		DnsRequest^ Send(DnsRequestManager^ requestManager, System::Net::IPAddress ^dnsServer);

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


	internal:
		property dns_message_t* Handle
		{
			dns_message_t* get()
			{
				Ensure();

				return m_pMessage;
			}
		}

		property DnsMessageFlags Flags
		{
			DnsMessageFlags get()
			{
				return m_Flags;
			}
			void set(DnsMessageFlags value)
			{
				m_Flags = value;

				if(m_pMessage)
					m_pMessage->flags = (unsigned)m_Flags;
			}
		}


		void Ensure();
	};

}