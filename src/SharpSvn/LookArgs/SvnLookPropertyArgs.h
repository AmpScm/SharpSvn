// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s Property method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLookPropertyArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		String^ _transaction;
		String^ _result;
		__int64 _resultRevision;

	public:
		SvnLookPropertyArgs()
		{
			_revision = SvnRevision::None;
		}

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

		property String^ Result
		{
			String^ get()
			{
				return _result;
			}
			void set(String^ value)
			{
				_result = value;
			}
		}

		property __int64 ResultRevision
		{
			__int64 get()
			{
				return _resultRevision;
			}
			void set(__int64 value)
			{
				_resultRevision = value;
			}
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}
	};
}
