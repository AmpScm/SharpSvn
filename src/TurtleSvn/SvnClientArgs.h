
#pragma once

namespace QQn {
	namespace Svn {
		ref class SvnException;

		public ref class SvnClientArgs abstract
		{
			event EventHandler<SvnClientCancelEventArgs^>^		Cancel;
			event EventHandler<SvnClientProgressEventArgs^>^	Progress;
			event EventHandler<SvnClientNotifyEventArgs^>^		Notify;
			bool _noThrowOnError;
			SvnException^ _exception;

		protected public:
			SvnClientArgs()
			{
			}

			virtual void OnCancel(SvnClientCancelEventArgs^ e)
			{
				Cancel(this, e);
			}

			virtual void OnProgress(SvnClientProgressEventArgs^ e)
			{
				Progress(this, e);
			}

			virtual void OnNotify(SvnClientNotifyEventArgs^ e)
			{
				Notify(this, e);
			}

			property bool ThrowOnError
			{
				bool get()
				{
					return !_noThrowOnError;
				}
				void set(bool value)
				{
					_noThrowOnError = !value;
				}
			}

			property SvnException^ Exception
			{
				SvnException^ get()
				{
					return _exception;
				}
			}

		internal:
			bool HandleResult(svn_error_t *error);
		};

		public ref class SvnCheckOutArgs : public SvnClientArgs
		{
			bool _notRecursive;
			bool _ignoreExternals;
			SvnRevision^ _revision;
		public:
			SvnCheckOutArgs()
			{
				_revision = SvnRevision::Head;
			}

			property bool NotRecursive
			{
				bool get()
				{
					return _notRecursive;
				}
				void set(bool value)
				{
					_notRecursive = value;
				}
			}

			property bool IgnoreExternals
			{
				bool get()
				{
					return _ignoreExternals;
				}
				void set(bool value)
				{
					_ignoreExternals = value;
				}
			}

			property SvnRevision^ Revision
			{
				SvnRevision^ get()
				{
					return _revision;
				}
				void set(SvnRevision^ value)
				{
					if(value)
						_revision = value;
					else
						_revision = SvnRevision::Head;
				}
			}
		};

		public ref class SvnCommitArgs
		{
			event EventHandler<SvnClientCommitLogEventArgs^>^ GetCommitLog;

		protected public:
			SvnCommitArgs()
			{
			}

			virtual void OnGetCommitLog(SvnClientCommitLogEventArgs^ e)
			{
				GetCommitLog(this, e);
			}
		};
	}
}
