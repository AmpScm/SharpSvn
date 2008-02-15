// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientEventArgs.h

namespace SharpSvn {

	public ref class SvnCommittingEventArgs sealed : public SvnEventArgs
	{
		AprPool^ _pool;
		const apr_array_header_t *_commitItems;
		bool _cancel;
		String^ _logMessage;
		SvnCommitItemCollection^ _items;
	internal:
		SvnCommittingEventArgs(const apr_array_header_t *commitItems, AprPool^ pool);

	public:
		property bool Cancel
		{
			bool get()
			{
				return _cancel;
			}
			void set(bool value)
			{
				_cancel = value;
			}
		}

		property String^ LogMessage
		{
			String^ get()
			{
				return _logMessage;
			}
			void set(String^ value)
			{
				_logMessage = value;
			}
		}

		property SvnCommitItemCollection^ Items
		{
			SvnCommitItemCollection^ get();
		}

		virtual void Detach(bool keepProperties) override;
	};

}