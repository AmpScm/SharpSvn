#pragma once

namespace SharpSvn {
	using System::Globalization::CultureInfo;

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
		/// this_dir entries will have depth-empty.
		/// </summary>
		Immediates		= svn_depth_immediates,

		/// <summary>
		/// D + all descendants (full recursion from D). Updates will pull in any
		/// files or subdirectories not already present; those subdirectories'
		/// this_dir entries will have depth-infinity. Equivalent to the pre-1.5
		/// default update behavior.
		/// </summary>
		Infinity		= svn_depth_infinity,
	};

	public enum class SvnConflictResult
	{
		/// <summary>user did nothing; conflict remains</summary>
		Conflicted		= svn_wc_conflict_result_conflicted,
		/// <summary>user has resolved the conflict</summary>
		Resolved		= svn_wc_conflict_result_resolved,

		/// <summary>user chooses the base file</summary>
		Base			= svn_wc_conflict_result_choose_base,
		/// <summary>user chooses their file</summary>
		Theirs			= svn_wc_conflict_result_choose_theirs,
		/// <summary>user chooses own version of file</summary>
		Mine			= svn_wc_conflict_result_choose_mine,
		/// <summary>user chooses the merged-file</summary>
		Merged			= svn_wc_conflict_result_choose_merged,
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
		Add				= svn_wc_notify_add, // =0
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
		// 1.2+
		Locked			= svn_wc_notify_locked,
		LockUnlocked	= svn_wc_notify_unlocked,
		LockFailed		= svn_wc_notify_failed_lock,
		LockFailedUnlock=svn_wc_notify_failed_unlock,

		// 1.5+
		Exists			= svn_wc_notify_exists,
		ChangeListSet	= svn_wc_notify_changelist_set,
		ChangeListClear = svn_wc_notify_changelist_clear,
		ChangeListFailed=svn_wc_notify_changelist_failed,
		MergeBegin		= svn_wc_notify_merge_begin,
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
		None		= svn_wc_notify_lock_state_inapplicable,
		Unknown		= svn_wc_notify_lock_state_unknown,
		Unchanged	= svn_wc_notify_lock_state_unchanged,
		Locked		= svn_wc_notify_lock_state_locked,
		Unlocked	= svn_wc_notify_lock_state_unlocked
	};

	public enum class SvnStatus
	{
		/// <summary>Zero value. Never used by Subversion</summary>
		Zero				= 0,

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

	public enum class SvnSchedule
	{
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
	public enum class SvnCertificateTrustFailures
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
		AllFields		= Kind | Size | HasProperties | CreatedRevision | Time | LastAuthor,
		//AllFields	was SVN_DIRENT_ALL, but that value does not allow future extension without side-effects

		SnvListDefault	= Kind | Time
	};

	public enum class SvnRepositoryFilesystem
	{
		Default			= 0,
		FsFs			= 1,
		BerkeleyDB		= 2
	};

	public enum class SvnRepositoryCompatibility
	{
		Default				= 0,
		SubversionPre14		= 1,
		SubversionPre15		= 2,
	};

	public enum class SvnDiffKind
	{
		None = 0,

		/** An item with no text modifications */
		Normal = svn_client_diff_summarize_kind_normal,

		/** An added item */
		Added = svn_client_diff_summarize_kind_added,

		/** An item with text modifications */
		Modified = svn_client_diff_summarize_kind_modified,

		/** A deleted item */
		Deleted = svn_client_diff_summarize_kind_deleted
	};

	public enum class SvnLineStyle
	{
		Native=0,
		CariageReturnLineFeed,
		LineFeed,
		CariageReturn,
		Windows = CariageReturnLineFeed,
		Unix = LineFeed
	};

	public enum class SvnConflictReason
	{
		None=0,
		/// <summary>local edits are already present</summary>
		Edited	= svn_wc_conflict_reason_edited,
		/// <summary>another object is in the way</summary>
		Obstructed = svn_wc_conflict_reason_obstructed,
		/// <summary>object is already schedule-delete</summary>
		Deleted = svn_wc_conflict_reason_deleted,
		/// <summary>object is unknown or missing</summary>
		Missing = svn_wc_conflict_reason_missing,
		/// <summary>object is unversioned</summary>
		Unversioned = svn_wc_conflict_reason_unversioned
	};

	public enum class SvnIgnoreSpacing
	{
		// Default the none value to zero

		None		= svn_diff_file_ignore_space_none   - svn_diff_file_ignore_space_none,
		IgnoreSpace = svn_diff_file_ignore_space_change - svn_diff_file_ignore_space_none,
		IgnoreAll	= svn_diff_file_ignore_space_all	- svn_diff_file_ignore_space_all
	};

	ref class EnumVerifier
	{
	private:
		EnumVerifier()
		{}

	public:
		generic<typename T>
		where T : System::Enum
		static T Verify(T value)
		{
			if(!Enum::IsDefined(T::typeid, value))
				throw gcnew ArgumentOutOfRangeException("value", value, String::Format(CultureInfo::InvariantCulture, SharpSvnStrings::VerifyEnumFailed, value, T::typeid->FullName));

			return value;
		}
	};
}