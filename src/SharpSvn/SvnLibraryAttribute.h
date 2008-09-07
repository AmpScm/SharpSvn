// $Id: AprPool.h 623 2008-07-23 22:10:29Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {
	namespace Implementation {

		[System::AttributeUsage(AttributeTargets::Assembly, AllowMultiple=true)]
		public ref class SvnLibraryAttribute sealed : public System::Attribute
		{
			initonly String^ _name;
			initonly String^ _version;
			bool _dynamicallyLinked;

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
		};

		public ref class SvnLibrary sealed
		{
			initonly String^ _name;
			initonly String^ _version;
			initonly bool _dynamicallyLinked;
			initonly System::Version^ _versionData;
			
		internal:
			SvnLibrary(SvnLibraryAttribute^ attribute)
			{
				if (!attribute)
					throw gcnew ArgumentNullException("attribute");

				_name = attribute->Name;
				_version = attribute->Version;
				_dynamicallyLinked = attribute->DynamicallyLinked;

				System::Text::RegularExpressions::Match^ m =
					System::Text::RegularExpressions::Regex::Match(_version, "\\d+\\.(\\d+\\.)*\\d+");

				if (m->Success)
					_versionData = gcnew System::Version(m->Value);				
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
		};
	}
}