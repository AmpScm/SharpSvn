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

#include "SvnChangeList.h"
#include "SvnCommandLineArgumentCollection.h"

namespace SharpSvn {
	ref class SvnException;
	using System::Collections::ObjectModel::Collection;

	public ref class SvnClientContextArgs abstract
	{
		static SvnClientContextArgs()
		{
			SvnBase::EnsureLoaded();
		}

	private protected:
		SvnClientContextArgs()
		{
		}
	};

	/// <summary>Base class of all <see cref="SvnClient" /> arguments</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientArgs abstract : SvnClientContextArgs
	{
		static SvnClientArgs()
		{
			SvnBase::EnsureLoaded();
		}

		bool _noThrowOnError;
		bool _noThrowOnCancel;
		bool _throwOnWarning;
		SvnException^ _exception;

		// On +- 90% of the SvnClientArgs instances none of these is used
		array<SvnException^>^ _warnings;
		array<SvnErrorCode>^ _expectedErrors;
		array<SvnErrorCategory>^ _expectedErrorCategories;

	public:
		/// <summary>Raised to allow canceling an in-progress command</summary>
		DECLARE_EVENT(SvnCancelEventArgs^, Cancel)
		/// <summary>Raised to notify of progress by an in-progress command</summary>
		DECLARE_EVENT(SvnProgressEventArgs^, Progress);
		/// <summary>Raised to notify changes by an in-progress command</summary>
		DECLARE_EVENT(SvnNotifyEventArgs^, Notify);
		/// <summary>Raised to notify errors from an command</summary>
		DECLARE_EVENT(SvnErrorEventArgs^, SvnError);

	private protected:
		SvnClientArgs()
		{
		}

	protected:
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
		bool _hooked;
		virtual void RaiseOnNotify(SvnNotifyEventArgs^ e)
		{
			switch(e->Action)
			{
			case SvnNotifyAction::LockFailedLock:
			case SvnNotifyAction::LockFailedUnlock:
			case SvnNotifyAction::PropertyDeletedNonExistent:
			case SvnNotifyAction::ExternalFailed:
				if (e->Error)
				{
					_warnings = SvnBase::ExtendArray(_warnings, e->Error);
				}
				break;
			}
			OnNotify(e);
		}

		void RaiseOnCancel(SvnCancelEventArgs^ e)
		{
			OnCancel(e);
		}

		void RaiseOnProgress(SvnProgressEventArgs^ e)
		{
			OnProgress(e);
		}

		void RaiseOnSvnError(SvnErrorEventArgs^ e)
		{
			OnSvnError(e);
		}

	public:
		/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
		virtual property SvnCommandType CommandType
		{
			SvnCommandType get() abstract;
		}

		/// <summary>
		/// Gets or sets a boolean indicating whether the call must throw an error if an error occurs.
		/// If an exception would occur, the method returns false and the <see cref="LastException" /> property
		/// is set to the exception which would have been throw. Defaults to true.
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
		/// Gets or sets a boolean indicating whether the call must throw an error if a non fatal error occurs.
		/// (E.g. locking or updating an external failed). Defaults to false
		/// </summary>
		property bool ThrowOnWarning
		{
			bool get()
			{
				return _throwOnWarning;
			}
			void set(bool value)
			{
				_throwOnWarning = value;
			}
		}

		/// <summary>
		/// Gets or sets a boolean indicating whether the call must throw an error if the operation is cancelled
		/// <see cref="IsLastInvocationCanceled" /> is true and the returnvalue <c>false</c> if the operation was canceled.
		/// (The <see cref="LastException" /> property is set to the cancel exception). Defaults to true
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

		/// <summary>Gets a collection of warnings issued by the last command invocation</summary>
		property ICollection<SvnException^>^ Warnings
		{
			ICollection<SvnException^>^ get()
			{
				ReadOnlyCollection<SvnException^>^ rc = nullptr;

				if (_warnings && _warnings->Length)
				{
					IList<SvnException^>^ warnings = safe_cast<IList<SvnException^>^>(_warnings);
					rc = gcnew ReadOnlyCollection<SvnException^>(warnings);
				}

				return rc;
			}
		}

		property bool IsLastInvocationCanceled
		{
			bool get();
		}

		/// <summary>Adds an error code to the list of errors not to throw exceptions for</summary>
		void AddExpectedError(SvnErrorCode errorCode);
		/// <summary>Adds error codes to the list of errors not to throw exceptions for</summary>
		void AddExpectedError(... array<SvnErrorCode>^ errorCodes);
		/// <summary>Adds an error category to the list of errors not to throw exceptions for</summary>
		void AddExpectedError(SvnErrorCategory errorCategory);
		/// <summary>Adds error categories to the list of errors not to throw exceptions for</summary>
		void AddExpectedError(... array<SvnErrorCategory>^ errorCategories);

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
		DECLARE_EVENT(SvnCommittingEventArgs^, Committing)

	protected:
		/// <summary>Applies the <see cref="LogMessage" /> and raises the <see cref="Committing" /> event</summary>
		virtual void OnCommitting(SvnCommittingEventArgs^ e)
		{
			if (LogMessage && !e->LogMessage)
				e->LogMessage = LogMessage;

			Committing(this, e);
		}

	internal:
		void RaiseCommitting(SvnCommittingEventArgs^ e)
		{
			OnCommitting(e);
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
	protected:
		SvnClientArgsWithConflict()
		{
		}

	public:
		/// <summary>
		/// Raised on conflict. The event is first
		/// raised on the <see cref="SvnClientArgsWithConflict" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		DECLARE_EVENT(SvnConflictEventArgs^, Conflict)
	protected:
		/// <summary>Raises the <see cref="Conflict" /> event</summary>
		virtual void OnConflict(SvnConflictEventArgs^ e)
		{
			Conflict(this, e);
		}

	internal:
		void RaiseConflict(SvnConflictEventArgs^ e)
		{
			OnConflict(e);
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
