// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	public ref class SvnChangedEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _name;
		initonly SvnChangeAction _action;
		initonly String^ _copyFromPath;
		initonly __int64 _copyFromRevision;
		initonly SvnNodeKind _kind;
		initonly bool _propertyModifications;
		initonly bool _textualModifications;

	internal:
		SvnChangedEventArgs(String^ name, SvnChangeAction action, String^ copyFromPath, __int64 copyFromRevision, SvnNodeKind kind, bool propertyModifications, bool textualModifications)
		{
			_name = name;
			_action = action;
			_copyFromPath = copyFromPath;
			_copyFromRevision = copyFromRevision;
			_kind = kind;
			_propertyModifications = propertyModifications;
			_textualModifications = textualModifications;
		}
	public:
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

		property SvnNodeKind Kind
		{
			SvnNodeKind get()
			{
				return _kind;
			}
		}

		property bool PropertyModifications
		{
			bool get()
			{
				return _propertyModifications;
			}
		}

		property bool TextualModifications
		{
			bool get()
			{
				return _textualModifications;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return SafeGetHashCode(Name);
		}

		virtual String^ ToString() override
		{
			return this->Name + "," + this->Action.ToString() + "," + this->Kind.ToString();
		}
	};
}
