
#pragma once

namespace QQn {
	namespace Svn {

		public ref class SvnClientEventArgs abstract : public System::EventArgs
		{
			apr_pool_t *_pool;

		internal:
			SvnClientEventArgs(apr_pool_t *pool)
			{
				_pool = pool;
			}

		protected:
			SvnClientEventArgs()
			{
			}
		};

		public ref class SvnClientCancelEventArgs : public SvnClientEventArgs
		{
			bool _cancel;

		public:
			SvnClientCancelEventArgs()
			{
			}

			property bool Cancel
			{
				bool get()
				{
					return _cancel;
				}
				void set(bool value)
				{
					_cancel = value;
				}
			}
		};

		public ref class SvnClientProgressEventArgs : public SvnClientEventArgs
		{
			initonly __int64 _progress;
			initonly __int64 _totalProgress;


		internal:
			SvnClientProgressEventArgs(__int64 progress, __int64 totalProgress, apr_pool_t *pool)
				: SvnClientEventArgs(pool)
			{
				_progress = progress;
				_totalProgress = totalProgress;
			}

		public:
			SvnClientProgressEventArgs(__int64 progress, __int64 totalProgress)
			{
				_progress = progress;
				_totalProgress = totalProgress;
			}

		public:
			property __int64 Progress
			{
				__int64 get()
				{
					return _progress;
				}
			}

			property __int64 TotalProgress
			{
				__int64 get()
				{
					return _totalProgress;
				}
			}
		};

		public ref class SvnClientCommitLogEventArgs : public SvnClientEventArgs
		{
		internal:
			SvnClientCommitLogEventArgs()
			{
			}
		};

		public ref class SvnClientNotifyEventArgs : public SvnClientEventArgs
		{
		internal:
			SvnClientNotifyEventArgs()
			{
			}
		};
	}
}