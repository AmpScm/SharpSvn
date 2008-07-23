// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;
	using System::Collections::ObjectModel::KeyedCollection;

	ref class SvnException;
	ref class SvnMergeRange;

	public ref class SvnEventArgs abstract : public System::EventArgs
	{
		static SvnEventArgs()
		{
			SvnBase::EnsureLoaded();
		}

	protected:
		SvnEventArgs()
		{
		}

	public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties values are stored in managed code</description>
		void Detach()
		{
			Detach(true);
		}
	protected public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties)
		{
			UNUSED_ALWAYS(keepProperties);
		}

	internal:
		static int SafeGetHashCode(Object^ value)
		{
			return value ? value->GetHashCode() : 0;
		}
	};
};