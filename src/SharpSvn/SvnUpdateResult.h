// $Id: SvnCommitItem.h 171 2007-10-14 19:38:35Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {
	using namespace SharpSvn::Apr;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::IEnumerator;


	public ref class SvnUpdateResult sealed
	{
		initonly __int64 _revision;
		initonly IDictionary<String^,SvnUpdateResult^>^ _resultMap;
	public:
		SvnUpdateResult(__int64 revision)
		{
			if(revision < 0)
				revision = -1;

			_revision = revision;
		}

		SvnUpdateResult(IDictionary<String^, SvnUpdateResult^>^ resultMap, __int64 revision)
		{
			if(revision < 0)
				revision = -1;

			_revision = revision;
			_resultMap = resultMap;
		}

	internal:
		SvnUpdateResult(ICollection<String^>^ paths, ICollection<__int64>^ revisions, __int64 revision)
		{
			if(!paths)
				throw gcnew ArgumentNullException("paths");
			else if(!revisions)
				throw gcnew ArgumentNullException("revisions");
			else if(paths->Count != revisions->Count)
				throw gcnew ArgumentException();

			if(revision < 0)
				revision = -1;

			_revision = revision;
			_resultMap = gcnew System::Collections::Generic::SortedList<String^, SvnUpdateResult^>();
			IEnumerator<String^>^ ePath = paths->GetEnumerator();
			IEnumerator<__int64>^ eRev = revisions->GetEnumerator();

			while(ePath->MoveNext() && eRev->MoveNext())
			{
				if(!(_resultMap->ContainsKey(ePath->Current)))
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
	};

}
