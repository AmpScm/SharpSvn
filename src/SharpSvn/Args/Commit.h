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

	/// <summary>Extended Parameter container of <see cref="SvnClient::Commit(String^, SvnCommitArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCommitArgs : public SvnClientArgsWithCommit
	{
		SvnDepth _depth;
		bool _keepLocks;
		bool _keepChangeLists;
		SvnChangeListCollection^ _changelists;
		bool _commitOperationsRecursively;

	public:
		SvnCommitArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Commit;
			}
		}

	public:
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

		property bool KeepLocks
		{
			bool get()
			{
				return _keepLocks;
			}
			void set(bool value)
			{
				_keepLocks = value;
			}
		}

		/// <summary>Gets the list of changelist-names to commit</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if (!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}

		property bool KeepChangeLists
		{
			bool get()
			{
				return _keepChangeLists;
			}
			void set(bool value)
			{
				_keepChangeLists = value;
			}
		}

		property bool CommitOperationsRecursively
		{
			bool get()
			{
				return _commitOperationsRecursively;
			}
			void set(bool value)
			{
				_commitOperationsRecursively = value;
			}
		}
	};

}
