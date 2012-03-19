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

#include "EventArgs/SvnImportFilterEventArgs.h"

namespace SharpSvn {

/// <summary>Extended Parameter container of <see cref="SvnClient::Import(String^,Uri^,SvnImportArgs^)" /> and <see cref="SvnClient::RemoteImport(String^,Uri^,SvnImportArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnImportArgs : public SvnClientArgsWithCommit
	{
		bool _noIgnore;
		bool _ignoreUnknownNodeTypes;
		SvnDepth _depth;
	public:
		SvnImportArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Import;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
			}
		}

		property bool NoIgnore
		{
			bool get()
			{
				return _noIgnore;
			}
			void set(bool value)
			{
				_noIgnore = value;
			}
		}

		property bool IgnoreUnknownNodeTypes
		{
			bool get()
			{
				return _ignoreUnknownNodeTypes;
			}
			void set(bool value)
			{
				_ignoreUnknownNodeTypes = value;
			}
		}

    public:
        DECLARE_EVENT(SvnImportFilterEventArgs^, Filter)

	internal:
		// Used by SvnImport to forward SvnCheckout errors
		void HandleOnSvnError(Object^ sender, SvnErrorEventArgs^ e)
		{
			UNUSED_ALWAYS(sender);
			OnSvnError(e);
		}

        property bool HasFilter
        {
            bool get()
            {
                return (event_Filter != nullptr) || (GetType() != SvnImportArgs::typeid);
            }
        }

    protected public:
		virtual void OnFilter(SvnImportFilterEventArgs^ e)
		{
			Filter(this, e);
		}
	};

}
