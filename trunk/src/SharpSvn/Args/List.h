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

	/// <summary>Extended Parameter container for SvnClient.List</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnListArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		SvnDirEntryItems _entryItems;
		bool _fetchLocks;
		bool _includeExternals;

	public:
		DECLARE_EVENT(SvnListEventArgs^, List)

	protected public:
		virtual void OnList(SvnListEventArgs^ e)
		{
			List(this, e);
		}

	public:
		SvnListArgs()
		{
			_depth = SvnDepth::Children;
			_revision = SvnRevision::None;
			_entryItems = SvnDirEntryItems::SvnListDefault;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::List;
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

		property SvnDirEntryItems RetrieveEntries
		{
			SvnDirEntryItems get()
			{
				return _entryItems;
			}

			void set(SvnDirEntryItems value)
			{
				_entryItems = value;
			}
		}

		property bool RetrieveLocks
		{
			bool get()
			{
				return _fetchLocks;
			}
			void set(bool value)
			{
				_fetchLocks = value;
			}
		}

		property bool IncludeExternals
		{
			bool get()
			{
				return _includeExternals;
			}
			void set(bool value)
			{
				_includeExternals = value;
			}
		}

	internal:
		String^ _queryRoot;
		Uri^ _repositoryRoot;
		void Prepare(SvnTarget^ target, bool hasRevision)
		{
			_repositoryRoot = nullptr;

			SvnUriTarget^ uriTarget = dynamic_cast<SvnUriTarget^>(target);
			if (uriTarget && !hasRevision)
				_queryRoot = SvnBase::UriToCanonicalString(uriTarget->Uri);
			else
				_queryRoot = nullptr;
		}

		Uri^ CalculateRepositoryRoot(const char* abs_path)
		{
			if (_repositoryRoot || !_queryRoot)
				return _repositoryRoot;

			String^ qr = _queryRoot;
			_queryRoot = nullptr; // Only parse in the first call, which matches the exact request

			String^ absPath = SvnBase::Utf8_PtrToString(abs_path+1);
			String^ path;

			if (absPath->Length > 0)
				path = SvnBase::PathToUri(SvnBase::Utf8_PtrToString(abs_path+1))->ToString(); // Skip the initial '/'
			else
				path = "";

			Uri^ rt;
			if (path->Length == 0)
			{
				// Special case. The passed path is the solution root; EndsWith would always succeed
				if (qr->Length > 0 && qr[qr->Length-1] != '/')
					_repositoryRoot = gcnew Uri(qr + "/");
				else
					_repositoryRoot = gcnew Uri(qr);
			}
			else if (Uri::TryCreate(qr, UriKind::Absolute, rt))
			{
				int n = path->Length;

				int up = 0;
				while (--n > 0)
				{
					if (path[n] == '/')
						up++;
				}

				if (qr[qr->Length-1] == '/')
					up++;
				else if (up == 0)
					rt = gcnew Uri(rt, "./");

				while (up-- > 0)
					rt = gcnew Uri(rt, "../");

				_repositoryRoot = rt;
			}

			return _repositoryRoot;
		}
	};

}
