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

	/// <summary>Extended Parameter container of <see cref="SvnClient::Delete(String^,SvnDeleteArgs^)" /> and <see cref="SvnClient::RemoteDelete(Uri^,SvnDeleteArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDeleteArgs : public SvnClientArgsWithCommit
	{
		bool _force;
		bool _keepLocal;
	public:
		SvnDeleteArgs()
		{}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Delete;
			}
		}

		/// <summary>If Force is not set then this operation will fail if any path contains locally modified
		/// and/or unversioned items. If Force is set such items will be deleted.</summary>
		property bool Force
		{
			[DebuggerStepThrough]
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}

		property bool KeepLocal
		{
			[DebuggerStepThrough]
			bool get()
			{
				return _keepLocal;
			}
			void set(bool value)
			{
				_keepLocal = value;
			}
		}
	};

}
