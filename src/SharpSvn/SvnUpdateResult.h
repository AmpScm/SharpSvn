// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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

		SvnUpdateResult(IDictionary<String^, SvnUpdateResult^>^ resultMap, __int64 revision)
		{
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

		static SvnUpdateResult^ Create(SvnClient^ client, SvnClientArgs^ args, IDictionary<String^, SvnUpdateResult^>^ resultMap, __int64 revision)
		{
			if (!client)
				throw gcnew ArgumentNullException("client");
			else if (!args)
				throw gcnew ArgumentNullException("args");

			return gcnew SvnUpdateResult(resultMap, revision);
		}


	internal:
		SvnUpdateResult(ICollection<String^>^ paths, ICollection<__int64>^ revisions, __int64 revision)
		{
			if (!paths)
				throw gcnew ArgumentNullException("paths");
			else if(!revisions)
				throw gcnew ArgumentNullException("revisions");
			else if(paths->Count != revisions->Count)
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
