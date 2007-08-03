#pragma once

namespace SharpSvn {

	public enum class SvnDepth
	{
		/// <summary>Depth undetermined or ignored</summary>
		Unknown			= svn_depth_unknown,

		/// <summary>Exclude (remove, whatever) directory D</summary>
		Exclude			= svn_depth_exclude,

		/// <summary> 
		/// Just the named directory D, no entries. Updates will not pull in any 
		/// files or subdirectories not already present
		/// </summary>
		Empty			= svn_depth_empty,

		/// <summary>
		/// D + its file children, but not subdirs.  Updates will pull in any files
		/// not already present, but not subdirectories.
		/// </summary>
		Files			= svn_depth_files,

		/// <summary>
		/// D + immediate children (D and its entries). Updates will pull in any 
		/// files or subdirectories not already present; those subdirectories' 
		/// this_dir entries will have depth-empty. */
		/// </summary>
		Immediates		= svn_depth_immediates,

		/// <summary>
		/// D + all descendants (full recursion from D). Updates will pull in any 
		/// files or subdirectories not already present; those subdirectories' 
		/// this_dir entries will have depth-infinity. Equivalent to the pre-1.5 
		/// default update behavior. */
		/// </summary>
		Infinity		= svn_depth_infinity,
	};

	public enum class SvnConflictResult
	{
		/// <summary>user did nothing; conflict remains</summary>
		Conflicted		= svn_wc_conflict_result_conflicted,
		/// <summary>user has resolved the conflict</summary>
		Resolved		= svn_wc_conflict_result_resolved,

		/// <summary>user chooses the base file; The caller of the conflict-callback is responsible for "installing" the chosen file as the final version of the file</summary>
		Base			= svn_wc_conflict_result_choose_base,
		/// <summary>user chooses the repository file; The caller of the conflict-callback is responsible for "installing" the chosen file as the final version of the file</summary>
		Repository		= svn_wc_conflict_result_choose_repos,
		/// <summary>user chooses own version of file; The caller of the conflict-callback is responsible for "installing" the chosen file as the final version of the file</summary>
		User			= svn_wc_conflict_result_choose_user,
		/// <summary>user chooses the merged-file; The caller of the conflict-callback is responsible for "installing" the chosen file as the final version of the file</summary>
		Merged			= svn_wc_conflict_result_choose_merged,
	};

	public enum class SvnAccept
	{
		Invalid			= svn_accept_invalid,

		/// <summary>Resolve the conflict as usual</summary>
		Default			= svn_accept_default,

		/// <summary>Resolve the conflict with the pre-conflict base file</summary>
		Left			= svn_accept_left,

		/// <summary>Resolve the conflict with the pre-conflict working copy file</summary>
		Working			= svn_accept_working,

		/// <summary>Resolve the conflict with the post-conflict base file</summary>
		Right			= svn_accept_right,
	};

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

	public enum class SvnChangeAction
	{
		None = 0,
		Add = 'A',
		Delete = 'D', 
		Replace = 'R',
		Modify = 'M'
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

	public enum class SvnWcSchedule
	{
		/// <summary>Zero value; probably same as normal</summary>
		None = 0,
		/// <summary> Nothing special here </summary>
		Normal			= svn_wc_schedule_normal,
		/// <summary> Slated for addition </summary>
		Add				= svn_wc_schedule_add,

		/// <summary> Slated for deletion </summary>
		Delete			= svn_wc_schedule_delete,

		/// <summary> Slated for replacement (delete + add) </summary>
		Replace			= svn_wc_schedule_replace
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

	[Flags]
	public enum class SvnDirEntryItems
	{
		None			= 0,

		/** An indication that you are interested in the @c kind field */
		Kind			= SVN_DIRENT_KIND,

		/** An indication that you are interested in the @c size field */
		Size			= SVN_DIRENT_SIZE,

		/** An indication that you are interested in the @c has_props field */
		HasProperties	= SVN_DIRENT_HAS_PROPS,

		/** An indication that you are interested in the @c created_rev field */
		CreatedRevision	= SVN_DIRENT_CREATED_REV,

		/** An indication that you are interested in the @c time field */
		Time			= SVN_DIRENT_TIME,

		/** An indication that you are interested in the @c last_author field */
		LastAuthor		= SVN_DIRENT_LAST_AUTHOR,

		/** A combination of all the dirent fields */
		AllFields		= SVN_DIRENT_ALL
	};

	public enum class SvnRepositoryFilesystem
	{
		Default			= 0,
		FsFs			= 1,
		BerkeleyDb		= 2
	};

	public enum class SvnRepositoryCompatibility
	{
		Default				= 0,
		SubversionPre1_4	= 1,
		SubversionPre1_5	= 2,
	};
}