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