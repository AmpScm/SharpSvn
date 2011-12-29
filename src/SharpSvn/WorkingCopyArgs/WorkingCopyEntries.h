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

namespace SharpSvn {


	public ref class SvnWorkingCopyEntriesArgs : public SvnClientArgs
	{
		bool _hideSome;
		SvnDepth _depth;

	internal:
		String^ _dir;

	public:
		SvnWorkingCopyEntriesArgs()
		{
			_depth = SvnDepth::Unknown;
		}

	public:
		DECLARE_EVENT(SvnWorkingCopyEntryEventArgs^, Entry)

	protected public:
		virtual void OnEntry(SvnWorkingCopyEntryEventArgs^ e)
		{
			Entry(this, e);
		}

	public:
		/// <summary>Gets or sets a boolean indicating whether to return deleted or absent entries</summary>
		property bool RetrieveHidden
		{
			bool get()
			{
				return !_hideSome;
			}
			void set(bool value)
			{
				_hideSome = !value;
			}
		}

	internal:
		/// <summary>The depth to walk for entries; reeds all entries in path when depth is unknown (default)</summary>
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

	public:
		/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
		virtual property SvnCommandType CommandType
		{
			SvnCommandType get() override
			{
				return SvnCommandType::GetWorkingCopyEntries;
			}
		}

	};

}