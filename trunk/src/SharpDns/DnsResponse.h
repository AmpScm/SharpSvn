#pragma once
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


namespace SharpDns 
{
	/// <summary>Encapsulates a dns response message</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class DnsResponse sealed : DnsMessage
	{
	public:
		DnsResponse(void);

	internal:
		virtual void EnsureWritable() override
		{
			throw gcnew InvalidOperationException();
		}

	public:
		property bool Succeeded
		{
			bool get()
			{
				return Result == DnsResult::NoError;
			}
		}
	};

}
