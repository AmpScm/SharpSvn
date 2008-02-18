// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnChangeList.h"
#include "SvnCommandLineArgumentCollection.h"

namespace SharpSvn {
	ref class SvnException;
	using System::Collections::ObjectModel::Collection;

	/// <summary>Base class of all <see cref="SvnClient" /> arguments</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientArgs abstract
	{
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

	public:
		virtual property SvnClientCommandType ClientCommandType
		{
			SvnClientCommandType get() abstract;
		}

		/// <summary>
		/// Gets or sets a boolean indicating whether the call must throw an error if an exception occurs.
		/// If an exception would occur, the method returns false and the <see cref="Exception" /> property
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
		/// Gets or sets a boolean indicating whether the call must throw an error if an exception occurs.
		/// If an exception would occur, the method returns false and the <see cref="Exception" /> property
		/// is set to the exception which would have been throw.
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
		property SvnException^ Exception
		{
			SvnException^ get()
			{
				return _exception;
			}

		protected public:
			void set(SvnException^ value)
			{
				_exception = value;
			}
		}

		property bool InvocationCanceled
		{
			bool get();
		}

	internal:
		bool HandleResult(SvnClientContext^ client, svn_error_t *error);
	};

	/// <summary>Base class of all <see cref="SvnClient" /> arguments which allow committing</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientArgsWithCommit abstract : public SvnClientArgs
	{
		String^ _logMessage;

	public:
		/// <summary>Raised just before committing to allow modifying the log message</summary>
		event EventHandler<SvnCommittingEventArgs^>^ Committing;

	protected:
		SvnClientArgsWithCommit()
		{
		}

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
	};

	/// <summary>Base class of all <see cref="SvnClient" /> arguments which allow handling conflicts</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientArgsWithConflict abstract : public SvnClientArgs
	{
		String^ _logMessage;

	public:
		/// <summary>
		/// Raised on conflict. The event is first
		/// raised on the <see cref="SvnClientArgsWithConflict" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnConflictEventArgs^>^ Conflict;

	protected:
		SvnClientArgsWithConflict()
		{
		}

	protected public:
		/// <summary>Invokes the <see cref="Conflict" /> event</summary>
		virtual void OnConflict(SvnConflictEventArgs^ e)
		{
			Conflict(this, e);
		}

	public:
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

/*
#include "Args/Add.h"
#include "Args/AddToChangeList.h"
#include "Args/Blame.h"
#include "Args/CheckOut.h"
#include "Args/CleanUp.h"
#include "Args/Commit.h"
#include "Args/Copy.h"
#include "Args/CreateDirectory.h"
#include "Args/Delete.h"
#include "Args/Diff.h"
#include "Args/DiffMerge.h"
#include "Args/DiffSummary.h"
#include "Args/Export.h"
#include "Args/GetAppliedMergeInfo.h"
#include "Args/GetAvailableMergeInfo.h"
#include "Args/GetProperty.h"
#include "Args/GetRevisionProperty.h"
#include "Args/GetSuggestedMergeSources.h"
#include "Args/GetWorkingCopyState.h"
#include "Args/Import.h"
#include "Args/Info.h"
#include "Args/List.h"
#include "Args/ListChangeList.h"
#include "Args/Lock.h"
#include "Args/Log.h"
#include "Args/Merge.h"
#include "Args/Move.h"
#include "Args/PropertyList.h"
#include "Args/ReintegrationMerge.h"
#include "Args/Relocate.h"
#include "Args/RemoveFromChangeList.h"
#include "Args/Resolved.h"
#include "Args/Revert.h"
#include "Args/RevisionPropertyList.h"
#include "Args/SetProperty.h"
#include "Args/SetRevisionProperty.h"
#include "Args/Status.h"
#include "Args/Switch.h"
#include "Args/Unlock.h"
#include "Args/Update.h"
#include "Args/Write.h"
*/