// $Id: WorkingCopyEntries.h 1432 2010-01-14 11:53:38Z rhuijben $
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


	public ref class SvnWorkingCopyInstallConflictArgs : public SvnClientArgs
	{
	internal:
		SvnUriOrigin^ _left;
		SvnUriOrigin^ _right;
		SvnConflictReason _reason;
		SvnConflictAction _action;
		SvnOperation _operation;
		bool _installTreeConflict;

	public:
		SvnWorkingCopyInstallConflictArgs()
		{
		}

		property bool TreeConflict
		{
			bool get()
			{
				return _installTreeConflict;
			}
			void set(bool value)
			{
				_installTreeConflict = value;
			}
		}

		property SvnOperation Operation
		{
			SvnOperation get()
			{
				return _operation;
			}
			void set(SvnOperation value)
			{
				_operation = EnumVerifier::Verify(value);
			}
		}

		property SvnConflictReason Reason
		{
			SvnConflictReason get()
			{
				return _reason;
			}
			void set(SvnConflictReason value)
			{
				_reason = EnumVerifier::Verify(value);
			}
		}

		property SvnConflictAction Action
		{
			SvnConflictAction get()
			{
				return _action;
			}
			void set(SvnConflictAction value)
			{
				_action = EnumVerifier::Verify(value);
			}
		}

		/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
		virtual property SvnCommandType CommandType
		{
			SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}
	};

}