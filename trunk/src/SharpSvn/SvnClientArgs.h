// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnEnums.h"
#include "SvnChangeList.h"
#include "SvnCommandLineArgumentCollection.h"

namespace SharpSvn {
	ref class SvnException;
	using System::Collections::ObjectModel::Collection;

	/// <summary>Base class of all <see cref="SvnClient" /> arguments</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientArgs abstract
	{
		static SvnClientArgs()
		{
			SvnBase::EnsureLoaded();
		}

		bool _noThrowOnError;
		bool _noThrowOnCancel;
		SvnException^ _exception;

	public:
		/// <summary>Raised to allow canceling an in-progress command</summary>
		event EventHandler<SvnCancelEventArgs^>^	Cancel;
		/// <summary>Raised to notify of progress by an in-progress command</summary>
		event EventHandler<SvnProgressEventArgs^>^	Progress;
		/// <summary>Raised to notify changes by an in-progress command</summary>
		event EventHandler<SvnNotifyEventArgs^>^	Notify;
		/// <summary>Raised to notify errors from an command</summary>
		event EventHandler<SvnErrorEventArgs^>^		SvnError;

	protected public:
		SvnClientArgs()
		{
		}

		/// <summary>Raises the <see cref="Cancel" /> event</summary>
		virtual void OnCancel(SvnCancelEventArgs^ e)
		{
			Cancel(this, e);
		}

		/// <summary>Raises the <see cref="Progress" /> event</summary>
		virtual void OnProgress(SvnProgressEventArgs^ e)
		{
			Progress(this, e);
		}

		/// <summary>Raises the <see cref="Notify" /> event</summary>
		virtual void OnNotify(SvnNotifyEventArgs^ e)
		{
			Notify(this, e);
		}

		/// <summary>Called when handling a <see cref="SvnException" /></summary>
		virtual void OnSvnError(SvnErrorEventArgs^ e)
		{
			SvnError(this, e);
		}

	internal:
		virtual void RaiseOnNotify(SvnNotifyEventArgs^ e)
		{
			OnNotify(e);
		}


	public:
		/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
		virtual property SvnCommandType CommandType
		{
			SvnCommandType get() abstract;
		}

		/// <summary>
		/// Gets or sets a boolean indicating whether the call must throw an error if an exception occurs.
		/// If an exception would occur, the method returns false and the <see cref="LastException" /> property
		/// is set to the exception which would have been throw.
		/// </summary>
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

		/// <summary>
		/// Gets or sets a boolean indicating whether the call must throw an error if the operation is cancelled
		/// <see cref="IsLastInvocationCanceled" /> is true and the returnvalue <c>false</c> if the operation was canceled.
		/// (The <see cref="LastException" /> property is set to the cancel exception)
		/// </summary>
		property bool ThrowOnCancel
		{
			bool get()
			{
				return !_noThrowOnCancel;
			}
			void set(bool value)
			{
				_noThrowOnCancel = !value;
			}
		}

		/// <summary>
		/// Gets the last exception thrown by a Subversion api command to which this argument was provided
		/// </summary>
		property SvnException^ LastException
		{
			SvnException^ get()
			{
				return _exception;
			}

		internal:
			void set(SvnException^ value)
			{
				_exception = value;
			}
		}

		property bool IsLastInvocationCanceled
		{
			bool get();
		}

	internal:
		bool HandleResult(SvnClientContext^ client, svn_error_t *error);
		bool HandleResult(SvnClientContext^ client, SvnException^ error);
		void Prepare();
	};

	/// <summary>Base class of all <see cref="SvnClient" /> arguments which allow committing</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientArgsWithCommit abstract : public SvnClientArgs
	{
		String^ _logMessage;
		SvnRevisionPropertyCollection^ _extraProperties;

	protected:
		SvnClientArgsWithCommit()
		{
		}

	public:
		/// <summary>Raised just before committing to allow modifying the log message</summary>
		event EventHandler<SvnCommittingEventArgs^>^ Committing;

	protected public:
		/// <summary>Applies the <see cref="LogMessage" /> and raises the <see cref="Committing" /> event</summary>
		virtual void OnCommitting(SvnCommittingEventArgs^ e)
		{
			if (LogMessage && !e->LogMessage)
				e->LogMessage = LogMessage;

			Committing(this, e);
		}

	public:
		/// <summary>Gets or sets the logmessage applied to the commit</summary>
		property String^ LogMessage
		{
			String^ get()
			{
				return _logMessage;
			}

			void set(String^ value)
			{
				_logMessage = value;
			}
		}

	public:
		/// <summary>Gets a list of 'extra' revision properties to set on commit</summary>
		property SvnRevisionPropertyCollection^ LogProperties
		{
			[DebuggerStepThrough]
			SvnRevisionPropertyCollection^ get()
			{
				if (!_extraProperties)
					_extraProperties = gcnew SvnRevisionPropertyCollection();

				return _extraProperties;
			}
		}
	};

	/// <summary>Base class of all <see cref="SvnClient" /> arguments which allow handling conflicts</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientArgsWithConflict abstract : public SvnClientArgs
	{
	public:
		/// <summary>
		/// Raised on conflict. The event is first
		/// raised on the <see cref="SvnClientArgsWithConflict" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnConflictEventArgs^>^ Conflict;

		/// <summary>
		/// Raised before automatically merging changes into a file. The event is
		/// first raised on the <see cref="SvnClientArgsWithConflict" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnBeforeAutomaticMergeEventArgs^>^ BeforeAutomaticMerge;

	protected:
		SvnClientArgsWithConflict()
		{
		}

	protected:
		/// <summary>Raises the <see cref="Conflict" /> event</summary>
		virtual void OnConflict(SvnConflictEventArgs^ e)
		{
			Conflict(this, e);
		}

		/// <summary>Raises the <see cref="BeforeAutomaticMerge" /> event</summary>
		virtual void OnBeforeAutomaticMerge(SvnBeforeAutomaticMergeEventArgs^ e)
		{
			BeforeAutomaticMerge(this, e);
		}

	internal:
		void RaiseOnConflict(SvnConflictEventArgs^ e)
		{
			OnConflict(e);
		}

		void RaiseOnBeforeAutomaticMerge(SvnBeforeAutomaticMergeEventArgs^ e)
		{
			OnBeforeAutomaticMerge(e);
		}
	};

	public ref class SvnMergeBaseArgs abstract : public SvnClientArgsWithConflict
	{
		SvnCommandLineArgumentCollection^ _mergeArguments;
	protected:
		SvnMergeBaseArgs()
		{
		}

	public:
		property SvnCommandLineArgumentCollection^ MergeArguments
		{
			SvnCommandLineArgumentCollection^ get()
			{
				if (!_mergeArguments)
					_mergeArguments = gcnew SvnCommandLineArgumentCollection();

				return _mergeArguments;
			}
		}
	};
}

#include "SvnMergeInfoArgs.h"
