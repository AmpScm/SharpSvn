// $Id: AprArray.h 534 2008-04-14 22:52:59Z rhuijben $
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
	namespace Implementation {

		[System::AttributeUsage(AttributeTargets::Assembly, AllowMultiple=true)]
		public ref class SvnLibraryAttribute sealed : public System::Attribute
		{
			initonly String^ _name;
			initonly String^ _version;
			bool _dynamicallyLinked;
			bool _skipPrefix;
			bool _optional;
			bool _useSharpSvnVersion;

		public:
			SvnLibraryAttribute(String^ name, String^ version)
			{
				if (String::IsNullOrEmpty(name))
					throw gcnew ArgumentNullException("name");
				else if (!version)
					throw gcnew ArgumentNullException("version");

				_name = name;
				_version = version;
			}

		public:
			property String^ Name
			{
				String^ get()
				{
					return _name;
				}
			}

			property String^ Version
			{
				String^ get()
				{
					return _version;
				}
			}

			property bool DynamicallyLinked
			{
				bool get()
				{
					return _dynamicallyLinked;
				}
				void set(bool value)
				{
					_dynamicallyLinked = value;
				}
			}

			property bool SkipPrefix
			{
				bool get()
				{
					return _skipPrefix;
				}
				void set(bool value)
				{
					_skipPrefix = value;
				}
			}

			property bool Optional
			{
				bool get()
				{
					return _optional;
				}
				void set(bool value)
				{
					_optional = value;
				}
			}

			property bool UseSharpSvnVersion
			{
				bool get()
				{
					return _useSharpSvnVersion;
				}
				void set(bool value)
				{
					_useSharpSvnVersion = value;
				}
			}
		};

		public ref class SvnLibrary sealed
		{
			initonly String^ _name;
			initonly String^ _version;
			initonly bool _dynamicallyLinked;
			initonly System::Version^ _versionData;
			initonly bool _optional;

		internal:
			SvnLibrary(SvnLibraryAttribute^ attribute)
			{
				if (!attribute)
					throw gcnew ArgumentNullException("attribute");

				_name = attribute->Name;
				_version = attribute->Version;

				if(attribute->UseSharpSvnVersion)
					_version = (gcnew System::Reflection::AssemblyName(SvnLibrary::typeid->Assembly->FullName))->Version->ToString();
				else
				{
					System::Text::RegularExpressions::Match^ m =
						System::Text::RegularExpressions::Regex::Match(_version, "\\d+\\.(\\d+\\.)*\\d+");

					if (m->Success)
					{
						_versionData = gcnew System::Version(m->Value);

						if (attribute->SkipPrefix)
							_version = _version->Substring(m->Index);
					}
				}

				_dynamicallyLinked = attribute->DynamicallyLinked;
				_optional = attribute->Optional;
			}

		public:
			property String^ Name
			{
				String^ get()
				{
					return _name;
				}
			}

			property String^ VersionString
			{
				String^ get()
				{
					return _version;
				}
			}

			property System::Version^ Version
			{
				System::Version^ get()
				{
					return _versionData;
				}
			}

			property bool DynamicallyLinked
			{
				bool get()
				{
					return _dynamicallyLinked;
				}
			}

			property bool Optional
			{
				bool get()
				{
					return _optional;
				}
			}
		};
	}
}