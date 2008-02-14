// $Id: SvnClientArgs_Wc.h 266 2008-02-14 13:24:29Z bhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnDirEntry
	{
		const svn_dirent_t *_entry;
		initonly SvnNodeKind _nodeKind;
		initonly __int64 _fileSize;
		initonly bool _hasProperties;
		initonly __int64 _rev;
		initonly DateTime _time;
		String^ _author;
	internal:
		SvnDirEntry(const svn_dirent_t *entry)
		{
			if (!entry)
				throw gcnew ArgumentNullException("entry");

			_entry = entry;
			_nodeKind = (SvnNodeKind)entry->kind;
			_fileSize = entry->size;
			_hasProperties = (entry->has_props != 0);
			_rev = entry->created_rev;
			_time = (entry->time != 0) ? SvnBase::DateTimeFromAprTime(entry->time) : DateTime::MinValue;
		}
	public:
		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>Gets the length of the file text or 0 for directories</summary>
		property __int64 FileSize
		{
			__int64 get()
			{
				return _fileSize;
			}
		}

		/// <summary>Gets a boolean indicating whether this node has svn properties</summary>
		property bool HasProperties
		{
			bool get()
			{
				return _hasProperties;
			}
		}

		/// <summary>Gets the last revision in which this node changed</summary>
		property __int64 Revision
		{
			__int64 get()
			{
				return _rev;
			}
		}

		/// <summary>Gets the time of the last change</summary>
		property DateTime Time
		{
			DateTime get()
			{
				return _time;
			}
		}

		/// <summary>Gets the author of the last revision of this file</summary>
		property String^ Author
		{
			String^ get()
			{
				if (!_author && _entry && _entry->last_author)
					_author = SvnBase::Utf8_PtrToString(_entry->last_author);

				return _author;
			}
		}

	public:
		virtual void Detach(bool keepProperties)
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Author);
				}

			}
			finally
			{
				_entry = nullptr;

				//__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		const char* _pAbsPath;
		const svn_lock_t *_pLock;
		const svn_dirent_t *_pDirEnt;

		String^ _absPath;
		SvnLockInfo^ _lock;
		SvnDirEntry^ _entry;

	internal:
		SvnListEventArgs(const char *path, const svn_dirent_t *dirent, const svn_lock_t *lock, const char *abs_path)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");
			else if(!abs_path)
				throw gcnew ArgumentNullException("abs_path");

			_path = SvnBase::Utf8_PtrToString(path);

			_pDirEnt = dirent;
			_pLock = lock;
			_pAbsPath = abs_path;
		}

	public:
		/// <summary>Gets the path of the item</summary>
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		/// <summary>Gets the origin path of the item</summary>
		property String^ BasePath
		{
			String^ get()
			{
				if (!_absPath && _pAbsPath)
					_absPath = SvnBase::Utf8_PtrToString(_pAbsPath);

				return _absPath;
			}
		}

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if (!_lock && _pLock)
					_lock = gcnew SvnLockInfo(_pLock, false);

				return _lock;
			}
		}

		property SvnDirEntry^ Entry
		{
			SvnDirEntry^ get()
			{
				if (!_entry && _pDirEnt)
					_entry = gcnew SvnDirEntry(_pDirEnt);

				return _entry;
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
					GC::KeepAlive(BasePath);
					GC::KeepAlive(Lock);
					GC::KeepAlive(Entry);
				}

				if (_lock)
					_lock->Detach(keepProperties);
				if (_entry)
					_entry->Detach(keepProperties);
			}
			finally
			{
				_pAbsPath = nullptr;
				_pLock = nullptr;
				_pDirEnt = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

}