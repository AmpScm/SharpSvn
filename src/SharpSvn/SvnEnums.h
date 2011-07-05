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

namespace SharpSvn {
	using System::Globalization::CultureInfo;

	public enum class SvnCommandType
	{
		Unknown = 0,

		// Add new members at the end
		Add,
		AddToChangeList,
		Blame,
		CheckOut,
		CleanUp,
		Commit,
		Copy,
		CreateDirectory,
		Delete,
		Diff,
		DiffMerge,
		DiffSummary,
		Export,
		GetAppliedMergeInfo,
		GetProperty,
		GetRevisionProperty,
		GetSuggestedMergeSources,
		Import,
		Info,
		List,
		ListChangeList,
		Lock,
		Log,
		Merge,
		MergesEligible,
		MergesMerged,
		Move,
		PropertyList,
		ReintegrationMerge,
		Relocate,
		RemoveFromChangeList,
		Resolved,
		Revert,
		RevisionPropertyList,
		SetProperty,
		SetRevisionProperty,
		Status,
		Switch,
		Unlock,
		Update,
		Write,
		Upgrade,
		Patch,

		CropWorkingCopy = 0x501,

		// Wc library helper
		GetWorkingCopyInfo = 0x1001,
		GetWorkingCopyVersion,
		GetWorkingCopyEntries,


		// Custom commands
		FileVersions = 0x2001,
		ReplayRevision,
		WriteRelated
	};

	public enum class SvnDepth
	{
		/// <summary>Depth undetermined or ignored</summary>
		Unknown			= svn_depth_unknown,

		// Not supported in 1.5 client api
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
		Children		= svn_depth_immediates,

		/// <summary>
		/// D + all descendants (full recursion from D). Updates will pull in any
		/// files or subdirectories not already present; those subdirectories'
		/// this_dir entries will have depth-infinity. Equivalent to the pre-1.5
		/// default update behavior.
		/// </summary>
		Infinity		= svn_depth_infinity,
	};

	public enum class SvnRevisionType
	{
		None		= svn_opt_revision_unspecified,
		Number		= svn_opt_revision_number,
		Time		= svn_opt_revision_date,
		Committed	= svn_opt_revision_committed,
		Previous	= svn_opt_revision_previous,
		Base		= svn_opt_revision_base,
		Working		= svn_opt_revision_working,
		Head		= svn_opt_revision_head
	};

	public enum class SvnNotifyAction
	{
		Add							= svn_wc_notify_add, // =0
		Copy						= svn_wc_notify_copy,
		Delete						= svn_wc_notify_delete,
		Restore						= svn_wc_notify_restore,
		Revert						= svn_wc_notify_revert,
		RevertFailed				= svn_wc_notify_failed_revert,
		Resolved					= svn_wc_notify_resolved,
		Skip						= svn_wc_notify_skip,
		UpdateDelete				= svn_wc_notify_update_delete,
		UpdateAdd					= svn_wc_notify_update_add,
		UpdateUpdate				= svn_wc_notify_update_update,
		UpdateCompleted 			= svn_wc_notify_update_completed,
		UpdateExternal  			= svn_wc_notify_update_external,
		StatusCompleted 			= svn_wc_notify_status_completed,
		StatusExternal				= svn_wc_notify_status_external,
		CommitModified				= svn_wc_notify_commit_modified,
		CommitAdded					= svn_wc_notify_commit_added,
		CommitDeleted				= svn_wc_notify_commit_deleted,
		CommitReplaced				= svn_wc_notify_commit_replaced,
		CommitSendData				= svn_wc_notify_commit_postfix_txdelta,
		BlameRevision				= svn_wc_notify_blame_revision,
		// 1.2+
		LockLocked					= svn_wc_notify_locked,
		LockUnlocked				= svn_wc_notify_unlocked,
		LockFailedLock				= svn_wc_notify_failed_lock,
		LockFailedUnlock			= svn_wc_notify_failed_unlock,

		// 1.5+
		Exists						= svn_wc_notify_exists,
		ChangeListSet				= svn_wc_notify_changelist_set,
		ChangeListClear 			= svn_wc_notify_changelist_clear,
		ChangeListMoved				= svn_wc_notify_changelist_moved,
		MergeBegin					= svn_wc_notify_merge_begin,
		MergeBeginForeign			= svn_wc_notify_foreign_merge_begin,
		UpdateReplace				= svn_wc_notify_update_replace,

		// 1.6+
		PropertyAdded				= svn_wc_notify_property_added,
		PropertyModified			= svn_wc_notify_property_modified,
		PropertyDeleted				= svn_wc_notify_property_deleted,
		PropertyDeletedNonExistent	= svn_wc_notify_property_deleted_nonexistent,
		RevisionPropertySet			= svn_wc_notify_revprop_set,
		RevisionPropertyDeleted		= svn_wc_notify_revprop_deleted,
		MergeCompleted				= svn_wc_notify_merge_completed,
		TreeConflict				= svn_wc_notify_tree_conflict,
		ExternalFailed				= svn_wc_notify_failed_external,

		// 1.7+
		/// <summary>Starting an update operation.</summary>
		UpdateStarted               = svn_wc_notify_update_started,

		/// <summary>An update tried to add a file or directory at a path where a separate working copy was found</summary>
		UpdateSkipObstruction       = svn_wc_notify_update_skip_obstruction,

		/// <summary>An explicit update tried to update a file or directory that doesn't live in the repository and can't be brought in.</summary>
		UpdateSkipWorkingOnly       = svn_wc_notify_update_skip_working_only,

		/// <summary>An update tried to update a file or directory to which access could not be obtained.</summary>
		UpdateSkipAccessDenied      = svn_wc_notify_update_skip_access_denied,

		/// <summary>An update operation removed an external working copy.</summary>
		UpdateExternalRemoved       =  svn_wc_notify_update_external_removed,

		/// <summary>A node below an existing node was added during update.</summary>
		UpdateShadowedAdd           = svn_wc_notify_update_shadowed_add,

		/// <summary>A node below an exising node was updated during update.</summary>
		UpdateShadowedUpdate        = svn_wc_notify_update_shadowed_update,

		/// <summary>A node below an existing node was deleted during update.</summary>
		UpdateShadowedDelete        = svn_wc_notify_update_shadowed_delete,

		/// <summary>The mergeinfo on path was updated.</summary>
		RecordMergeInfo             = svn_wc_notify_merge_record_info,

		/// <summary>An working copy directory was upgraded to the latest format.</summary>
		UpgradedDirectory           = svn_wc_notify_upgraded_path,

		/// <summary>Mergeinfo describing a merge was recorded.</summary>
		RecordMergeInfoStarted      = svn_wc_notify_merge_record_info_begin,

		/// <summary>Mergeinfo was removed due to elision.</summary>
		RecordMergeInfoElided       = svn_wc_notify_merge_elide_info,

		/// <summary>A file in the working copy was patched.</summary>
		PatchApplied                = svn_wc_notify_patch,

		/// <summary>A hunk from a patch was applied.</summary>
		PatchAppliedHunk            = svn_wc_notify_patch_applied_hunk,

		/// <summary>A hunk from a patch was rejected.</summary>
		PatchRejectedHunk           = svn_wc_notify_patch_rejected_hunk,

		/// <summary>A hunk from a patch was found to already be applied.</summary>
		PatchFoundAlreadyApplied    = svn_wc_notify_patch_hunk_already_applied,

		/// <summary>Committing a non-overwriting copy (path is the target of the
		/// copy, not the source).</summary>
		CommitAddCopy               = svn_wc_notify_commit_copied,

		/// <summary>Committing an overwriting (replace) copy (path is the target of
		/// the copy, not the source).</summary>
		CommitReplacedWithCopy      = svn_wc_notify_commit_copied_replaced,

		/// <summary>The server has instructed the client to follow a URL redirection.</summary>
		FollowUrlRedirect           = svn_wc_notify_url_redirect,

		/// <summary>The operation was attempted on a path which doesn't exist.</summary>
		NonExistentPath             = svn_wc_notify_path_nonexistent,

		/// <summary>Removing a path by excluding it.</summary>
		Excluded                    = svn_wc_notify_exclude,

		/// <summary>Operation failed because the node remains in conflict</summary>
		FailedConflict              = svn_wc_notify_failed_conflict,

		/// <summary>Operation failed because an added node is missing</summary>
		FailedMissing               =  svn_wc_notify_failed_missing,

		/// <summary>Operation failed because a node is out of date</summary>
		FailedOutOfDate             = svn_wc_notify_failed_out_of_date,

		/// <summary>Operation failed because an added parent is not selected</summary>
		FailedNoParent              = svn_wc_notify_failed_no_parent,

		/// <summary>Operation failed because a node is locked by another user and/or working copy</summary>
		FailedLocked                = svn_wc_notify_failed_locked,

		/// <summary>Operation failed because the operation was forbidden by the server</summary>
		FailedForbiddenByServer     = svn_wc_notify_failed_forbidden_by_server,
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
		NotVersioned		= svn_wc_status_unversioned,

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
	public enum class SvnDirEntryItems
	{
		None			= 0,

		/// <summary>An indication that you are interested in the Kind field</summary>
		Kind			= SVN_DIRENT_KIND,

		/// <summary>An indication that you are interested in the @c size field</summary>
		Size			= SVN_DIRENT_SIZE,

		/// <summary>An indication that you are interested in the @c has_props field</summary>
		HasProperties	= SVN_DIRENT_HAS_PROPS,

		/// <summary>An indication that you are interested in the @c created_rev field</summary>
		Revision	= SVN_DIRENT_CREATED_REV,

		/// <summary>An indication that you are interested in the @c time field</summary>
		Time			= SVN_DIRENT_TIME,

		/// <summary>An indication that you are interested in the @c last_author field</summary>
		LastAuthor		= SVN_DIRENT_LAST_AUTHOR,

		/// <summary>A combination of all the dirent fields at Subversion 1.5</summary>
		AllFieldsV15	= Kind | Size | HasProperties | Revision | Time | LastAuthor,

		/// <summary>The fields loaded if no other value is specified</summary>
		SvnListDefault	= Kind | Time
	};

	public enum class SvnRepositoryFileSystem
	{
		Default			= 0,
		FsFs			= 1,
		BerkeleyDB		= 2
	};

	public enum class SvnRepositoryCompatibility
	{
		/// <summary>Create a repository with the latest available format</summary>
		Default				= 0,
		/// <summary>Create a repository in a Subversion 1.0-1.3 compatible format</summary>
		Subversion10,
		/// <summary>Create a repository in Subversion 1.4 compatible format</summary>
		Subversion14,
		/// <summary>Create a repository in Subversion 1.5 compatible format</summary>
		Subversion15,
		/// <summary>Create a repository in Subversion 1.6 compatible format</summary>
		Subversion16,
		/// <summary>Create a repository in Subversion 1.7 compatible format</summary>
		Subversion17,
	};

	public enum class SvnDiffKind
	{
		/// <summary>An item with no text modifications</summary>
		Normal = svn_client_diff_summarize_kind_normal,

		/// <summary>An added item</summary>
		Added = svn_client_diff_summarize_kind_added,

		/// <summary>An item with text modifications</summary>
		Modified = svn_client_diff_summarize_kind_modified,

		/// <summary>A deleted item</summary>
		Deleted = svn_client_diff_summarize_kind_deleted
	};

	public enum class SvnLineStyle
	{
		Default=0,
		CarriageReturnLinefeed,
		Linefeed,
		CarriageReturn,
		Native,
		Windows = CarriageReturnLinefeed,
		Unix = Linefeed
	};

	public enum class SvnAccept
	{
		/// <summary>Don't resolve the conflict now.  Let subversion mark the path
		/// 'conflicted', so user can run 'svn resolved' later</summary>
		Postpone		= svn_wc_conflict_choose_postpone,

		/// <summary>Choose the base file</summary>
		Base			= svn_wc_conflict_choose_base,

		/// <summary>Choose the incoming file</summary>
		TheirsFull		= svn_wc_conflict_choose_theirs_full,

		/// <summary>Choose the local file</summary>
		MineFull		= svn_wc_conflict_choose_mine_full,


		/// <summary>Choose their on all conflicts; further use auto-merged</summary>
		Theirs			= svn_wc_conflict_choose_theirs_conflict,
		/// <summary>Choose local version on all conflicts; further use auto-merged</summary>
		Mine			= svn_wc_conflict_choose_mine_conflict,
		/// <summary>Choose the 'merged file'. The result file of the automatic merging; possibly with local edits</summary>
		Merged			= svn_wc_conflict_choose_merged,

		/// <summary>Aliad for Merged</summary>
		Working			= Merged,
	};

	public enum class SvnConflictType
	{
		/// <summary>textual conflict (on a file)</summary>
		Content			= svn_wc_conflict_kind_text,
		/// <summary>property conflict (on a file or dir)</summary>
		Property		= svn_wc_conflict_kind_property,
		/// <summary>tree conflict (on a dir)</summary>
		Tree    = svn_wc_conflict_kind_tree,

		/// <summary>Deprecated: Use .Tree</summary>
		TreeConflict = Tree,
	};

	public enum class SvnConflictReason
	{
		/// <summary>local edits are already present</summary>
		Edited	= svn_wc_conflict_reason_edited,
		/// <summary>another object is in the way</summary>
		Obstructed = svn_wc_conflict_reason_obstructed,
		/// <summary>object is already schedule-delete</summary>
		Deleted = svn_wc_conflict_reason_deleted,
		/// <summary>object is already added or schedule-add</summary>
		Added = svn_wc_conflict_reason_added,
		/// <summary>object is unknown or missing</summary>
		Missing = svn_wc_conflict_reason_missing,
		/// <summary>object is unversioned</summary>
		NotVersioned = svn_wc_conflict_reason_unversioned
	};

	public enum class SvnConflictAction
	{
		/// <summary>Attempting to change text or props</summary>
		Edit = svn_wc_conflict_action_edit,
		/// <summary>Attempting to add object</summary>
		Add = svn_wc_conflict_action_add,
		/// <summary>Attempting to delete object</summary>
		Delete = svn_wc_conflict_action_delete
	};

	public enum class SvnOperation
	{
		None = svn_wc_operation_none,
		Update = svn_wc_operation_update,
		Switch = svn_wc_operation_switch,
		Merge = svn_wc_operation_merge
	};

	public enum class SvnIgnoreSpacing
	{
		/// <summary>Ignore no whitespace</summary>
		None		= svn_diff_file_ignore_space_none,
		/// <summary>Ignore changes in sequences of whitespace characters, treating each
		/// sequence of whitespace characters as a single space</summary>
		IgnoreSpace = svn_diff_file_ignore_space_change,
		/// <summary>Ignore all whitespace characters</summary>
		IgnoreAll	= svn_diff_file_ignore_space_all
	};

	public enum class SvnCapability
	{
		None=0,
		Depth,
		MergeInfo,
		LogRevisionProperties,
		PartialReplay,
		CommitRevisionProperties
	};

	public enum class SvnPropertyKind
	{
		Entry=0,
		SvnInternal,
		Regular
	};

	namespace Implementation
	{

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
				if (!Enum::IsDefined(T::typeid, value))
					throw gcnew ArgumentOutOfRangeException("value", value, String::Format(CultureInfo::InvariantCulture, SharpSvnStrings::VerifyEnumFailed, value, T::typeid->FullName));

				return value;
			}
		};

	}
}
