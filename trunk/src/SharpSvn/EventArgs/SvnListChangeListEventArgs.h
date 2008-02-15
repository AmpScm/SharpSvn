// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnListChangeListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		initonly String^ _changelist;

	internal:
		SvnListChangeListEventArgs(const char *path, const char *changelist)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");

			_path = SvnBase::Utf8_PtrToString(path)->Replace('/', System::IO::Path::DirectorySeparatorChar);
			_changelist = SvnBase::Utf8_PtrToString(changelist);
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property String^ ChangeList
		{
			String^ get()
			{
				return _changelist;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
				}
			}
			finally
			{
				__super::Detach(keepProperties);
			}
		}
	};

}