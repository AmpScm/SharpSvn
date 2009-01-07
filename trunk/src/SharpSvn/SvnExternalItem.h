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
	public ref class SvnExternalItem sealed : SvnBase
	{
		initonly String^ _target;
		initonly String^ _url;
		initonly SvnRevision^ _revision;
		initonly SvnRevision^ _pegRevision;

	public:
		SvnExternalItem(String^ targetName, String^ url)
		{	
			if (String::IsNullOrEmpty(targetName))
				throw gcnew ArgumentNullException("targetName");
			else if (String::IsNullOrEmpty(url))
				throw gcnew ArgumentNullException("url");
			
			_target = targetName;
			_url = url;
			_revision = _pegRevision = SvnRevision::None;
		}

		SvnExternalItem(String^ targetName, String^ url, SvnRevision^ revision, SvnRevision^ pegRevision)
		{
			if (String::IsNullOrEmpty(targetName))
				throw gcnew ArgumentNullException("targetName");
			else if (String::IsNullOrEmpty(url))
				throw gcnew ArgumentNullException("url");
			else if(revision && revision != SvnRevision::None && !revision->IsExplicit)
				throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
			else if(pegRevision && pegRevision != SvnRevision::None && !pegRevision->IsExplicit)
				throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "pegRevision");

			_target = targetName;
			_url = url;
			_revision = (revision && revision != SvnRevision::Head) ? revision : SvnRevision::None;
			_pegRevision = (pegRevision && pegRevision != SvnRevision::Head) ? pegRevision : SvnRevision::None;
		}

	public:
		/// <summary>Gets the target path of the external</summary>
		property String^ Target
		{
			String^ get()
			{
				return _target;
			}
		}

		/// <summary>Gets the absolute or relative url of the external</summary>
		property String^ Url
		{
			String^ get()
			{
				return _url;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
		}

		property SvnRevision^ OperationalRevision
		{
			SvnRevision^ get()
			{
				return _pegRevision;
			}
		}

		/// <summary>Creates a string containing the external in string format. Preferring formats compatible with older clients</summary>
		virtual String^ ToString() override;
		/// <summary>Creates a string containing the external in string format. Optionally preferring formats compatible with older clients</summary>
		String^ ToString(bool useCompatibleFormat);

		/// <summary>Writes the value to the specified <see cref="System::Text::StringBuilder" /></summary>
		void WriteTo(System::Text::StringBuilder^ sb, bool useCompatibleFormat);

	public:
		/// <summary>Tries to parse a single SvnExternalItem</summary>
		static bool TryParse(String^ value, [Out]SvnExternalItem^% item);
		/// <summary>Tries to parse a single SvnExternalItem</summary>
		static bool TryParse(String^ value, [Out]array<SvnExternalItem^>^% items);
	};
}