#pragma once

namespace SharpSvn {

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
		UpdateUpdate	= svn_wc_notify_update_update, 
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

	public enum class SvnWcStatus
	{
		/// <summary>does not exist</summary>
		None				= svn_wc_status_none,

		/// <summary>is not a versioned thing in this wc</summary>
		Unversioned			= svn_wc_status_unversioned,

		/// <summary>exists, but uninteresting</summary>
		Normal				= svn_wc_status_normal,

		/// <summary>is scheduled for addition</summary>
		Added				= svn_wc_status_added,

		/// <summary>under v.c., but is missing</summary>
		Missing				= svn_wc_status_missing,

		/// <summary>scheduled for deletion</summary>
		Deleted				= svn_wc_status_deleted,

		/// <summary>was deleted and then re-added</summary>
		Replaced			= svn_wc_status_replaced,

		/// <summary>text or props have been modified</summary>
		Modified			= svn_wc_status_modified,

		/// <summary>local mods received repos mods</summary>
		Merged				= svn_wc_status_merged,

		/// <summary>local mods received conflicting repos mods</summary>
		Conflicted			= svn_wc_status_conflicted,

		/// <summary>is unversioned but configured to be ignored</summary>
		Ignored				= svn_wc_status_ignored,

		/// <summary>an unversioned resource is in the way of the versioned resource</summary>
		Obstructed			= svn_wc_status_obstructed,

		/// <summary>an unversioned path populated by an svn:externals property</summary>
		External			= svn_wc_status_external,

		/// <summary>a directory doesn't contain a complete entries list</summary>
		Incomplete			= svn_wc_status_incomplete
	};

	[Flags]
	public enum class SvnCertificateTrustFailure
	{
		None						=	0,
		CertificateNotValidYet		=	SVN_AUTH_SSL_NOTYETVALID,
		CertificateExpired			=	SVN_AUTH_SSL_EXPIRED,
		CommonNameMismatch			=	SVN_AUTH_SSL_CNMISMATCH,
		UnknownCertificateAuthority =	SVN_AUTH_SSL_UNKNOWNCA,

		UnknownSslProviderFailure	=	SVN_AUTH_SSL_OTHER,

		MaskAllFailures				=	CertificateNotValidYet | CertificateExpired | CommonNameMismatch | UnknownCertificateAuthority | UnknownSslProviderFailure
	};
}