#pragma once

namespace SharpSvn {

	[DebuggerDisplay("Path={Path}")]
	public ref class SvnLookListEventArgs : public SvnCancelEventArgs
	{
		const char *_pPath;
		const svn_fs_dirent_t *_pEntry;
		AprPool^ _pool;

		String^ _name;
		String^ _path;
		initonly SvnNodeKind _kind;
	internal:
		SvnLookListEventArgs(const char *path, const svn_fs_dirent_t *entry, AprPool ^pool)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!entry)
				throw gcnew ArgumentNullException("entry");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_pPath = path;
			_pEntry = entry;
			_pool = pool;
			_kind = (SvnNodeKind)entry->kind;
		}

	public:
		property String^ Name
		{
			String^ get()
			{
				if (!_name && _pEntry)
					_name = SvnBase::Utf8_PtrToString(_pEntry->name);

				return _name;
			}
		}

		property String^ Path
		{
			String^ get()
			{
				if (!_path && (_pPath || _pEntry) && _pool)
					_path = SvnBase::Utf8_PtrToString(svn_path_join(_pPath, _pEntry->name, _pool->Handle));

				return _path;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _kind;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Name);
					GC::KeepAlive(Path);
				}
			}
			finally
			{
				_pPath = nullptr;
				_pEntry = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};
}
