
#pragma once

#include "SvnEnums.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;


	public ref class SvnMergeSources : SvnBase
	{
		initonly SvnTarget^ _target;
		initonly bool _available;
		initonly IList<Uri^>^ _mergeSources;
	internal:
		SvnMergeSources(SvnTarget^ target, apr_array_header_t* mergeSources);


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
		property IList<Uri^>^ MergeSources
		{
			IList<Uri^>^ get()
			{
				return _mergeSources;
			}
		}
	};

};