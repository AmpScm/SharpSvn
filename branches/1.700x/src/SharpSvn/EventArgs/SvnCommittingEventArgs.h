// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
		initonly SvnCommandType _commandType;

	internal:
		SvnCommittingEventArgs(const apr_array_header_t *commitItems, SvnCommandType commandType, AprPool^ pool)
		{
			if (!commitItems)
				throw gcnew ArgumentNullException("commitItems");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_commitItems = commitItems;
			_pool = pool;
			_commandType = commandType;
		}

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

		property SvnCommandType CurrentCommandType
		{
			SvnCommandType get()
			{
				return _commandType;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					if (Items)
						for each (SvnCommitItem^ item in Items)
						{
							item->Detach(true);
						}
				}
			}
			finally
			{
				_commitItems = nullptr;
				_pool = nullptr;
			}
		}
	};

}
