// $Id: SvnMergesEligibleEventArgs.h 650 2008-08-07 11:28:07Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	ref class SvnLookClient;

	/// <summary>Base Extended Parameter container of <see cref="SvnLookClient" /> commands</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLookClientArgs abstract : public SvnClientArgs
	{
		__int64 _revision;
		String^ _transaction;
	protected:
		SvnLookClientArgs()
		{
			_revision = -1L;
		}

	public:
		/// <summary>Gets or sets the revision to retrieve information on if this value is negative the latest (HEAD) version is used</summary>
		/// <remarks>If the Transaction property is not-null the <see cref="Revision" /> property is ignored</remarks>
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
			void set(__int64 value)
			{
				_revision = value;
			}
		}

		/// <summary>Gets or sets the transaction to retrieve information on</summary>
		/// <remarks>If the Transaction property is not-null the <see cref="Revision" /> property is ignored</remarks>
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
	};
}
