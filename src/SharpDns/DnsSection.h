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
	ref class DnsSection;

	/// <summary>Formatting options of <see cref="DnsMessage"/> and <see cref="DnsSection"/> instances</summary>
	[Flags]
	public enum class DnsMessageFormatOptions
	{
		None = 0,
		NoComments = DNS_MESSAGETEXTFLAG_NOCOMMENTS,
		NoHeaders = DNS_MESSAGETEXTFLAG_NOHEADERS
	};

	/// <summary>Section within a <see cref="DnsMessage" /></summary>
	public ref class DnsSection sealed : public System::Collections::Generic::ICollection<DnsItem^>, System::Collections::Generic::IEnumerable<DnsItem^>
	{
		initonly DnsMessage^ _message;
		initonly dns_section_t _section;
		array<DnsItem^>^ _list;
	internal:
		DnsSection(DnsMessage^ message, dns_section_t section)
		{
			_message = message;
			_section = section;
		}

	public:
		virtual String^ ToString() override;
		String^ ToText(DnsMessageFormatOptions options);

	public:
		property DnsItem^ default[int]
		{
			DnsItem^ get(int index)
			{
				if(!_list)
					Refresh();

				return _list[index];
			}
		}

		virtual IEnumerator^ UGGetEnumerator() = IEnumerable::GetEnumerator
		{
			return GetEnumerator();
		}

		virtual System::Collections::Generic::IEnumerator<DnsItem^>^ GetEnumerator()
		{
			if(!_list)
				Refresh();

			return dynamic_cast<System::Collections::Generic::IEnumerable<DnsItem^>^>(_list)->GetEnumerator();
		}

		virtual property int Count 
		{
			int get()
			{
				if(!_list)
					Refresh();

				return _list ? _list->Length : 0;
			}
		}

		virtual property bool IsReadOnly
		{
			bool get()
			{
				return true;
			}
		}

	protected:
		virtual void Add(DnsItem^ name) = System::Collections::Generic::ICollection<DnsItem^>::Add
		{
			UNUSED_ALWAYS(name);
			throw gcnew InvalidOperationException();
		}

		virtual bool Remove(DnsItem^ name) = System::Collections::Generic::ICollection<DnsItem^>::Remove
		{
			UNUSED_ALWAYS(name);
			throw gcnew InvalidOperationException();
		}

	public:
		virtual bool Contains(DnsItem^ item)
		{
			if(!_list)
				Refresh();

			if(_list)
				return dynamic_cast<System::Collections::Generic::ICollection<DnsItem^>^>(_list)->Contains(item);
			else
				return false;
		}

	protected:
		virtual void Clear() = System::Collections::Generic::ICollection<DnsItem^>::Clear
		{
			throw gcnew InvalidOperationException();
		}

	public:
		virtual void CopyTo(array<DnsItem^>^ list, int offset) = System::Collections::Generic::ICollection<DnsItem^>::CopyTo
		{
			if(!_list)
				Refresh();

			if(_list)
				_list->CopyTo(list, offset);			
		}

	internal:
		void Refresh();
	};
};

