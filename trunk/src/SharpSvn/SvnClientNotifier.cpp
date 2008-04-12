// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "SvnClientNotifier.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

void SvnClientReporter::OnProcessing(SvnProcessingEventArgs^ e)
{
	// Reset parameters
	_receivedSomeChange = false;
	_commandType = e->CommandType;
	_suppressFinalLine = false;
	_sentFirstTxdelta = false;
	_inExternal = 0;
}

// Code is +- equivalent to those in notify.c of subversions svn project
void SvnClientReporter::OnNotify(SvnNotifyEventArgs^ e)
{
	if (!e)
		throw gcnew ArgumentNullException("e");

	switch(e->Action)
	{
	case SvnNotifyAction::Skip:
		if (e->ContentState == SvnNotifyState::Missing)
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifySkippedMissingTargetX, e->Path));
		else
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifySkippedMissingX, e->Path));
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
		if (e->ContentState == SvnNotifyState::Conflicted)
			WriteLine("C    " + e->Path);
		else
			WriteLine("A    " + e->Path);
		break;
	case SvnNotifyAction::Exists:
		_receivedSomeChange = true;
		if (e->ContentState == SvnNotifyState::Conflicted)
			Write("C");
		else
			Write("E");

		if (e->PropertyState == SvnNotifyState::Conflicted)
			Write("C");
		else
			Write("G");

		WriteLine("   " + e->Path);
		break;
	case SvnNotifyAction::Restore:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyRestoredX, e->Path));
		break;
	case SvnNotifyAction::Revert:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyRevertedX, e->Path));
		break;
	case SvnNotifyAction::RevertFailed:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyFailedToRevertX, e->Path));
		break;
	case SvnNotifyAction::Resolved:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyResolvedConflictedStateOfX, e->Path));
		break;
	case SvnNotifyAction::Add:
		if (e->MimeType && e->MimeTypeIsBinary)
			WriteLine("A  (bin)  " + e->Path);
		else
			WriteLine("A         " + e->Path);
		break;
	case SvnNotifyAction::Delete:
		_receivedSomeChange = true;
		WriteLine("D         " + e->Path);
		break;
	case SvnNotifyAction::UpdateUpdate:
		if (!(e->NodeKind == SvnNodeKind::Directory &&
			(  (e->PropertyState == SvnNotifyState::None)
			|| (e->PropertyState == SvnNotifyState::Unknown)
			|| (e->PropertyState == SvnNotifyState::Unchanged))))
		{
			if (e->NodeKind == SvnNodeKind::File)
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
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyFetchExternalItemIntoX, e->Path));
		break;
	case SvnNotifyAction::UpdateCompleted:
		if (!_suppressFinalLine)
		{
			if (e->Revision >= 0)
			{
				if (CurrentCommandType == SvnCommandType::Export)
				{
					if (_inExternal)
						WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyExportedExternalAtRevisionX, e->Revision));
					else
						WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyExportedRevisionX, e->Revision));
				}
				else if(CurrentCommandType == SvnCommandType::CheckOut)
				{
					if (_inExternal)
						WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyCheckedOutExternalAtRevisionX, e->Revision));
					else
						WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyCheckedOutRevisionX, e->Revision));
				}
				else
				{
					if (_receivedSomeChange)
					{
						if (_inExternal)
							WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyUpdatedExternalToRevisionX, e->Revision));
						else
							WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyUpdatedToRevisionX, e->Revision));
					}
					else
					{
						if (_inExternal)
							WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyExternalAtRevisionX, e->Revision));
						else
							WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyAtRevisionX, e->Revision));
					}
				}
			}

			if (_inExternal > 0)
			{
				_inExternal--;
				WriteLine("");
			}
		}
		break;

	case SvnNotifyAction::StatusExternal:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyPerformingStatusOnExternalItemAtX, e->Path));
		break;
	case SvnNotifyAction::StatusCompleted:
		if (e->Revision >= 0)
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyStatusAgainstRevisionX, e->Revision)); // Svn: %6ld
		break;

	case SvnNotifyAction::CommitModified:
		// svn: Align the %s's on this and the following 4 messages
		WriteLine("Sending        " + e->Path);
		break;
	case SvnNotifyAction::CommitAdded:
		if (e->MimeType && e->MimeTypeIsBinary)
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
		if (!_sentFirstTxdelta)
		{
			_sentFirstTxdelta = true;
			Write(SharpSvnStrings::NotifyTransmittingFileData + " ");
		}

		WriteProgress(".");
		break;

	case SvnNotifyAction::LockLocked:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyXLockedByUserY, e->Lock->Owner, e->Path));
		break;
	case SvnNotifyAction::LockUnlocked:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyXUnlocked, e->Path));
		break;
	case SvnNotifyAction::LockFailedLock:
	case SvnNotifyAction::LockFailedUnlock:
		WriteLine(e->Error->Message);
		break;

	case SvnNotifyAction::ChangeListSet:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyPathXIsNowAMemberOfChangeListY, e->Path, e->ChangeListName));
		break;
	case SvnNotifyAction::ChangeListClear:
		WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyPathXIsNoLongerAMemberOfAChangeList, e->Path));
		break;
	case SvnNotifyAction::ChangeListMoved:
		WriteLine(e->Error->Message);
		break;
	case SvnNotifyAction::MergeBegin:
		if (!e->MergeRange)
		{
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyMergingDifferencesBetweenRepositoryUrlsIntoX,
				e->Path));
		}
		else if((e->MergeRange->Start == e->MergeRange->End-1)
			|| (e->MergeRange->Start == e->MergeRange->End))
		{
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyMergingRXIntoY,
				e->MergeRange->End, e->Path));
		}
		else if(e->MergeRange->End == e->MergeRange->Start-1)
		{
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyReverseMergingRXIntoY,
				e->MergeRange->Start, e->Path));
		}
		else if(e->MergeRange->Start < e->MergeRange->End)
		{
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyMergingRXToRYIntoZ,
				e->MergeRange->Start+1, e->MergeRange->End, e->Path)); // Note: Start+1
		}
		else /*if(e->MergeRange->Start > e->MergeRange->End - 1)*/
		{
			WriteLine(String::Format(_fp,  SharpSvnStrings::NotifyReverseMergingRXToRYIntoZ,
				e->MergeRange->Start, e->MergeRange->End+1, e->Path)); // Note: End+1
		}
		break;
	default:
		break;
	}
}