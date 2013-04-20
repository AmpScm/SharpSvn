#pragma once

namespace SharpGit {

	ref class GitStatusArgs;

	public enum class GitStatus
	{
		Normal = 0,

		New			= 0x0001,
		Added		= 0x0002,

		Modified	= 0x0010,

		Deleted		= 0x0100,

		Ignored		= 0x1000,
	};

	public ref class GitStatusEventArgs : public GitClientEventArgs
	{
		initonly unsigned _status;
		String^ _relPath;
		String^ _fullPath;
		GitPool^ _pool;
		const char *_path;
		const char *_wcPath;

	internal:
		GitStatusEventArgs(const char *path, const char *wcPath, unsigned status, GitStatusArgs ^args, Implementation::GitPool ^pool);

	public:
		property String^ RelativePath
		{
			String^ get()
			{
				if (!_relPath && _path)
					_relPath = GitBase::Utf8_PtrToString(_path);

				return _relPath;
			}
		}

		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && _pool && _path && _wcPath)
					_fullPath = GitBase::StringFromDirent(svn_dirent_join(_wcPath, _path, _pool->Handle), _pool);

				return _fullPath;
			}
		}

	public:
		property GitStatus IndexStatus
		{
			GitStatus get()
			{
				switch (_status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED))
				{
				case 0:
					return GitStatus::Normal;
				case GIT_STATUS_INDEX_NEW:
					return GitStatus::Added;
				case GIT_STATUS_INDEX_MODIFIED:
					return GitStatus::Modified;
				case GIT_STATUS_INDEX_DELETED:
					return GitStatus::Deleted;
				default:
					assert(false);
					return GitStatus::Modified;
				}
			}
		}

		property GitStatus WorkingDirectoryStatus
		{
			GitStatus get()
			{
				switch (_status & (GIT_STATUS_WT_NEW | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED))
				{
				case 0:
					return GitStatus::Normal;
				case GIT_STATUS_WT_NEW:
					return GitStatus::New;
				case GIT_STATUS_WT_MODIFIED:
					return GitStatus::Modified;
				case GIT_STATUS_WT_DELETED:
					return GitStatus::Deleted;
				default:
					assert(false);
					return GitStatus::Modified;
				}
			}
		}

		property bool Ignored
		{
			bool get()
			{
				return (_status & GIT_STATUS_IGNORED) != 0;
			}
		}

	protected:
		virtual void Detach(bool keepValues) override
		{
			try
			{
				if (keepValues)
				{
					GC::KeepAlive(RelativePath);
					GC::KeepAlive(FullPath);
				}
			}
			finally
			{
				_pool = nullptr;
				_path = nullptr;
				_wcPath = nullptr;

				__super::Detach(keepValues);
			}
		}
	};
}