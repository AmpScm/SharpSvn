#include "StdAfx.h"
#include "DnsAll.h"

using namespace SharpDns;

DnsMessage::DnsMessage(void)
{
	m_uIntent =  DNS_MESSAGE_INTENTRENDER;
	m_Flags = DnsMessageFlags::Recursive;
}

DnsMessage::DnsMessage(unsigned intent)
{
	 m_uIntent = intent;
	 m_Flags = DnsMessageFlags::Recursive;
}

dns_messageid_t dns_id = 0x1234;

void DnsMessage::Ensure()
{
	if(m_pMessage)
		return;

	dns_message_t* pMessage = nullptr;
	if(!dns_message_create(SharpDnsBase::MemoryManager, m_uIntent, &pMessage))
		m_pMessage = pMessage;

	if(pMessage)
	{
		pMessage->flags = (unsigned)m_Flags;

		if(m_uIntent == DNS_MESSAGE_INTENTRENDER)
		{
			pMessage->opcode = dns_opcode_query;
		}
	}
}

void DnsMessage::AddQuestion(String^ name, DnsDataClass dataClass, DnsDataType dataType)
{
	if(!name)
		throw gcnew ArgumentNullException();

	isc_result_t result = 0;

	Ensure();

	dns_name_t* pName = nullptr;
	dns_message_gettempname(m_pMessage, &pName);
	dns_name_init(pName, nullptr);

	array<Byte> ^bytes = System::Text::Encoding::UTF8->GetBytes(name);
	pin_ptr<Byte> pFirst = &bytes[0];

	isc_buffer_t* pBuf = nullptr;
	isc_buffer_t* pBuf2 = nullptr;
	if(isc_buffer_allocate(SharpDnsBase::MemoryManager, &pBuf, bytes->Length+1))
		throw gcnew InvalidOperationException();

	if(isc_buffer_allocate(SharpDnsBase::MemoryManager, &pBuf2, bytes->Length+1))
		throw gcnew InvalidOperationException();

	char* pTo = (char*)pBuf->base;
	memcpy(pTo, (const void*)pFirst, bytes->Length);

	pTo[bytes->Length] = 0;

	isc_buffer_add(pBuf, bytes->Length);

	result = dns_name_fromtext(pName, pBuf, nullptr, 0, pBuf2);

	if(result)
	{
		throw gcnew ArgumentException("name");
	}	

	dns_rdataset_t *rdataset = nullptr;

	rdataset = nullptr;
	result = dns_message_gettemprdataset(m_pMessage, &rdataset);
	
	if(result)
		throw gcnew InvalidOperationException();

	dns_rdataset_init(rdataset);
	dns_rdataset_makequestion(rdataset, (dns_rdataclass_t)dataClass, (dns_rdatatype_t)dataType);

	ISC_LIST_APPEND(pName->list, rdataset, link);

	dns_message_addname(m_pMessage, pName, DNS_SECTION_QUESTION);
}

String^ DnsMessage::ToString()
{
	return ToText(DnsMessageFormatOptions::None);
}

String^ DnsMessage::ToText(DnsMessageFormatOptions options)
{
	Ensure();

	isc_buffer_t *pBuffer = nullptr;

	if(!isc_buffer_allocate(SharpDnsBase::MemoryManager, &pBuffer, 1024))
	try
	{
		dns_message_totext(m_pMessage, &dns_master_style_debug, (dns_messagetextflag_t)options, pBuffer);

		return gcnew String((char*)isc_buffer_base(pBuffer), 0, isc_buffer_length(pBuffer));
	}
	finally
	{
		isc_buffer_free(&pBuffer);
	}

	return nullptr;
}