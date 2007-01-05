
#pragma once

namespace TurtleSvn {
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
		bool _allowUnversionedObstructions;
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

		[Obsolete("Available from Svn 1.5; currently ignored")]
		property bool AllowUnversionedObstructions
		{
			bool get()
			{
				return _allowUnversionedObstructions;
			}
			void set(bool value)
			{
				_allowUnversionedObstructions = value;
			}
		}
	};

	public ref class SvnUpdateArgs : public SvnClientArgs
	{
		bool _notRecursive;
		bool _ignoreExternals;
		bool _allowUnversionedObstructions;
		SvnRevision^ _revision;
	public:
		SvnUpdateArgs()
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

		[Obsolete("Available from Svn 1.5; currently ignored")]
		property bool AllowUnversionedObstructions
		{
			bool get()
			{
				return _allowUnversionedObstructions;
			}
			void set(bool value)
			{
				_allowUnversionedObstructions = value;
			}
		}
	};

	public enum class SvnEolStyle
	{
		Native,
		CrLf,
		Lf,
		Cr
	};	

	public ref class SvnExportArgs : public SvnClientArgs
	{
		bool _notRecursive;
		bool _ignoreExternals;
		bool _overwrite;
		SvnRevision^ _revision;
		SvnEolStyle _eolStyle;
	public:
		SvnExportArgs()
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

		property bool Overwrite
		{
			bool get()
			{
				return _overwrite;
			}
			void set(bool value)
			{
				_overwrite = value;
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

		property SvnEolStyle EolStyle
		{
			SvnEolStyle get()
			{
				return _eolStyle;
			}
			void set(SvnEolStyle value)
			{
				_eolStyle = value;
			}
		}
	};

	public ref class SvnCommitArgs : public SvnClientArgs
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

	public ref class SvnCleanUpArgs : public SvnClientArgs
	{
	public:
		SvnCleanUpArgs()
		{
		}
	};

}
