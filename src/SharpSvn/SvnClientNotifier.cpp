// $Id: SvnMergeInfo.cpp 171 2007-10-14 19:38:35Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "SvnClientNotifier.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;

void SvnClientNotifier::OnBeforeCommand(SvnBeforeCommandEventArgs^ e)
{
	UNUSED_ALWAYS(e);

	// Reset parameters
	_receivedSomeChange = false;
	_isCheckout = (nullptr != dynamic_cast<SvnCheckOutArgs^>(e->CommandArgs));
	_isExport = (nullptr != dynamic_cast<SvnExportArgs^>(e->CommandArgs));
	_suppressFinalLine = false;
	_sentFirstTxdelta = false;
	_inExternal = 0;
}

void SvnClientNotifier::OnNotify(SvnNotifyEventArgs^ e)
{
	if(!e)
		throw gcnew ArgumentNullException("e");

	switch(e->Action)
	{
	case SvnNotifyAction::Skip:
		if(e->ContentState == SvnNotifyState::Missing)
			WriteLine(String::Format(SharpSvnStrings::NotifySkippedMissingTargetX, e->Path));
		else
			WriteLine(String::Format(SharpSvnStrings::NotifySkippedMissingX, e->Path));
		break;

	case SvnNotifyAction::UpdateDelete:
		_receivedSomeChange = true;
		WriteLine("D    " + e->Path);
		break;
	case SvnNotifyAction::UpdateReplace:
		_receivedSomeChange = true;
		WriteLine("R    " + e->Path);
		break;
	case SvnNotifyAction::UpdateAdd:
		_receivedSomeChange = true;
		if(e->ContentState == SvnNotifyState::Conflicted)
			WriteLine("C    " + e->Path);
		else
			WriteLine("A    " + e->Path);
		break;
	case SvnNotifyAction::Exists:
		_receivedSomeChange = true;
		if(e->ContentState == SvnNotifyState::Conflicted)
			Write("C");			
		else
			Write("E");

		if(e->PropertyState == SvnNotifyState::Conflicted)
			Write("C");
		else
			Write("G");

		WriteLine("   " + e->Path);
		break;
	case SvnNotifyAction::Restore:
		WriteLine(String::Format(SharpSvnStrings::NotifyRestoredX, e->Path));
		break;
	case SvnNotifyAction::Revert:
		WriteLine(String::Format(SharpSvnStrings::NotifyRevertedX, e->Path));
		break;
	case SvnNotifyAction::RevertFailed:
		WriteLine(String::Format(SharpSvnStrings::NotifyFailedToRevertX, e->Path));
		break;
	case SvnNotifyAction::Resolved:
		WriteLine(String::Format(SharpSvnStrings::NotifyResolvedConflictedStateOfX, e->Path));
		break;
	case SvnNotifyAction::Add:
		if(e->MimeType && e->MimeTypeIsBinary)
			WriteLine("A  (bin)  " + e->Path);
		else
			WriteLine("A         " + e->Path);
		break;
	case SvnNotifyAction::Delete:
		_receivedSomeChange = true;
		WriteLine("D         " + e->Path);
		break;
	case SvnNotifyAction::UpdateUpdate:
		if(!(e->NodeKind == SvnNodeKind::Directory && 
			(  (e->PropertyState == SvnNotifyState::None)
			|| (e->PropertyState == SvnNotifyState::Unknown)
			|| (e->PropertyState == SvnNotifyState::Unchanged))))
		{
			if(e->NodeKind == SvnNodeKind::File)
			{
				switch(e->ContentState)
				{
				case SvnNotifyState::Conflicted:
					_receivedSomeChange = true;
					Write("C");
					break;
				case SvnNotifyState::Merged:
					_receivedSomeChange = true;
					Write("G");
					break;
				case SvnNotifyState::Changed:
					_receivedSomeChange = true;
					Write("U");
					break;
				default:
					Write(" ");
					break;
				}
			}
			else
				Write(" ");

			switch(e->PropertyState)
			{
			case SvnNotifyState::Conflicted:
				_receivedSomeChange = true;
				Write("C");
				break;
			case SvnNotifyState::Merged:
				_receivedSomeChange = true;
				Write("G");
				break;
			case SvnNotifyState::Changed:
				_receivedSomeChange = true;
				Write("U");
				break;
			default:
				Write(" ");
				break;
			}

			switch(e->LockState)
			{
			case SvnLockState::Unlocked:
				_receivedSomeChange = true;
				Write("B");
				break;
			default:
				Write(" ");
				break;
			}

			WriteLine(" " + e->Path);
		}
		break;
	case SvnNotifyAction::UpdateExternal:
		_inExternal++; // Remember that we're now "inside" an externals definition.

		WriteLine("");
		WriteLine(String::Format(SharpSvnStrings::NotifyFetchExternalItemIntoX, e->Path));
		break;
	case SvnNotifyAction::UpdateCompleted:
		if(!_suppressFinalLine)
		{
			if(e->Revision >= 0)
			{
				if(_isExport)
				{
					if(_inExternal)
						WriteLine(String::Format(SharpSvnStrings::NotifyExportedExternalAtRevisionX, e->Revision));
					else
						WriteLine(String::Format(SharpSvnStrings::NotifyExportedRevisionX, e->Revision));
				}
				else if(_isCheckout)
				{
					if(_inExternal)
						WriteLine(String::Format(SharpSvnStrings::NotifyCheckedOutExternalAtRevisionX, e->Revision));
					else
						WriteLine(String::Format(SharpSvnStrings::NotifyCheckedOutRevisionX, e->Revision));
				}
				else
				{
					if(_receivedSomeChange)
					{
						if(_inExternal)
							WriteLine(String::Format(SharpSvnStrings::NotifyUpdatedExternalToRevisionX, e->Revision));
						else
							WriteLine(String::Format(SharpSvnStrings::NotifyUpdatedToRevisionX, e->Revision));
					}
					else
					{
						if(_inExternal)
							WriteLine(String::Format(SharpSvnStrings::NotifyExternalAtRevisionX, e->Revision));
						else
							WriteLine(String::Format(SharpSvnStrings::NotifyAtRevisionX, e->Revision));
					}
				}
			}

			if(_inExternal > 0)
			{
				_inExternal--;
				WriteLine("");
			}
		}
		break;

	case SvnNotifyAction::StatusExternal:
		WriteLine(String::Format(SharpSvnStrings::NotifyPerformingStatusOnExternalItemAtX, e->Path));
		break;
	case SvnNotifyAction::StatusCompleted:
		if(e->Revision >= 0)
			WriteLine(String::Format(SharpSvnStrings::NotifyStatusAgainstRevisionX, e->Revision)); // Svn: %6ld
		break;

	case SvnNotifyAction::CommitModified:
		// svn: Align the %s's on this and the following 4 messages
		WriteLine("Sending        " + e->Path);
		break;
	case SvnNotifyAction::CommitAdded:
		if(e->MimeType && e->MimeTypeIsBinary)
			WriteLine("Adding  (bin)  " + e->Path);
		else
			WriteLine("Adding         " + e->Path);
		break;
	case SvnNotifyAction::CommitDeleted:
		WriteLine("Deleting       " + e->Path);
		break;
	case SvnNotifyAction::CommitReplaced:
		WriteLine("Replacing      " + e->Path);
		break;
	case SvnNotifyAction::CommitSendData:
		if(!_sentFirstTxdelta)
		{
			_sentFirstTxdelta = true;
			Write(SharpSvnStrings::NotifyTransmittingFileData + " ");
		}

		WriteProgress(".");
		break;			
	}

#if 0
	// TODO: Implement rest of the notifies

	case svn_wc_notify_locked:
		if ((err = svn_cmdline_printf(pool, _("'%s' locked by user '%s'.\n"),
			path_local, n->lock->owner)))
			goto print_error;
		break;

	case svn_wc_notify_unlocked:
		if ((err = svn_cmdline_printf(pool, _("'%s' unlocked.\n"),
			path_local)))
			goto print_error;
		break;

	case svn_wc_notify_failed_lock:
	case svn_wc_notify_failed_unlock:
		svn_handle_warning(stderr, n->err);
		break;

	case svn_wc_notify_changelist_set:
		if ((err = svn_cmdline_printf(pool, _("Path '%s' is now a member of "
			"changelist '%s'.\n"),
			path_local, n->changelist_name)))
			goto print_error;
		break;

	case svn_wc_notify_changelist_clear:
		if ((err = svn_cmdline_printf(pool,
			_("Path '%s' is no longer a member of "
			"a changelist.\n"),
			path_local)))
			goto print_error;
		break;

	case svn_wc_notify_changelist_failed:
		svn_handle_warning(stderr, n->err);
		svn_error_clear(n->err);
		break;

	case svn_wc_notify_changelist_moved:
		svn_handle_warning(stderr, n->err);
		svn_error_clear(n->err);
		break;

	case svn_wc_notify_merge_begin:
		if (n->merge_range == NULL)
			err = svn_cmdline_printf(pool,
			_("--- Merging differences between "
			"repository URLs into '%s':\n"),
			path_local);
		else if (n->merge_range->start == n->merge_range->end - 1
			|| n->merge_range->start == n->merge_range->end)
			err = svn_cmdline_printf(pool, _("--- Merging r%ld into '%s':\n"),
			n->merge_range->end, path_local);
		else if (n->merge_range->start - 1 == n->merge_range->end)
			err = svn_cmdline_printf(pool,
			_("--- Reverse-merging r%ld into '%s':\n"),
			n->merge_range->start, path_local);
		else if (n->merge_range->start < n->merge_range->end)
			err = svn_cmdline_printf(pool,
			_("--- Merging r%ld through r%ld into "
			"'%s':\n"),
			n->merge_range->start + 1,
			n->merge_range->end, path_local);
		else /* n->merge_range->start > n->merge_range->end - 1 */
			err = svn_cmdline_printf(pool,
			_("--- Reverse-merging r%ld through r%ld "
			"into '%s':\n"),
			n->merge_range->start,
			n->merge_range->end + 1, path_local);
		if (err)
			goto print_error;
		break;

	default:
		break;
}
#endif
}