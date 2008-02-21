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
