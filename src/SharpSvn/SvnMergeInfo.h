
#pragma once

#include "SvnEnums.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;
	using System::Collections::ObjectModel::KeyedCollection;
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
			if(!start)
				throw gcnew ArgumentNullException("start");
			else if(!end)
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

	internal:
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

		virtual int GetHashCode() override
		{
			return Start.GetHashCode();
		}

	public:
		virtual String^ ToString() override
		{
			return String::Format(System::Globalization::CultureInfo::InvariantCulture, "r{0}-{1}", Start, End);
		}
	};

	[DebuggerDisplayAttribute("Uri={Uri}")]
	public ref class SvnAppliedMergeItem sealed
	{
		initonly Uri^ _uri;
		IList<SvnMergeRange^>^ _ranges;

	internal:
		SvnAppliedMergeItem(Uri^ uri, IList<SvnMergeRange^>^ ranges)
		{
			if(!uri)
				throw gcnew ArgumentNullException("uri");
			else if(!ranges)
				throw gcnew ArgumentNullException("ranges");

			_uri = uri;
			_ranges = ranges;
		}


	internal:
		static IList<SvnMergeRange^>^ CreateRangeList(apr_array_header_t *rangeList);

	public:
		property Uri^ Uri
		{
			System::Uri^ get()
			{ 
				return _uri; 
			}
		}

		property IList<SvnMergeRange^>^ MergeRanges
		{
			IList<SvnMergeRange^>^ get()
			{
				return _ranges;
			}
		}
	};

	public ref class SvnAppliedMergeInfo sealed
	{
#pragma region // AppliedMergesList
	internal:
		ref class AppliedMergesList sealed : KeyedCollection<Uri^, SvnAppliedMergeItem^>
		{
		protected:
			virtual Uri^ GetKeyForItem(SvnAppliedMergeItem^ item) override
			{
				return item ? item->Uri : nullptr;
			}
		};
#pragma endregion

	private:
		initonly SvnTarget^ _target;
		initonly AppliedMergesList^ _appliedMerges;

	internal:
		SvnAppliedMergeInfo(SvnTarget^ target, apr_hash_t* mergeInfo, AprPool^ pool);

	public:
		property SvnTarget^ Target
		{
			SvnTarget^ get()
			{
				return _target;
			}
		}

		/// <summary>Gets a boolean indicating whether MergeInfo is available for the specified target</summary>
		property KeyedCollection<Uri^, SvnAppliedMergeItem^>^ AppliedMerges
		{
			KeyedCollection<Uri^, SvnAppliedMergeItem^>^ get()
			{
				return _appliedMerges;
			}
		}
	};

	public ref class SvnAvailableMergeInfo
	{
		initonly SvnTarget^ _target;
		initonly IList<SvnMergeRange^>^ _mergeRanges;
	internal:
		SvnAvailableMergeInfo(SvnTarget^ target, apr_array_header_t* mergeInfo, AprPool^ pool);

	public:
		/// <summary>Gets a boolean indicating whether MergeInfo is available for the specified target</summary>
		property SvnTarget^ Target
		{
			SvnTarget^ get()
			{
				return _target;
			}
		}

	public:
		property IList<SvnMergeRange^>^ MergeRanges
		{
			IList<SvnMergeRange^>^ get()
			{
				return _mergeRanges;
			}
		}
	};

};