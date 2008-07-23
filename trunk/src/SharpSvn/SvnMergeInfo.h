// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnEnums.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::IEnumerable;
	using System::Collections::Generic::SortedList;
	using System::Collections::ObjectModel::KeyedCollection;
	using System::Collections::ObjectModel::Collection;
	using System::Diagnostics::DebuggerDisplayAttribute;

	[DebuggerDisplayAttribute("Range={StartRevision}-{EndRevision}")]
	public ref class SvnRevisionRange
	{
		initonly SvnRevision^ _start;
		initonly SvnRevision^ _end;

	public:
		SvnRevisionRange(__int64 start, __int64 end)
		{
			_start = gcnew SvnRevision(start);
			_end = gcnew SvnRevision(end);
		}

		SvnRevisionRange(SvnRevision^ start, SvnRevision^ end)
		{
			if (!start)
				throw gcnew ArgumentNullException("start");
			else if (!end)
				throw gcnew ArgumentNullException("end");

			_start = start;
			_end = end;
		}

		property SvnRevision^ StartRevision
		{
			SvnRevision^ get()
			{
				return _start;
			}
		}

		property SvnRevision^ EndRevision
		{
			SvnRevision^ get()
			{
				return _end;
			}
		}
	};

	[DebuggerDisplayAttribute("Range=r{Start}-{End}, Inheritable={Inheritable}")]
	public ref class SvnMergeRange sealed : SvnRevisionRange
	{
		initonly bool _inheritable;

	public:
		SvnMergeRange(__int64 start, __int64 end, bool inheritable)
			: SvnRevisionRange(start, end)
		{
			_inheritable = inheritable;
		}

	public:
		property __int64 Start
		{
			__int64 get()
			{
				return StartRevision->Revision;
			}
		}

		property __int64 End
		{
			__int64 get()
			{
				return EndRevision->Revision;
			}
		}

		property bool Inheritable
		{
			bool get()
			{
				return _inheritable;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Start.GetHashCode();
		}

	public:
		virtual String^ ToString() override
		{
			return String::Format(System::Globalization::CultureInfo::InvariantCulture, "r{0}-{1}", Start, End);
		}

	internal:
		svn_merge_range_t* AllocMergeRange(AprPool^ pool);
	};

	ref class SvnMergeItem;	
	ref class SvnMergeDiffArgs;
	ref class SvnMergeRemoveArgs;
	ref class SvnMergeIntersectArgs;

	public ref class SvnMergeItemCollection sealed : KeyedCollection<Uri^, SvnMergeItem^>
	{
	internal:
		SvnMergeItemCollection(svn_mergeinfo_t mergeInfo, AprPool^ pool);

	public:
		/// <summary>Initializes a new instance of the <see cref="SvnMergeItemCollection" /> class with default properties</summary>
		SvnMergeItemCollection()
		{}

		/// <summary>Initializes a new instance of the <see cref="SvnMergeItemCollection" /> class with the values from the specified collection</summary>
		SvnMergeItemCollection(IEnumerable<SvnMergeItem^>^ items)
		{
			if (!items)
				throw gcnew ArgumentNullException("items");

			for each (SvnMergeItem^ item in items)
			{
				Add(item);
			}
		}

	public:
		static bool TryParse(String^ input, [Out] SvnMergeItemCollection^% items);

	public:
		bool TryDiff(ICollection<SvnMergeItem^>^ to, [Out] SvnMergeItemCollection^% added, [Out] SvnMergeItemCollection^% removed);
		bool TryDiff(ICollection<SvnMergeItem^>^ to, SvnMergeDiffArgs^ args, [Out] SvnMergeItemCollection^% added, [Out] SvnMergeItemCollection^% removed);

	public:
		bool TryRemove(ICollection<SvnMergeItem^>^ items, [Out] SvnMergeItemCollection^% rest);
		/// <remarks>ConsiderInheritance is ignored at this time</remarks>
		bool TryRemove(ICollection<SvnMergeItem^>^ items, SvnMergeRemoveArgs^ args, [Out] SvnMergeItemCollection^% rest);
		bool TryIntersect(ICollection<SvnMergeItem^>^ to, [Out] SvnMergeItemCollection^% intersected);
		/// <remarks>ConsiderInheritance is ignored at this time</remarks>
		bool TryIntersect(ICollection<SvnMergeItem^>^ to, SvnMergeIntersectArgs^ args, [Out] SvnMergeItemCollection^% intersected);

	public:
		virtual String^ ToString() override;

	internal:
		svn_mergeinfo_t AllocMergeInfo(AprPool^ pool);

	protected:
		virtual Uri^ GetKeyForItem(SvnMergeItem^ item) override;
	};

	public ref class SvnMergeRangeCollection sealed : Collection<SvnMergeRange^>
	{
	public:
		SvnMergeRangeCollection()
		{}

		SvnMergeRangeCollection(ICollection<SvnMergeRange^>^ list)
		{
			if (!list)
				throw gcnew ArgumentNullException("list");

			for each (SvnMergeRange^ i in list)
				Add(i);
		}

	public:
		bool TryDiff(ICollection<SvnMergeRange^>^ to, [Out] SvnMergeRangeCollection^% added, [Out] SvnMergeRangeCollection^% removed);
		bool TryDiff(ICollection<SvnMergeRange^>^ to, SvnMergeDiffArgs^ args, [Out] SvnMergeRangeCollection^% added, [Out] SvnMergeRangeCollection^% removed);

	public:
		bool TryRemove(ICollection<SvnMergeRange^>^ items, [Out] SvnMergeRangeCollection^% rest);
		bool TryRemove(ICollection<SvnMergeRange^>^ items, SvnMergeRemoveArgs^ args, [Out] SvnMergeRangeCollection^% rest);
		bool TryIntersect(ICollection<SvnMergeRange^>^ to, [Out] SvnMergeRangeCollection^% intersected);
		bool TryIntersect(ICollection<SvnMergeRange^>^ to, SvnMergeIntersectArgs^ args, [Out] SvnMergeRangeCollection^% intersected);

	public:
		virtual String^ ToString() override;

	internal:
		static SvnMergeRangeCollection^ Create(apr_array_header_t *rangeList);

		apr_array_header_t *AllocMergeRange(AprPool^ pool);
	};


	[DebuggerDisplayAttribute("Uri={Uri}")]
	public ref class SvnMergeItem sealed
	{
		initonly Uri^ _uri;
		SvnMergeRangeCollection^ _ranges;

	internal:
		SvnMergeItem(Uri^ uri, SvnMergeRangeCollection^ ranges)
		{
			if (!uri)
				throw gcnew ArgumentNullException("uri");
			else if (!ranges)
				throw gcnew ArgumentNullException("ranges");

			_uri = uri;
			_ranges = ranges;
		}

	public:
		property Uri^ Uri
		{
			System::Uri^ get()
			{
				return _uri;
			}
		}

		property SvnMergeRangeCollection^ MergeRanges
		{
			SvnMergeRangeCollection^ get()
			{
				return _ranges;
			}
		}
	};

	public ref class SvnAppliedMergeInfo sealed
	{
	private:
		initonly SvnTarget^ _target;
		initonly SvnMergeItemCollection^ _appliedMerges;

	internal:
		SvnAppliedMergeInfo(SvnTarget^ target, svn_mergeinfo_t mergeInfo, AprPool^ pool);

	public:
		property SvnTarget^ Target
		{
			SvnTarget^ get()
			{
				return _target;
			}
		}

		/// <summary>Gets a list of applied merges</summary>
		property SvnMergeItemCollection^ AppliedMerges
		{
			SvnMergeItemCollection^ get()
			{
				return _appliedMerges;
			}
		}
	};
};