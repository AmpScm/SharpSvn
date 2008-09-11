// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	[DebuggerDisplay("Path={Path}, Action={Action}")]
	public ref class SvnChangedEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _name;
		initonly String^ _path;
		initonly SvnChangeAction _action;
		initonly String^ _copyFromPath;
		initonly __int64 _copyFromRevision;
		initonly SvnNodeKind _kind;
		initonly bool _propertyModifications;
		initonly bool _textualModifications;

	internal:
		SvnChangedEventArgs(String^ path, String^ name, SvnChangeAction action, String^ copyFromPath, __int64 copyFromRevision, SvnNodeKind kind, bool propertyModifications, bool textualModifications)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!name)
				throw gcnew ArgumentNullException("name");

			_name = name;
			_path = path;
			_action = action;
			_copyFromPath = copyFromPath;
			_copyFromRevision = copyFromRevision;
			_kind = kind;
			_propertyModifications = propertyModifications;
			_textualModifications = textualModifications;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}
		property String^ Name
		{
			String^ get()
			{
				return _name;
			}
		}

		property SvnChangeAction Action
		{
			SvnChangeAction get()
			{
				return _action;
			}
		}

		property String^ CopyFromPath
		{
			String^ get()
			{
				return _copyFromPath;
			}
		}

		property __int64 CopyFromRevision
		{
			__int64 get()
			{
				return _copyFromRevision;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _kind;
			}
		}

		property bool PropertiesModified
		{
			bool get()
			{
				return _propertyModifications;
			}
		}

		property bool ContentModified
		{
			bool get()
			{
				return _textualModifications;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return SafeGetHashCode(Path);
		}

		virtual String^ ToString() override
		{
			return Path + ", " + Action.ToString() + ", " + NodeKind.ToString();
		}
	};
}
