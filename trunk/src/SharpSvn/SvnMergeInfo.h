
#pragma once

#include "SvnEnums.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;
	using System::Collections::ObjectModel::KeyedCollection;
	using System::Diagnostics::DebuggerDisplayAttribute;

	[DebuggerDisplayAttribute("Range=r{Start}-{End}, Inheritable={Inheritable}")]
	public ref class SvnMergeRange sealed
	{
		initonly __int64 _start;
		initonly __int64 _end;
		initonly bool _inheritable;

	internal:
		SvnMergeRange(__int64 start, __int64 end, bool inheritable)
		{
			_start = start;
			_end = end;
			_inheritable = inheritable;
		}

	public:
		property __int64 Start
		{
			__int64 get()
			{
				return _start;
			}
		}

		property __int64 End
		{
			__int64 get()
			{
				return _end;
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
			return (_start + _end).GetHashCode();
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