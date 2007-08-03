
#pragma once

#include "SvnEnums.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;


	public ref class SvnMergeInfo : SvnBase
	{
		initonly SvnTarget^ _target;
		initonly bool _available;
		initonly IList<String^>^ _mergeSources;
	internal:
		SvnMergeInfo(SvnTarget^ target, apr_hash_t* mergeInfo);
		

	public:
		/// <summary>Gets a boolean indicating whether MergeInfo is available for the specified target</summary>
		property bool Available
		{
			bool get()
			{
				return _available;
			}
		}

		property SvnTarget^ Target
		{
			SvnTarget^ get()
			{
				return _target;
			}
		}

	public:
		property IList<String^>^ MergeSources
		{
			IList<String^>^ get()
			{
				return _mergeSources;
			}
		}
	};

};