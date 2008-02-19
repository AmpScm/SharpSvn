// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {
	public ref class SvnLockInfo sealed
	{
		const svn_lock_t *_lock;
		initonly bool _localData;
		String^ _path;
		String^ _fullPath;
		String^ _token;
		String^ _owner;
		String^ _comment;
		initonly bool _isDavComment;
		initonly DateTime _creationDate;
		initonly DateTime _expirationDate;
	internal:
		SvnLockInfo(const svn_lock_t *lock, bool localData)
		{
			if (!lock)
				throw gcnew ArgumentNullException("lock");

			_localData = localData;
			_lock = lock;
			_isDavComment = lock->is_dav_comment != 0;
			_creationDate = SvnBase::DateTimeFromAprTime(lock->creation_date);
			_expirationDate = SvnBase::DateTimeFromAprTime(lock->expiration_date);
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				if (!_path && _lock)
					_path = SvnBase::Utf8_PtrToString(_lock->path);

				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="System::IO::Path::GetFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && _localData && Path)
					_fullPath = System::IO::Path::GetFullPath(Path);

				return _fullPath;
			}
		}

		property String^ Token
		{
			String^ get()
			{
				if (!_token && _lock)
					_token = SvnBase::Utf8_PtrToString(_lock->token);

				return _token;
			}
		}

		property String^ Owner
		{
			String^ get()
			{
				if (!_owner && _lock)
					_owner = SvnBase::Utf8_PtrToString(_lock->owner);

				return _owner;
			}
		}

		property String^ Comment
		{
			String^ get()
			{
				if (!_comment && _lock && _lock->comment)
				{
					_comment = SvnBase::Utf8_PtrToString(_lock->comment);

					if (_comment)
						_comment = _comment->Replace("\n", Environment::NewLine);
				}

				return _comment;
			}
		}

		/// <summary>Gets a boolean indicating whether the Comment was generated by a Generic WebDav client</summary>
		property bool IsRawNetworkComment
		{
			bool get()
			{
				return _isDavComment;
			}
		}

		property DateTime CreationTime
		{
			DateTime get()
			{
				return _creationDate;
			}
		}

		property DateTime ExpirationTime
		{
			DateTime get()
			{
				return _expirationDate;
			}
		}

		void Detach()
		{
			Detach(true);
		}
	protected public:
		virtual void Detach(bool keepProperties)
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(Token);
					GC::KeepAlive(Owner);
					GC::KeepAlive(Comment);
				}
			}
			finally
			{
				_lock = nullptr;
				//__super::Detach(keepProperties);
			}
		}
	};

}