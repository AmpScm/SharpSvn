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
		const char* _pChangeList;
		String^ _changelist;

	internal:
		SvnListChangeListEventArgs(const char *path, const char *changelist)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");

			_path = SvnBase::Utf8_PtrToString(path)->Replace('/', System::IO::Path::DirectorySeparatorChar);
			_pChangeList = changelist;
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
				if(!_changelist && _pChangeList)
					_changelist = SvnBase::Utf8_PtrToString(_pChangeList);

				return _changelist;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(ChangeList);
				}
			}
			finally
			{
				_pChangeList = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}