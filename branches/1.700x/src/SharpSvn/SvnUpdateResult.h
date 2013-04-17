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
	using namespace SharpSvn::Implementation;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::IEnumerator;

	public ref class SvnUpdateResult sealed : public SvnCommandResult
	{
		initonly __int64 _revision;
		initonly IDictionary<String^,SvnUpdateResult^>^ _resultMap;
	private:
		SvnUpdateResult(__int64 revision)
		{
			if (revision < 0)
				revision = -1;

			_revision = revision;
		}

	public:
		SvnUpdateResult(IDictionary<String^, SvnUpdateResult^>^ resultMap, __int64 revision)
		{
			if (!resultMap)
				throw gcnew ArgumentNullException("resultMap");
			if (revision < 0)
				revision = -1;

			_revision = revision;
			_resultMap = resultMap;
		}

	internal:
		static SvnUpdateResult^ Create(SvnClient^ client, SvnClientArgs^ args, __int64 revision)
		{
			if (!client)
				throw gcnew ArgumentNullException("client");
			else if (!args)
				throw gcnew ArgumentNullException("args");

			return gcnew SvnUpdateResult(revision);
		}

	internal:
		SvnUpdateResult(ICollection<String^>^ paths, ICollection<__int64>^ revisions, __int64 revision)
		{
			if (!paths)
				throw gcnew ArgumentNullException("paths");
			else if (!revisions)
				throw gcnew ArgumentNullException("revisions");
			else if (paths->Count != revisions->Count)
				throw gcnew ArgumentException(SharpSvnStrings::PathCountDoesNotMatchRevisions, "paths");

			if (revision < 0)
				revision = -1;

			_revision = revision;
			_resultMap = gcnew System::Collections::Generic::SortedList<String^, SvnUpdateResult^>();
			IEnumerator<String^>^ ePath = paths->GetEnumerator();
			IEnumerator<__int64>^ eRev = revisions->GetEnumerator();

			while (ePath->MoveNext() && eRev->MoveNext())
			{
				if (!(_resultMap->ContainsKey(ePath->Current)))
					_resultMap->Add(ePath->Current, gcnew SvnUpdateResult(eRev->Current));
			}
		}

	public:
		property bool HasRevision
		{
			bool get()
			{
				return _revision >= 0;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property bool HasResultMap
		{
			bool get()
			{
				return (_resultMap != nullptr);
			}
		}

		property IDictionary<String^,SvnUpdateResult^>^ ResultMap
		{
			IDictionary<String^,SvnUpdateResult^>^ get()
			{
				return _resultMap;
			}
		}

		virtual int GetHashCode() override
		{
			return Revision.GetHashCode();
		}
	};

}
