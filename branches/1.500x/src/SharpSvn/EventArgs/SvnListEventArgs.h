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
		Uri^ _repositoryRoot;

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

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Time.GetHashCode() ^ SvnEventArgs::SafeGetHashCode(Author);
		}

	protected public:
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
		Uri^ _repositoryRoot;
		Uri^ _baseUri;
		Uri^ _entryUri;

	internal:
		SvnListEventArgs(const char *path, const svn_dirent_t *dirent, const svn_lock_t *lock, const char *abs_path, Uri^ repositoryRoot)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!abs_path)
				throw gcnew ArgumentNullException("abs_path");

			_path = SvnBase::Utf8_PtrToString(path);

			_pDirEnt = dirent;
			_pLock = lock;
			_pAbsPath = abs_path;
			_repositoryRoot = repositoryRoot;
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

		/// <summary>When retrieving a listing using an Uri target: contains the repository root</summary>
		/// <value>The Repository root or <c>null</c> when the repository root is not available</value>
		property Uri^ RepositoryRoot
		{
			Uri^ get()
			{
				return _repositoryRoot;
			}
		}

		/// <summary>When retrieving a listing using an Uri target: contains the uri from which Path is relative</summary>
		/// <value>The Base Uri or <c>null</c> when the repository root is not available</value>
		property Uri^ BaseUri
		{
			Uri^ get()
			{
				if (!_baseUri && (RepositoryRoot && (_absPath || _pAbsPath)))
				{
					if (String::IsNullOrEmpty(BasePath) ||
						(BasePath->Length == 1 && BasePath[0] == '/'))
					{
						_baseUri = RepositoryRoot;
					}
					else
					{
						bool isFile = false;
						if (String::IsNullOrEmpty(_path)) // = Request Root
						{
							if (_pDirEnt && _pDirEnt->kind != svn_node_dir)
								isFile = true;
							else if (Entry && Entry->NodeKind != SvnNodeKind::Directory)
								isFile = true;
						}

						_baseUri = gcnew Uri(RepositoryRoot, SvnBase::PathToUri(BasePath->Substring(1) + (isFile ? "" : "/")));
					}
				}

				return _baseUri;
			}
		}

		property Uri^ EntryUri
		{
			Uri^ get()
			{
				if (!_entryUri && BaseUri && Path && Entry)
				{
					if (Path->Length == 0)
						_entryUri = BaseUri;
					else if (Entry->NodeKind == SvnNodeKind::Directory)
						_entryUri = gcnew Uri(BaseUri, SvnBase::PathToUri(Path + "/"));
					else
						_entryUri = gcnew Uri(BaseUri, SvnBase::PathToUri(Path));
				}

				return _entryUri;
			}
		}

		/// <summary>Gets lock information if RetrieveLocks is set on the args object</summary>
		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if (!_lock && _pLock)
					_lock = gcnew SvnLockInfo(_pLock, false);

				return _lock;
			}
		}

		/// <summary>Gets the information specified in RetrieveEntries on the args object</summary>
		property SvnDirEntry^ Entry
		{
			SvnDirEntry^ get()
			{
				if (!_entry && _pDirEnt)
					_entry = gcnew SvnDirEntry(_pDirEnt);

				return _entry;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Path->GetHashCode();
		}


	protected public:
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