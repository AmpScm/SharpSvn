#pragma once

namespace TurtleSvn {

	public enum class SvnRevisionType
	{
		None		= svn_opt_revision_unspecified, 
		Number		= svn_opt_revision_number,
		Date		= svn_opt_revision_date,
		Committed	= svn_opt_revision_committed,
		Previous	= svn_opt_revision_previous,
		Base		= svn_opt_revision_base,
		Working		= svn_opt_revision_working,
		Head		= svn_opt_revision_head
	};

	public enum class SvnNotifyAction
	{
		Add				= svn_wc_notify_add, 
		Copy			= svn_wc_notify_copy, 
		Delete			= svn_wc_notify_delete, 
		Restore			= svn_wc_notify_restore, 
		Revert			= svn_wc_notify_revert, 
		FailedRevert	= svn_wc_notify_failed_revert, 
		Resolved		= svn_wc_notify_resolved, 
		Skip			= svn_wc_notify_skip, 
		UpdateDelete	= svn_wc_notify_update_delete, 
		UpdateAdd		= svn_wc_notify_update_add, 
		UpdateData		= svn_wc_notify_update_update, 
		UpdateCompleted = svn_wc_notify_update_completed, 
		UpdateExternal  = svn_wc_notify_update_external, 
		StatusCompleted = svn_wc_notify_status_completed, 
		StatusExternal	= svn_wc_notify_status_external, 
		CommitModified	= svn_wc_notify_commit_modified, 
		CommitAdded		= svn_wc_notify_commit_added, 
		CommitDeleted	= svn_wc_notify_commit_deleted, 
		CommitReplaced	= svn_wc_notify_commit_replaced, 
		CommitBeforeDelta=svn_wc_notify_commit_postfix_txdelta, 
		BlameRevision	= svn_wc_notify_blame_revision, 
		Locked			= svn_wc_notify_locked, 
		LockUnlocked	= svn_wc_notify_unlocked, 
		LockFailed		= svn_wc_notify_failed_lock, 
		LockFailedUnlock=svn_wc_notify_failed_unlock 

		// TODO: Add new items for Subversion 1.5+
	};

	public enum class SvnNodeKind
	{
		None		= svn_node_none, 
		File		= svn_node_file, 
		Directory	= svn_node_dir, 
		Unknown		= svn_node_unknown
	};

	public enum class SvnNotifyState
	{
		None		= svn_wc_notify_state_inapplicable, 
		Unknown		= svn_wc_notify_state_unknown, 
		Unchanged	= svn_wc_notify_state_unchanged, 
		Missing		= svn_wc_notify_state_missing, 
		Obstructed	= svn_wc_notify_state_obstructed, 
		Changed		= svn_wc_notify_state_changed, 
		Merged		= svn_wc_notify_state_merged, 
		Conflicted	= svn_wc_notify_state_conflicted 
	};

	public enum class SvnLockState
	{
		Unchanged	= svn_wc_notify_lock_state_unchanged,
		Locked		= svn_wc_notify_lock_state_locked,
		Unlocked	= svn_wc_notify_lock_state_unlocked
	};

	[Flags]
	public enum class SvnCertificateTrustFailure
	{
		CertificateNotValidYet		=	SVN_AUTH_SSL_NOTYETVALID,
		CertificateExpired			=	SVN_AUTH_SSL_EXPIRED,
		CommonNameMismatch			=	SVN_AUTH_SSL_CNMISMATCH,
		UnknownCertificateAuthority =	SVN_AUTH_SSL_UNKNOWNCA,

		UnknownSslProviderFailure	=	SVN_AUTH_SSL_OTHER,

		MaskAllFailures				=	CertificateNotValidYet | CertificateExpired | CommonNameMismatch | UnknownCertificateAuthority | UnknownSslProviderFailure
	};
}