// $Id: SvnStatusEventArgs.h 579 2008-06-11 10:57:43Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnLockInfo.h"
// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnWorkingCopyEntryEventArgs sealed : public SvnEventArgs
	{
		initonly String^ _basePath;
		const char* _pPath;
		const svn_wc_entry_t* _entry;

		String^ _path;
		String^ _fullPath;
		String^ _name;
		initonly SvnDepth _depth;
		initonly SvnSchedule _schedule;

	internal:
		SvnWorkingCopyEntryEventArgs(String^ basePath, const char* path, const svn_wc_entry_t* entry)
		{
			if (String::IsNullOrEmpty(basePath))
				throw gcnew ArgumentNullException("basePath");
			else if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!entry)
				throw gcnew ArgumentNullException("entry");

			_basePath = basePath;
			_pPath = path;
			_entry = entry;
			_depth = (SvnDepth)entry->depth;
			_schedule = (SvnSchedule)entry->schedule;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				if (!_path && _pPath)
					_path = SvnBase::Utf8_PtrToString(_pPath);

				return _path;
			}
		}

		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && Path)
					_fullPath = (Path->Length > 0) ? System::IO::Path::Combine(_basePath, Path) : _basePath;

				return _fullPath;
			}
		}

		property String^ Name
		{
			String^ get()
			{
				if (!_name && _entry)
					_name = SvnBase::Utf8_PtrToString(_entry->name);

				return _fullPath;
			}
		}

		property SvnSchedule Schedule
		{
			SvnSchedule get()
			{
				return _schedule;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
		}


	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(FullPath);
					GC::KeepAlive(Name);
				}
			}
			finally
			{
				_pPath = nullptr;
				_entry = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
};