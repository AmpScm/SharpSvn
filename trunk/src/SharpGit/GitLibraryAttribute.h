#pragma once

namespace SharpGit {
	namespace Implementation {

		using System::String;
		using System::ArgumentNullException;
		using System::AttributeTargets;

		[System::AttributeUsage(AttributeTargets::Assembly, AllowMultiple=true)]
		public ref class GitLibraryAttribute sealed : public System::Attribute
		{
			initonly String^ _name;
			initonly String^ _version;
			bool _dynamicallyLinked;
			bool _skipPrefix;
			bool _optional;
			bool _useSharpGitVersion;

		public:
			GitLibraryAttribute(String^ name, String^ version)
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

			property bool UseSharpGitVersion
			{
				bool get()
				{
					return _useSharpGitVersion;
				}
				void set(bool value)
				{
					_useSharpGitVersion = value;
				}
			}
		};

		public ref class GitLibrary sealed
		{
			initonly String^ _name;
			initonly String^ _version;
			initonly bool _dynamicallyLinked;
			initonly System::Version^ _versionData;
			initonly bool _optional;

		internal:
			GitLibrary(GitLibraryAttribute^ attribute)
			{
				if (!attribute)
					throw gcnew ArgumentNullException("attribute");

				_name = attribute->Name;
				_version = attribute->Version;

				if(attribute->UseSharpGitVersion)
				{
					_versionData = (gcnew System::Reflection::AssemblyName(GitLibrary::typeid->Assembly->FullName))->Version;
					_version = _versionData->ToString();
				}
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