// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s Changed method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnChangedArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		String^ _transaction;

	public:
		SvnChangedArgs()
		{
			_revision = SvnRevision::None;
		}

		event EventHandler<SvnChangedEventArgs^>^ Changed;

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property String^ Transaction
		{
			String^ get()
			{
				return _transaction;
			}
			void set(String^ value)
			{
				_transaction = value;
			}
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

	protected public:
		virtual void OnChanged(SvnChangedEventArgs^ e)
		{
			Changed(this, e);
		}
	};
}