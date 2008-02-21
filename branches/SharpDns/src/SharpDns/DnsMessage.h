// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details
#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


namespace SharpDns 
{
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

	public enum class DnsResult
	{
		NoError = dns_rcode_noerror,
		FormatError = dns_rcode_formerr,
		ServFail = dns_rcode_servfail,
		NxDomain = dns_rcode_nxdomain,
		NotImplemented = dns_rcode_notimp,
		Refused = dns_rcode_refused,
		YxDomain = dns_rcode_yxdomain,
		YxRrSet = dns_rcode_yxrrset,
		NxRrSet = dns_rcode_nxrrset,
		NotAuthorized = dns_rcode_notauth,
		NotZone = dns_rcode_notzone,
		BadVersion = dns_rcode_badvers
	};

	/// <summary>Represents a message to or from a Dns server</summary>
	/// <threadsafety static="true" instance="false"/>
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

	internal:
		void AddQuestion(String^ name, DnsDataClass dataClass, DnsDataType dataType);
		virtual void EnsureWritable()
		{
		}

	private:
		DnsSection^ _question;
		DnsSection^ _answer;
		DnsSection^ _authority;
		DnsSection^ _additional;

	public:
		property DnsSection^ Question
		{
			DnsSection^ get()
			{
				if(!_question)
					_question = gcnew DnsSection(this, DNS_SECTION_QUESTION);

				return _question;
			}
		}

		property DnsSection^ Answer
		{
			DnsSection^ get()
			{
				if(!_answer)
					_answer = gcnew DnsSection(this, DNS_SECTION_ANSWER);

				return _answer;
			}
		}

		property DnsSection^ Autority
		{
			DnsSection^ get()
			{
				if(!_authority)
					_authority = gcnew DnsSection(this, DNS_SECTION_AUTHORITY);

				return _authority;
			}
		}

		property DnsSection^ Additional
		{
			DnsSection^ get()
			{
				if(!_additional)
					_additional = gcnew DnsSection(this, DNS_SECTION_ADDITIONAL);

				return _additional;
			}
		}

		property DnsResult Result
		{
			DnsResult get()
			{
				return (DnsResult)m_pMessage->rcode;
			}
		}

	public:
		virtual String^ ToString() override;

		String^ ToText(DnsMessageFormatOptions options);

	internal:
		String^ SectionToText(dns_section_t section, DnsMessageFormatOptions options);
		array<DnsItem^>^ GetNames(dns_section_t section);

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
				EnsureWritable();
				m_Flags = value;

				if(m_pMessage)
					m_pMessage->flags = (unsigned)m_Flags;
			}
		}

		void Ensure();
	};

}