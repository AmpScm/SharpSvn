// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details
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

		return gcnew String((char*)isc_buffer_base(pBuffer), 0, isc_buffer_usedlength(pBuffer));
	}
	finally
	{
		isc_buffer_free(&pBuffer);
	}

	return nullptr;
}

String^ DnsMessage::SectionToText(dns_section_t section, DnsMessageFormatOptions options)
{
	Ensure();	

	isc_buffer_t *pBuffer = nullptr;

	if(!isc_buffer_allocate(SharpDnsBase::MemoryManager, &pBuffer, 1024))
	try
	{
		dns_message_sectiontotext(m_pMessage, section, &dns_master_style_debug, (dns_messagetextflag_t)options, pBuffer);

		return gcnew String((char*)isc_buffer_base(pBuffer), 0, isc_buffer_usedlength(pBuffer));
	}
	finally
	{
		isc_buffer_free(&pBuffer);
	}

	return nullptr;
}

String^ DnsSection::ToString()
{
	return ToText(DnsMessageFormatOptions::NoComments | DnsMessageFormatOptions::NoHeaders);
}

String^ DnsSection::ToText(DnsMessageFormatOptions options)
{
	return _message->SectionToText(_section, options);
}

void DnsSection::Refresh()
{
	_list = _message->GetNames(_section);

	if(!_list)
		_list = gcnew array<DnsItem^>(0);
}

array<DnsItem^>^ DnsMessage::GetNames(dns_section_t section)
{
	if(dns_message_firstname(m_pMessage, section))
		return nullptr;

	System::Collections::Generic::List<DnsItem^>^ list = nullptr;
	isc_buffer_t *pBuffer = nullptr;
	isc_buffer_t *pBuf2 = nullptr;

	try
	{
		if(!isc_buffer_allocate(SharpDnsBase::MemoryManager, &pBuffer, 1024) && !isc_buffer_allocate(SharpDnsBase::MemoryManager, &pBuf2, 1024))
		do
		{
			dns_name_t *pName = nullptr;
			dns_message_currentname(m_pMessage, section, &pName);

			if(pName)
			{
				if(!list)
					list = gcnew System::Collections::Generic::List<DnsItem^>();

				isc_buffer_clear(pBuffer);
				dns_name_totext(pName, isc_boolean_false, pBuffer);

				String^ name = gcnew String((char*)isc_buffer_base(pBuffer), 0, isc_buffer_usedlength(pBuffer));		

				dns_rdataset_t * rdataset = ISC_LIST_HEAD(pName->list);

				if(!dns_rdataset_first(rdataset))
				{
					do
					{
						dns_rdata_t data;
						memset(&data, 0, sizeof(data));

						dns_rdataset_current(rdataset, &data);

						isc_buffer_clear(pBuf2);
						if(!dns_rdata_totext(&data, pName, pBuf2))
						{
							String^ value = gcnew String((char*)isc_buffer_base(pBuf2), 0, isc_buffer_usedlength(pBuf2));
							list->Add(gcnew DnsItem(name, (DnsDataClass)data.rdclass, (DnsDataType)data.type, rdataset->ttl, value));
						}					
					}
					while(!dns_rdataset_next(rdataset));
				}
			}
		}
		while(!dns_message_nextname(m_pMessage, section));
	}
	finally
	{
		if(pBuffer)
			isc_buffer_free(&pBuffer);
		if(pBuf2)
			isc_buffer_free(&pBuf2);
	}

	if(list)
		return list->ToArray();
	else
		return nullptr;
}