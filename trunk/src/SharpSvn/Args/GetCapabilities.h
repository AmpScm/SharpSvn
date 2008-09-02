// $Id: Blame.h 682 2008-08-16 22:28:39Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {
	
	/// <summary>Extended Parameter container for SvnClient.Blame</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetCapabilitiesArgs : public SvnClientArgs
	{
		initonly Collection<SvnCapability>^ _capabilities;
		bool _retrieveAll;

	public:
		SvnGetCapabilitiesArgs()
		{
			_capabilities = gcnew Collection<SvnCapability>();			
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

	public:
		property bool RetrieveAllCapabilities
		{
			bool get()
			{
				return _retrieveAll;
			}
			void set(bool value)
			{
				_retrieveAll = value;
			}
		}

		property Collection<SvnCapability>^ Capabilities
		{
			Collection<SvnCapability>^ get()
			{
				return _capabilities;
			}
		}
	};

}
