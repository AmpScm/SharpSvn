//  SubversionSharp, a wrapper library around the Subversion client API
#region Copyright (C) 2004 SOFTEC sa.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//  Sources, support options and lastest version of the complete library
//  is available from:
//		http://www.softec.st/SubversionSharp
//		Support@softec.st
//
//  Initial authors : 
//		Denis Gervalle
//		Olivier Desaive
#endregion
//
using System;
using System.Diagnostics;
using System.Collections;
using Softec.AprSharp;

namespace Softec.SubversionSharp
{
	public class SvnClient : SvnClientBase
	{
		AprPool mGlobalPool;
		AprPool mPool;
		SvnClientContext mContext;
		ArrayList mAuthObjs;
		
		#region ctors and finalize
		static SvnClient()
		{
			SvnConfig.Ensure();
		}
		
		public SvnClient()
		{
			mGlobalPool = Svn.PoolCreate();
			mPool = Svn.PoolCreate(mGlobalPool);
			mContext = SvnClientContext.Create(mGlobalPool);
			mContext.Config = SvnConfig.GetConfig(mGlobalPool);
			mAuthObjs = null;
		}
		
		public SvnClient(AprPool pool)
		{
			mGlobalPool = pool;
			mPool = Svn.PoolCreate(mGlobalPool);
			mContext = SvnClientContext.Create(mGlobalPool);
			mContext.Config = SvnConfig.GetConfig(mGlobalPool);
			mAuthObjs = null;
		}
		
		public SvnClient(SvnClientContext ctx, AprPool pool)
		{
			mGlobalPool = pool;
			mPool = Svn.PoolCreate(mGlobalPool);
			mContext = ctx;
			mAuthObjs = null;
		}
		#endregion
		
		#region Methods
		public void Clear()
		{
			mPool.Clear();
		}
		
		public void Reset()
		{
			mPool.Destroy();
			mPool = Svn.PoolCreate(mGlobalPool);
		}
		
		public IAprUnmanaged PathOrUrl(string str)
		{
			if(SvnPath.PathIsUrl(str))
			{
				return(new SvnUrl(str, mPool));
			}
			
			return(new SvnPath(str, mPool));
		}
		
		private Type PathOrUrlArrayType(ICollection coll)
		{
			IEnumerator it = coll.GetEnumerator();
			it.MoveNext();
			Type t = it.Current.GetType();
			
			if(t == typeof(SvnUrl) || t == typeof(SvnPath))
				return t;
			
			if(t == typeof(Uri))
				return typeof(SvnUrl);

			if (SvnPath.PathIsUrl(it.Current.ToString()))
			{
				return (typeof(SvnUrl));
			}
			return typeof(SvnPath);
		}
		#endregion
		
		#region Client methods

		/// <summary>
		/// Checkout a working copy of <paramref name="url"/> at <paramref name="revision"/>, looked up at 
		/// <paramref name="peg_revision"/>, using <paramref name="path"/> as the root directory of the newly
		/// checked out working copy.
		/// </summary>
		/// <param name="url">Repository URL</param>
		/// <param name="path">The root directory of the newly checked out working copy.</param>
		/// <param name="peg_revision">Peg revision</param>
		/// <param name="revision">Operative revision</param>
		/// <param name="recurse">If is true, check out recursively.  Otherwise, check out
		/// just the directory represented by <paramref name="url"/> and its immediate
		/// non-directory children, but none of its child directories (if any).</param>
		/// <param name="ignoreExternals">If is set, don't process externals definitions
		/// as part of this operation.</param>
		/// <returns>The value of the revision actually checked out from the repository.</returns>
		public int Checkout2(string url, string path, SvnRevision peg_revision, SvnRevision revision, bool recurse, bool ignoreExternals)
		{
			return Checkout2(new SvnUrl(url, mPool), new SvnPath(path, mPool), peg_revision.ToSvnOpt(mPool), revision.ToSvnOpt(mPool), recurse, ignoreExternals,
				mContext, mPool);
		}

		/// <summary>
		/// Update working trees <paramref name="paths"/> to <paramref name="revision"/>.
		/// </summary>
		/// <param name="paths">array of paths to be updated.</param>
		/// <param name="revision">Operative revision</param>
		/// <param name="recurse">If is true, update directories recursively; otherwise,
		/// update just their immediate entries, but not their child
		/// directories (if any).</param>
		/// <param name="ignoreExternals">Don't process externals definitions 
		/// as part of this operation</param>
		/// <returns>An array of revisions will be returned with each element set
		/// to the revision to which <paramref name="revision"/> was resolved.</returns>
		/// <remarks>Unversioned paths that are direct children 
		/// of a versioned path will cause an update that attempts to add that path,
		/// other unversioned paths are skipped.
		/// The paths in <paramref name="paths"/> can be from multiple working copies from multiple
		/// repositories, but even if they all come from the same repository there
		/// is no guarantee that <paramref name="revision"/> represented by Svn.Revision.Head
		/// will remain the same as each path is updated.
		/// </remarks>
		public int[] Update2(ICollection paths, SvnRevision revision, bool recurse, bool ignoreExternals)
		{
			AprArray arr = Update2(AprArray.LazyMake(mPool, paths, typeof(SvnPath)), revision.ToSvnOpt(mPool), recurse, ignoreExternals,
									  mContext, mPool);
			int[] revs = new int[arr.Count];
			arr.CopyTo(revs, 0);
			return revs;
		}

		/// <summary>
		/// Switch working tree <paramref name="path"/> to <paramref name="url"/> at <paramref name="revision"/>.
		/// </summary>
		/// <param name="path">Working tree path</param>
		/// <param name="url">Repository URL</param>
		/// <param name="revision">Operative revision</param>
		/// <param name="recurse">If is true, and <paramref name="path"/> is a directory, switch it
		/// recursively; otherwise, switch just <paramref name="path"/> and its immediate
		/// entries, but not its child directories (if any).</param>
		/// <returns>The value of the revision to which the working copy was actually switched.</returns>
		public int Switch(string path, string url, SvnRevision revision, bool recurse)
		{
			return Switch(new SvnPath(path, mPool), new SvnUrl(url, mPool), revision.ToSvnOpt(mPool), recurse,
						  mContext, mPool);
		}

		/// <summary>
		/// Schedule a working copy path for addition to the repository.
		/// </summary>
		/// <param name="path">Path to be added. Path's parent must be under revision 
		/// control already, but <paramref name="path"/> is not.  If <paramref name="recurse"/> is set, 
		/// then assuming <paramref name="path"/> is a directory, all of its contents will be scheduled 
		/// for addition as well.</param>
		/// <param name="recurse">If is set, then assuming <paramref name="path"/> is a 
		/// directory, all of its contents will be scheduled for addition as well.</param>
		/// <param name="force">If is set, do not error on already-versioned items.  
		/// When used on a directory in conjunction with the <paramref name="recurse"/> flag, this has the
		/// effect of scheduling for addition unversioned files and directories
		/// scattered deep within a versioned tree.</param>
		/// <param name="no_ignore">If is FALSE, don't add files or directories that match
		/// ignore patterns.</param>
		public void Add3(string path, bool recurse, bool force, bool no_ignore)
		{
			Add3(new SvnPath(path, mPool), recurse, force, no_ignore, mContext, mPool);
		}

		/// <summary>
		/// Create a directory, either in a repository or a working copy.
		/// </summary>
		/// <param name="paths">Paths or URLs to create</param>
		/// <returns>If commit was success, returns SvnCommitInfo with commit information.</returns>
		/// <remarks>If <paramref name="paths"/> contains URLs, attempt to commit the creation of the
		/// directories in <paramref name="paths"/> in the repository.  
		/// Else, create the directories on disk, and attempt to schedule them
		/// for addition.</remarks>
		public SvnCommitInfo Mkdir2(ICollection paths)
		{
			return Mkdir2(AprArray.LazyMake(mPool, paths, PathOrUrlArrayType(paths)), mContext, mPool);
		}

		/// <summary>
		/// Delete items from a repository or working copy.
		/// </summary>
		/// <param name="paths">Working copy paths or repository URLs to be deleted. 
		/// Every path must belong to the same repository.</param>
		/// <param name="force">If is not set then this operation will fail if any path 
		/// contains locally modified and/or unversioned items. If is set such items
		/// will be deleted.</param>
		/// <returns>If commit was success, returns SvnCommitInfo with commit information.</returns>
		/// <remarks>If the paths in <paramref name="paths"/> are URLs, immediately attempt to commit 
		/// a deletion of the URLs from the repository. If the commit succeeds, returns SvnCommitInfo 
		/// with commit information.  
		/// Else, schedule the working copy paths in <paramref name="paths"/> for removal from the repository.  
		/// Each path's parent must be under revision control. This is just a *scheduling* operation.  
		/// No changes will happen to the repository until a commit occurs. This scheduling can be
		/// removed with SvnClient.Revert. If a path is a file it is immediately removed from 
		/// the working copy. If the path is a directory it will remain in the working copy but 
		/// all the files, and all unversioned items, it contains will be removed.</remarks>
		public SvnCommitInfo Delete2(ICollection paths, bool force)
		{
			return Delete2(AprArray.LazyMake(mPool, paths, PathOrUrlArrayType(paths)), force, mContext, mPool);
		}

		/// <summary>
		/// Import file or directory <paramref name="path"/> into repository directory <paramref name="url"/> at head.
		/// </summary>
		/// <param name="path">Path to be imported</param>
		/// <param name="url">Repository URL</param>
		/// <param name="nonrecursive">Used to indicate that imported directories should not
		/// recurse into any subdirectories they may have.</param>
		/// <param name="no_ignore">If is FALSE, don't add files or directories that match
		/// ignore patterns.</param>
		/// <returns>Results of the commit</returns>
		/// <remarks> * If <paramref name="path"/> is a directory, the contents of that directory are
		/// imported directly into the directory identified by <paramref name="url"/>.  Note that the
		/// directory <paramref name="path"/> itself is not imported -- that is, the basename of 
		/// <paramref name="path"/> is not part of the import.
		/// If <paramref name="path"/> is a file, then the dirname of <paramref name="url"/> is the directory
		/// receiving the import.  The basename of <paramref name="url"/> is the filename in the
		/// repository.  In this case if <paramref name="url"/> already exists, exception thrown.</remarks>

		public SvnCommitInfo Import2(string path, string url, bool nonrecursive, bool no_ignore)
		{
			return Import2(new SvnPath(path, mPool), new SvnUrl(url, mPool), nonrecursive, no_ignore, mContext, mPool);
		}
		/// <summary>
		/// Commit files or directories into repository
		/// </summary>
		/// <param name="targets">Paths to commit. They need not be canonicalized 
		/// nor condensed; this function will take care of that
		/// If <paramref name="targets"/> has zero elements, then do nothing and return
		/// immediately without error</param>
		/// <param name="recurse">If is false, subdirectories of directories in 
		/// <paramref name="targets"/> will be ignored</param>
		/// <param name="keep_locks">If false, unlock paths in the repository</param>
		/// <returns>Results of the commit</returns>
		public SvnCommitInfo Commit3(ICollection targets, bool recurse, bool keep_locks)
		{
			return Commit3(AprArray.LazyMake(mPool, targets, typeof(SvnPath)), recurse, keep_locks,
						  mContext, mPool);
		}

		/// <summary>
		/// Retrieve status of <paramref name="path"/> and its children.
		/// </summary>
		/// <param name="path">Path to a working copy directory or single file</param>
		/// <param name="revision">Operative revision</param>
		/// <param name="statusFunc">Callback function</param>
		/// <param name="statusBaton">Callback function baton</param>
		/// <param name="recurse">If is true, recurse fully, else do only immediate children.</param>
		/// <param name="getAll">If is set, retrieve all entries; otherwise,retrieve only 
		/// "interesting" entries (local mods and/or out-of-date).</param>
		/// <param name="update">If is set, contact the repository and augment the
		/// status structures with information about out-of-dateness (with 
		/// respect to <paramref name="revision"/>).  Also, returns the actual revision against which the
		/// working copy was compared</param>
		/// <param name="noIgnore">If is false, don't retrieve files or directories that match
		/// ignore patterns.</param>
		/// <param name="ignoreExternals">If is not set, then recurse into externals
		/// definitions (if any exist) after handling the main target.</param>
		/// <returns>If <paramref name="update"/> is set, returns the actual revision against which the
		/// working copy was compared. Otherwise returns -1</returns>
		/// <remarks>
		/// When <paramref name="update"/> is set to true, add trailing slash to <paramref name="path"/>, 
		/// if path is directory. This is a workaround to Subversion bug 
		/// (http://subversion.tigris.org/issues/show_bug.cgi?id=2492). The path returned in 
		/// <paramref name="statusFunc"/> will contain double slashes. 
		/// You must remove this duplication yourself.
		/// </remarks>
		public int Status2(string path, SvnRevision revision,
						  SvnWcStatus2.Func statusFunc, IntPtr statusBaton,
						  bool recurse, bool getAll, bool update, bool noIgnore, bool ignoreExternals)
		{
			return Status2(new SvnPath(path,mPool), revision.ToSvnOpt(mPool), statusFunc, statusBaton,
						  recurse, getAll, update, noIgnore, ignoreExternals, mContext, mPool);
		}

		/// <summary>
		/// Invoke <paramref name="receiver"/> with <paramref name="baton"/> on each 
		/// log message from <paramref name="start"/> to <paramref name="end"/> in turn, 
		/// inclusive (but never invoke <paramref name="receiver"/> on a given log message 
		/// more than once).
		/// </summary>
		/// <param name="targets">Contains either a URL followed by zero or more relative paths, 
		/// or a list of working copy paths, for which log messages are desired.  
		/// The repository info is determined by taking the common prefix of the 
		/// target entries' URLs. <paramref name="receiver"/> is invoked only on messages whose 
		/// revisions involved a change to some path in <paramref name="targets"/>.</param>
		/// <param name="start">Revision to start from</param>
		/// <param name="end">End revision</param>
		/// <param name="limit">If is non-zero only invoke <paramref name="receiver"/> on the 
		/// first <paramref name="limit"/> logs.</param>
		/// <param name="discoverChangedPaths">If is set, then the `changed_paths' argument
		/// to <paramref name="receiver"/> will be passed on each invocation.</param>
		/// <param name="strictNodeHistory">If is set, copy history (if any exists) will
		/// not be traversed while harvesting revision logs for each target.</param>
		/// <param name="receiver">Log message receiver callback function</param>
		/// <param name="baton">Log message receiver baton</param>
		public void Log2(ICollection targets,
						SvnRevision start, SvnRevision end, int limit,
						bool discoverChangedPaths, bool strictNodeHistory,
						LogMessageReceiver receiver, IntPtr baton)
		{
			Log2(AprArray.LazyMake(mPool, targets, typeof(SvnPath)),
				start.ToSvnOpt(mPool), end.ToSvnOpt(mPool), limit,
				discoverChangedPaths, strictNodeHistory, receiver, baton,
				mContext, mPool);
		}

		/// <summary>
		/// Invoke <paramref name="receiver"/> with <paramref name="receiver_baton"/> on each line-blame item
		/// associated with revision <paramref name="end"/> of <paramref name="pathOrUrl"/>, using <paramref name="start"/>
		/// as the default source of all blame.
		/// </summary>
		/// <param name="pathOrUrl">Path or URL to blame</param>
		/// <param name="peg_revision">Indicates in which revision <paramref name="pathOrUrl"/> is valid.  
		/// If <paramref name="peg_revision"/> kind is Svn.Revision.Unspecified, then it defaults to 
		/// Svn.Revision.Head for URLs or Svn.Revision.Working for WC targets.</param>
		/// <param name="start">Start revision</param>
		/// <param name="end">End revision</param>
		/// <param name="receiver">Blame callback function</param>
		/// <param name="baton">Blame callback function baton</param>
		/// <remarks>If <paramref name="start"/> or <paramref name="end"/> kind is Svn.Revision.Unspecified,
		/// exception thrown. If any of the revisions of <paramref name="pathOrUrl"/> have a binary mime-type, 
		/// <c>SvnException</c> thrown.</remarks>
		public void Blame2(string pathOrUrl, SvnRevision peg_revision,
						  SvnRevision start, SvnRevision end,
						  BlameReceiver receiver, IntPtr receiver_baton)
		{
			InternalBlame2(PathOrUrl(pathOrUrl), peg_revision.ToSvnOpt(mPool),
						  start.ToSvnOpt(mPool), end.ToSvnOpt(mPool),
						  receiver, receiver_baton, mContext, mPool);
		}

		/// <summary>
		/// Produce diff output which describes the delta between
		/// <paramref name="path1"/>/<paramref name="revision1"/> and 
		/// <paramref name="path2"/>/<paramref name="revision2"/>.  Print the output 
		/// of the diff to <paramref name="outfile"/>, and any errors to 
		/// <paramref name="errfile"/>.  <paramref name="path1"/> and 
		/// <paramref name="path2"/> can be either working-copy paths or URLs.
		/// </summary>
		/// <param name="diffOptions">Used to pass additional command line options 
		/// to the diff processes invoked to compare files.</param>
		/// <param name="path1">First file or URL</param>
		/// <param name="revision1">Operative revision of the <paramref name="path1"/></param>
		/// <param name="path2">Second file or URL</param>
		/// <param name="revision2">Operative revision of the <paramref name="path2"/></param>
		/// <param name="recurse">If is true (and the paths are directories) this will be a
		/// recursive operation.</param>
		/// <param name="ignoreAncestry">Used to control whether or not items being
		/// diffed will be checked for relatedness first.  Unrelated items
		/// are typically transmitted to the editor as a deletion of one thing
		/// and the addition of another, but if this flag is <c>true</c>,
		/// unrelated items will be diffed as if they were related.</param>
		/// <param name="noDiffDeleted">If is true, then no diff output will be
		/// generated on deleted files.</param>
		/// <param name="ignoreContentType">Diff output will not be generated for binary files, 
		/// unless this flag is true, in which case diffs will be shown
		/// regardless of the content types.</param>
		/// <param name="header_encoding">Encoding for generated headers.</param>
		/// <param name="outFile">Diff output will be in this file</param>
		/// <param name="errFile">Diff errors will be in this file</param>
		/// <remarks> If either <paramref name="revision1"/> or <paramref name="revision2"/> 
		/// has an `Svn.Revision.Unspecified' or unrecognized `kind', SvnException thrown.
		/// <paramref name="path1"/> and <paramref name="path2"/> must both represent the same 
		/// node kind -- that is, if <paramref name="path1"/> is a directory, <paramref name="path2"/> 
		/// must also be, and if <paramref name="path1"/> is a file, <paramref name="path2"/> must also be.  
		/// (Currently, <paramref name="path1"/> and <paramref name="path2"/> must be the exact same path)</remarks>
		public void Diff3(ICollection diffOptions,
						string path1, SvnRevision revision1,
						string path2, SvnRevision revision2,
						bool recurse, bool ignoreAncestry, bool noDiffDeleted,
						bool ignoreContentType, string header_encoding,
						AprFile outFile, AprFile errFile)
		{ 
			InternalDiff3(AprArray.LazyMake(mPool,diffOptions,typeof(AprString)),
						PathOrUrl(path1), revision1.ToSvnOpt(mPool),
						PathOrUrl(path2), revision2.ToSvnOpt(mPool),
						recurse, ignoreAncestry, noDiffDeleted,
						ignoreContentType, new SvnData(header_encoding, mPool),
						outFile, errFile, mContext, mPool);
		}

		/// <summary>
		/// Produce diff output which describes the delta between the
		/// filesystem object <paramref name="path"/> in peg revision 
		/// <paramref name="peg_revision"/>, as it changed between 
		/// <paramref name="start_revision"/> and <paramref name="end_revision"/>.  
		/// <paramref name="path"/> can be either a working-copy path or URL.
		/// </summary>
		/// <param name="diffOptions">Used to pass additional command line options 
		/// to the diff processes invoked to compare files.</param>
		/// <param name="path">Path or URL</param>
		/// <param name="peg_revision">peg revision</param>
		/// <param name="start_revision">Start revision</param>
		/// <param name="end_revision">End revision</param>
		/// <param name="recurse">If is true (and the paths are directories) this will be a
		/// recursive operation.</param>
		/// <param name="ignoreAncestry">Used to control whether or not items being
		/// diffed will be checked for relatedness first.  Unrelated items
		/// are typically transmitted to the editor as a deletion of one thing
		/// and the addition of another, but if this flag is <c>true</c>,
		/// unrelated items will be diffed as if they were related.</param>
		/// <param name="noDiffDeleted">If is true, then no diff output will be
		/// generated on deleted files.</param>
		/// <param name="ignoreContentType">Diff output will not be generated for binary files, 
		/// unless this flag is true, in which case diffs will be shown
		/// regardless of the content types.</param>
		/// <param name="header_encoding">Encoding for generated headers.</param>
		/// <param name="outFile">Diff output will be in this file</param>
		/// <param name="errFile">Diff errors will be in this file</param>
		public void DiffPeg3(ICollection diffOptions,
							string path, SvnRevision peg_revision,
							SvnRevision start_revision, SvnRevision end_revision,
							bool recurse, bool ignoreAncestry, bool noDiffDeleted,
							bool ignoreContentType, string header_encoding,
							AprFile outFile, AprFile errFile)
		{
			DiffPeg3(AprArray.LazyMake(mPool, diffOptions, typeof(AprString)),
								PathOrUrl(path), peg_revision.ToSvnOpt(mPool),
								start_revision.ToSvnOpt(mPool), end_revision.ToSvnOpt(mPool),
								recurse, ignoreAncestry, noDiffDeleted,
								ignoreContentType, new SvnData(header_encoding, mPool),
								outFile, errFile,
								mContext, mPool);
		}

		/// <summary>
		/// Merge changes from <paramref name="source1"/>/<paramref name="revision1"/> to 
		/// <paramref name="source2"/>/<paramref name="revision2"/> into the working-copy 
		/// path <paramref name="targetWCPath"/>.
		/// </summary>
		/// <param name="source1">First file or URL</param>
		/// <param name="revision1">Operative revision of the <paramref name="source1"/></param>
		/// <param name="source2">Second file or URL</param>
		/// <param name="revision2">Operative revision of the <paramref name="source2"/></param>
		/// <param name="targetWCPath">Working-copy path where merge results will be stored</param>
		/// <param name="recurse">If is true (and the URLs are directories), apply changes
		/// recursively; otherwise, only apply changes in the current directory.</param>
		/// <param name="ignoreAncestry">Used to control whether or not items being
		/// diffed will be checked for relatedness first.  Unrelated items
		/// are typically transmitted to the editor as a deletion of one thing
		/// and the addition of another, but if this flag is <c>true</c>,
		/// unrelated items will be diffed as if they were related.</param>
		/// <param name="force">If is not set and the merge involves deleting 
		/// locally modified or unversioned items the operation will fail.
		/// If is set such items will be deleted.</param>
		/// <param name="dryRun">If is true the merge is carried out, and full notification
		/// feedback is provided, but the working copy is not modified.</param>
		/// <remarks> <paramref name="source1"/> and <paramref name="source2"/> are either URLs 
		/// that refer to entries in the repository, or paths to entries in the working copy.
		/// <paramref name="source1"/> and <paramref name="source2"/> must both represent 
		/// the same node kind -- that is, if <paramref name="source1"/> is a directory, 
		/// <paramref name="source2"/> must also be, and if <paramref name="source1"/> 
		/// is a file, <paramref name="source2"/> must also be.
		/// If either <paramref name="revision1"/> or <paramref name="revision2"/> has an 
		/// `Unspecified' or unrecognized `kind', SvnException thrown.</remarks>
		public void Merge(string source1, SvnRevision revision1,
						  string source2, SvnRevision revision2,
						  string targetWCPath, bool recurse,
						  bool ignoreAncestry, bool force, bool dryRun)
		{
			InternalMerge(PathOrUrl(source1), revision1.ToSvnOpt(mPool),
						  PathOrUrl(source2), revision2.ToSvnOpt(mPool),
						  new SvnPath(targetWCPath,mPool),
						  recurse, ignoreAncestry, force, dryRun, mContext, mPool);
		}

		/// <summary>
		/// Merge the changes between the filesystem object <paramref name="source"/> in peg
		/// revision <paramref name="peg_revision"/>, as it changed between <paramref name="revision1"/>
		/// and <paramref name="revision2"/>.
		/// </summary>
		/// <param name="source">Path or URL to merge</param>
		/// <param name="revision1">First revision</param>
		/// <param name="revision2">Second revision</param>
		/// <param name="peg_revision">Peg revision</param>
		/// <param name="targetWCPath">Working-copy path where merge results will be stored</param>
		/// <param name="recurse">If is true (and the URLs are directories), apply changes
		/// recursively; otherwise, only apply changes in the current directory.</param>
		/// <param name="ignoreAncestry">Used to control whether or not items being
		/// diffed will be checked for relatedness first.  Unrelated items
		/// are typically transmitted to the editor as a deletion of one thing
		/// and the addition of another, but if this flag is <c>true</c>,
		/// unrelated items will be diffed as if they were related.</param>
		/// <param name="force">If is not set and the merge involves deleting 
		/// locally modified or unversioned items the operation will fail.
		/// If is set such items will be deleted.</param>
		/// <param name="dryRun">If is true the merge is carried out, and full notification
		/// feedback is provided, but the working copy is not modified.</param>
		public void Merge(string source, SvnRevision revision1,
						  SvnRevision revision2, SvnRevision peg_revision,
						  string targetWCPath, bool recurse,
						  bool ignoreAncestry, bool force, bool dryRun)
		{
			MergePeg(PathOrUrl(source), revision1.ToSvnOpt(mPool),
				  revision2.ToSvnOpt(mPool), peg_revision.ToSvnOpt(mPool),
				  new SvnPath(targetWCPath, mPool), recurse, ignoreAncestry, force, dryRun, mContext, mPool);
		}

		/// <summary>
		/// Recursively cleanup a working copy directory, finishing any
		/// incomplete operations, removing lockfiles, etc.
		/// </summary>
		/// <param name="dir">Directory to cleanup</param>
		public void CleanUp(string dir)
		{
			CleanUp(new SvnPath(dir, mPool), mContext, mPool);
		}

		/// <summary>
		/// Modify a working copy directory <paramref name="dir"/>, changing any
		/// repository URLs that begin with <paramref name="from"/> to begin with 
		/// <paramref name="to"/> instead, recursing into subdirectories 
		/// if <paramref name="recurse"/> is true.
		/// </summary>
		/// <param name="dir">Working copy directory</param>
		/// <param name="from">Original URL</param>
		/// <param name="to">New URL</param>
		/// <param name="recurse">Whether to recurse</param>
		public void Relocate(string dir, string from, string to, bool recurse)
		{
			Relocate(new SvnPath(dir, mPool), new SvnUrl(from, mPool), new SvnUrl(to, mPool),
					 recurse, mContext, mPool);
		}

		/// <summary>
		/// Restore the pristine version of a working copy <paramref name="paths"/>,
		/// effectively undoing any local mods.  For each path in <paramref name="paths"/>, if
		/// it is a directory, and <paramref name="recursive"/> is <c>true</c>, this will be a
		/// recursive operation.
		/// </summary>
		/// <param name="paths">Working copy paths</param>
		/// <param name="recursive">Whether to recurse</param>
		public void Revert(ICollection paths, bool recursive)
		{
			Revert(AprArray.LazyMake(mPool, paths, typeof(SvnPath)), recursive, mContext, mPool);
		}

		/// <summary>
		/// Remove the 'conflicted' state on a working copy <paramref name="path"/>.
		/// This will not semantically resolve conflicts;  it just allows <paramref name="path"/> 
		/// to be committed in the future.  The implementation details are opaque.
		/// If <paramref name="recursive"/> is set, recurse below <paramref name="path"/>, 
		/// looking for conflicts to resolve.
		/// </summary>
		/// <param name="path">Working copy path</param>
		/// <param name="recurse">Whether to recurse</param>
		public void Resolved(string path, bool recurse)
		{
			Resolved(new SvnPath(path, mPool), recurse, mContext, mPool);
		}

		/// <summary>
		/// Copy <paramref name="srcPath"/> to <paramref name="dstPath"/>.
		/// </summary>
		/// <param name="srcPath">Source path</param>
		/// <param name="srcRevision">Revision of the source if <paramref name="srcPath"/> is URL</param>
		/// <param name="dstPath">Destination path</param>
		/// <returns>Results of the commit in SvnCommitInfo if commit occurred</returns>
		/// <remarks><paramref name="srcPath"/> must be a file or directory under version control, or the
		/// URL of a versioned item in the repository.  If <paramref name="srcPath"/> is a 
		/// URL, <paramref name="srcRevision"/> is used to choose the revision from which to copy 
		/// the <paramref name="srcPath"/>. <paramref name="dstPath"/> must be a file or directory under version
		/// control, or a repository URL, existent or not.
		/// If <paramref name="dstPath"/> is a URL, immediately attempt to commit the copy 
		/// action in the repository. 
		/// If <paramref name="dstPath"/> is not a URL, then this is just a
		/// variant of SvnClient.Add, where the <paramref name="dstPath"/> items are scheduled
		/// for addition as copies.  No changes will happen to the repository
		/// until a commit occurs.  This scheduling can be removed with SvnClient.Revert.</remarks>
		public SvnCommitInfo Copy2(string srcPath, SvnRevision srcRevision, string dstPath)
		{
			return InternalCopy2(PathOrUrl(srcPath), srcRevision.ToSvnOpt(mPool),
								PathOrUrl(dstPath), mContext, mPool);
		}

		/// <summary>
		/// Move <paramref name="srcPath"/> to <paramref name="dstPath"/>.
		/// </summary>
		/// <param name="srcPath">File or directory under version control, or the
		/// URL of a versioned item in the repository.</param>
		/// <param name="dstPath">Destination path or URL</param>
		/// <param name="force">Force move modified and/or unversioned items</param>
		/// <returns>Results of the commit in SvnCommitInfo if commit occurred</returns>
		/// <remarks>
		/// If <paramref name="srcPath"/> is a repository URL <paramref name="dstPath"/> 
		/// must also be a repository URL (existent or not), the move operation will 
		/// be immediately committed.
		/// If <paramref name="srcPath"/> is a working copy path <paramref name="dstPath"/> 
		/// must also be a working copy path (existent or not), this is a scheduling 
		/// operation.  No changes will happen to the repository until a commit occurs.
		/// This scheduling can be removed with SvnClient.Revert. If <paramref name="srcPath"/> 
		/// is a file it is removed from the working copy immediately. 
		/// If <paramref name="srcPath"/> is a directory it will remain in the working 
		/// copy but all the files, and unversioned items, it contains will be removed.
		/// If <paramref name="srcPath"/> contains locally modified and/or unversioned items 
		/// and <paramref name="force"/> is not set, the copy will fail. 
		/// If <paramref name="force"/> is set such items will be removed.
		/// </remarks>
		public SvnCommitInfo Move3( string srcPath, string dstPath, bool force)
		{
			return InternalMove3(PathOrUrl(srcPath), PathOrUrl(dstPath), force, mContext, mPool);
		}

		/// <summary>
		/// Set subversion property on <paramref name="target"/>
		/// </summary>
		/// <param name="propName">Property name</param>
		/// <param name="propVal">Property value. If <paramref name="propName"/> is empty property will be deleted.</param>
		/// <param name="target">Target</param>
		/// <param name="recurse">If is true, then <paramref name="propName"/> will be set on recursively 
		/// on <paramref name="target"/> and all children.  If is false, and <paramref name="target"/> is 
		/// a directory, <paramref name="propName"/> will be set on _only_ <paramref name="target"/>.</param>
		/// <param name="skip_checks">If is true, do no validity checking.  
		/// But if is false, and <paramref name="propName"/> is not a valid property for 
		/// <paramref name="target"/>, SvnException thrown</param>
		public void PropSet2(string propName, SvnString propVal, string target,
								   bool recurse, bool skip_checks)
		{
			InternalPropSet2(propName, propVal, PathOrUrl(target), recurse, 
							 skip_checks, mContext, mPool);
		}

		/// <summary>
		/// Set <paramref name="propName"/> to <paramref name="propVal"/> on 
		/// revision <paramref name="revision"/> in the repository 
		/// represented by <paramref name="url"/>.
		/// A <paramref name="propVal"/> of NULL will delete the property.
		/// </summary>
		/// <param name="propName">Property name</param>
		/// <param name="propVal">Property value. If <paramref name="propName"/> is an 
		/// svn-controlled property (i.e. prefixed with "svn:"), then the caller is 
		/// responsible for ensuring that the value UTF8-encoded and uses LF line-endings.</param>
		/// <param name="url">Repository URL</param>
		/// <param name="revision">Operative revision</param>
		/// <param name="force">If is true, allow newlines in the author property</param>
		/// <returns>Actual revision affected in operation</returns>
		/// <remarks> 
		/// Note that unlike its cousin SvnClient.PropSet2(), this routine
		/// doesn't affect the working copy at all;  it's a pure network
		/// operation that changes an *unversioned* property attached to a
		/// revision.  This can be used to tweak log messages, dates, authors,
		/// and the like.  Be careful:  it's a lossy operation.
		/// Also note that unless the administrator creates a
		/// pre-revprop-change hook in the repository, this feature will fail.
		/// </remarks>
		public int RevPropSet(string propName, SvnString propVal,
							  string url, SvnRevision revision, bool force)		
		{
			return RevPropSet(propName, propVal, new SvnUrl(url, mPool),
							  revision.ToSvnOpt(mPool), force, mContext, mPool);
		}

		/// <summary>
		/// Returns a hash table whose keys are paths,
		/// prefixed by <paramref name="target"/> (a working copy path or a URL), of items on
		/// which property <paramref name="propName"/> is set, 
		/// and whose values are SvnString representing the property value 
		/// for <paramref name="propName"/> at that path.
		/// </summary>
		/// <param name="propName">Property name</param>
		/// <param name="target">Target path or URL</param>
		/// <param name="peg_revision">Peg revision</param>
		/// <param name="revision">Operative revision</param>
		/// <param name="recurse">Recurse into subdirectory</param>
		/// <returns>Hash table with property values if property exists or empty hash table</returns>
		/// <remarks>
		/// If <paramref name="target"/> is a file or <paramref name="recurse"/> is false, 
		/// returned hash table will have at most one element.
		/// Don't store any path, not even <paramref name="target"/>, if it does not have a
		/// property named <paramref name="propName"/>.
		/// If <paramref name="revision"/> kind is 'Unspecified', then: get
		/// properties from the working copy if <paramref name="target"/> is a working copy
		/// path, or from the repository head if <paramref name="target"/> is a URL.  Else get
		/// the properties as of <paramref name="revision"/>.  The actual node revision
		/// selected is determined by the path as it exists in <paramref name="peg_revision"/>.
		/// If <paramref name="peg_revision"/> kind is 'Unspecified', then
		/// it defaults to Svn.Revision.Head for URLs or Svn.Revision.Working for WC targets.  
		///  </remarks>
		public AprHash PropGet2(string propName, string target,
								SvnRevision peg_revision, SvnRevision revision, bool recurse)
		{
			return InternalPropGet2(propName, PathOrUrl(target), peg_revision.ToSvnOpt(mPool), 
									revision.ToSvnOpt(mPool), recurse, mContext, mPool);
		}

		/// <summary>
		/// Returns the value of <paramref name="propName"/> on revision <paramref name="revision"/> 
		/// in the repository represented by <paramref name="url"/>.  
		/// </summary>
		/// <param name="propName">Property name</param>
		/// <param name="url">Repository URL</param>
		/// <param name="revision">Operative revision</param>
		/// <param name="setRev">Actual revision queried.</param>
		/// <returns>The value of <paramref name="propName"/></returns>
		public SvnString RevPropGet(string propName, string url,
									SvnRevision revision, out int setRev)		
		{
			return RevPropGet(propName, new SvnUrl(url, mPool), revision.ToSvnOpt(mPool),
							  out setRev, mContext, mPool);
		}

		public AprArray PropList(string target, SvnRevision revision, bool recurse)		
		{
			return InternalPropList(new SvnPath(target, mPool), revision.ToSvnOpt(mPool), recurse,
									mContext, mPool);
		}

		public AprHash RevPropList(string url, SvnRevision revision, out int setRev)		
		{
			return RevPropList(new SvnUrl(url, mPool), revision.ToSvnOpt(mPool), out setRev,
							   mContext, mPool);
		}

		public int Export3(string from, string to, SvnRevision peg_revision, SvnRevision revision, 
							bool overwrite, bool ignore_externals, bool recurse)
		{
			return InternalExport3(PathOrUrl(from), new SvnPath(to,mPool), 
									peg_revision.ToSvnOpt(mPool), revision.ToSvnOpt(mPool), 
									overwrite, ignore_externals, recurse, mContext, mPool);
		}

		//TODO: Replace this with call to svn_client_list; replace Obsolete method from List()
		public AprHash List2(string pathOrUrl, SvnRevision peg_revision, SvnRevision revision, bool recurse)		
		{
			return InternalList2(PathOrUrl(pathOrUrl), peg_revision.ToSvnOpt(mPool), revision.ToSvnOpt(mPool), recurse,
				mContext, mPool);
		}

		public void Cat2(SvnStream stream, string pathOrUrl, SvnRevision peg_revision, SvnRevision revision)		
		{
			InternalCat2(stream, PathOrUrl(pathOrUrl), peg_revision.ToSvnOpt(mPool), revision.ToSvnOpt(mPool), mContext, mPool);
		}

		public void Lock(ICollection targets, string comment, bool steal_lock)
		{
			InternalLock(AprArray.LazyMake(mPool, targets, PathOrUrlArrayType(targets)), comment, steal_lock, mContext, mPool);	
		}

		public void UnLock(ICollection targets, bool break_lock)
		{
			InternalUnLock(AprArray.LazyMake(mPool, targets, PathOrUrlArrayType(targets)), break_lock, mContext, mPool);	
		}

		public void Info(string pathOrUrl, SvnRevision peg_revision, SvnRevision revision, 
						 InfoReceiver receiver, IntPtr receiver_baton, bool recurse)
		{
			InternalInfo(PathOrUrl(pathOrUrl), peg_revision.ToSvnOpt(mPool), revision.ToSvnOpt(mPool), receiver, receiver_baton, recurse,
						 mContext, mPool);
		}
		#endregion

		#region Utilities methods
		public string UrlFromPath(string pathOrUrl)
		{
			return InternalUrlFromPath(PathOrUrl(pathOrUrl), mPool);
		}

		public string UuidFromUrl(string url)
		{
			return UuidFromUrl(new SvnUrl(url, mPool), mContext, mPool);
		}
		#endregion

		#region Obsolete Client Methods
		// Methods which have been deprecated in the Subversion API
		[Obsolete("Provided for backward compatibility with the 1.1 API. Use SvnClient.Checkout2 instead")]
		public int Checkout(string url, string path, SvnRevision revision, bool recurse)
		{
			return Checkout(new SvnUrl(url, mPool), new SvnPath(path, mPool),
							revision.ToSvnOpt(mPool), recurse, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. Use SvnClient.Update2 instead")]
		public int Update(string path, SvnRevision revision, bool recurse)
		{
			return Update(new SvnPath(path, mPool), revision.ToSvnOpt(mPool), recurse,
						  mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.0 API. Use SvnClient.Add3 instead")]
		public void Add(string path, bool recurse)
		{
			Add(new SvnPath(path, mPool), recurse, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API. Use SvnClient.Mkdir2 instead")]
		public SvnClientCommitInfo Mkdir(ICollection paths)
		{
			return Mkdir(AprArray.LazyMake(mPool, paths, PathOrUrlArrayType(paths)), mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API. Use SvnClient.Delete2 instead")]
		public SvnClientCommitInfo Delete(ICollection paths, bool force)
		{
			return Delete(AprArray.LazyMake(mPool, paths, PathOrUrlArrayType(paths)), force, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API. Use SvnClient.Import2 instead")]
		public SvnClientCommitInfo Import(string path, SvnUrl url, bool nonrecursive)
		{
			return Import(new SvnPath(path, mPool), url, nonrecursive, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. Use SvnClient.Commit3 instead")]
		public SvnClientCommitInfo Commit(ICollection targets, bool nonrecursive)
		{
			return Commit(AprArray.LazyMake(mPool, targets, typeof(SvnPath)), nonrecursive,
						  mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. Use SvnClient.Status2 instead")]
		public int Status(string path, SvnRevision revision,
						  SvnWcStatus.Func statusFunc, IntPtr statusBaton,
						  bool descend, bool getAll, bool update, bool noIgnore)
		{
			return Status(new SvnPath(path, mPool), revision.ToSvnOpt(mPool), statusFunc, statusBaton,
						  descend, getAll, update, noIgnore, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.0 API. Use SvnClient.Log2 instead")]
		public void Log(ICollection targets,
						SvnRevision start, SvnRevision end,
						bool discoverChangedPaths, bool strictNodeHistory,
						LogMessageReceiver receiver, IntPtr baton)
		{
			Log(AprArray.LazyMake(mPool, targets, typeof(SvnPath)),
				start.ToSvnOpt(mPool), end.ToSvnOpt(mPool),
				discoverChangedPaths, strictNodeHistory, receiver, baton,
				mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. Use SvnClient.Blame2 instead")]
		public void Blame(string pathOrUrl,
						  SvnRevision start, SvnRevision end,
						  BlameReceiver receiver, IntPtr baton)
		{
			InternalBlame(PathOrUrl(pathOrUrl),
						  start.ToSvnOpt(mPool), end.ToSvnOpt(mPool),
						  receiver, baton, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API. Use SvnClient.Diff3 instead")]
		public void Diff(ICollection diffOptions,
						 string path1, SvnRevision revision1,
						 string path2, SvnRevision revision2,
						 bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
						 AprFile outFile, AprFile errFile)
		{
			InternalDiff(AprArray.LazyMake(mPool, diffOptions, typeof(AprString)),
				 PathOrUrl(path1), revision1.ToSvnOpt(mPool),
				 PathOrUrl(path2), revision2.ToSvnOpt(mPool),
				 recurse, ignoreAncestry, noDiffDeleted, ignoreContentType,
				 outFile, errFile, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API. Use SvnClient.DiffPeg3 instead")]
		public void Diff(ICollection diffOptions,
			string path, SvnRevision peg_revision,
			SvnRevision start_revision, SvnRevision end_revision,
			bool recurse, bool ignoreAncestry, bool noDiffDeleted,
			AprFile outFile, AprFile errFile)
		{
			DiffPeg(AprArray.LazyMake(mPool, diffOptions, typeof(AprString)),
				PathOrUrl(path), peg_revision.ToSvnOpt(mPool),
				start_revision.ToSvnOpt(mPool), end_revision.ToSvnOpt(mPool),
				recurse, ignoreAncestry, noDiffDeleted,
				outFile, errFile, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API. Use SvnClient.DiffPeg3 instead")]
		public void Diff(ICollection diffOptions,
			string path, SvnRevision peg_revision,
			SvnRevision start_revision, SvnRevision end_revision,
			bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
			AprFile outFile, AprFile errFile)
		{
			DiffPeg2(AprArray.LazyMake(mPool, diffOptions, typeof(AprString)),
				PathOrUrl(path), peg_revision.ToSvnOpt(mPool),
				start_revision.ToSvnOpt(mPool), end_revision.ToSvnOpt(mPool),
				recurse, ignoreAncestry, noDiffDeleted, ignoreContentType,
				outFile, errFile, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API. Use SvnClient.Copy2 instead")]
		public SvnClientCommitInfo Copy(string srcPath, SvnRevision srcRevision, string dstPath)
		{
			return InternalCopy(PathOrUrl(srcPath), srcRevision.ToSvnOpt(mPool),
								PathOrUrl(dstPath), mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. Use SvnClient.Move3 instead")]
		public SvnClientCommitInfo Move(string srcPath, SvnRevision srcRevision,
										string dstPath, bool force)
		{
			return InternalMove(PathOrUrl(srcPath), srcRevision.ToSvnOpt(mPool),
								PathOrUrl(dstPath), force, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. User SvnClient.PropSet2 instead")]
		public void PropSet(string propName, SvnString propVal, string target, bool recurse)
		{
			InternalPropSet(propName, propVal, new SvnPath(target, mPool), recurse, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. User SvnClient.PropGet2 instead.")]
		public AprHash PropGet(string propName, string target,
							   SvnRevision revision, bool recurse)
		{
			return InternalPropGet(propName, new SvnPath(target, mPool), revision.ToSvnOpt(mPool), recurse,
								   mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.0 API. Use Export3 instead.")]
		public int Export(string from, string to, SvnRevision revision, bool force)
		{
			return InternalExport(PathOrUrl(from), new SvnPath(to,mPool), revision.ToSvnOpt(mPool),
								  force, mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. Use List2 instead")]
		public AprHash List(string pathOrUrl, SvnRevision revision, bool recurse)		
		{
			return InternalList(PathOrUrl(pathOrUrl), revision.ToSvnOpt(mPool), recurse,
								mContext, mPool);
		}

		[Obsolete("Provided for backward compatibility with the 1.1 API. Use Cat2 instead.")]
		public void Cat(SvnStream stream, string pathOrUrl, SvnRevision revision)		
		{
			InternalCat(stream, PathOrUrl(pathOrUrl), revision.ToSvnOpt(mPool), mContext, mPool);
		}

		#endregion

		#region Authentication methods
		public void AddSimpleProvider()
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetSimpleProvider(mGlobalPool));
		}
		
		public void AddUsernameProvider()
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetUsernameProvider(mGlobalPool));
		}
		
		public void AddSslServerTrustFileProvider()
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetSslServerTrustFileProvider(mGlobalPool));
		}
		
		public void AddSslClientCertFileProvider()
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetSslClientCertFileProvider(mGlobalPool));
		}
		
		public void AddSslClientCertPwFileProvider()
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetSslClientCertPwFileProvider(mGlobalPool));
		}

		public void AddPromptProvider(SvnAuthProviderObject.SimplePrompt promptFunc,
									  IntPtr promptBaton, int retryLimit)
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetPromptProvider(promptFunc, promptBaton, 
																  retryLimit, mGlobalPool));
		}

		public void AddPromptProvider(SvnAuthProviderObject.UsernamePrompt promptFunc,
									  IntPtr promptBaton, int retryLimit)
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetPromptProvider(promptFunc, promptBaton, 
																  retryLimit, mGlobalPool));
		}

		public void AddPromptProvider(SvnAuthProviderObject.SslServerTrustPrompt promptFunc,
									  IntPtr promptBaton)
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetPromptProvider(promptFunc, promptBaton, 
																  mGlobalPool));
		}

		public void AddPromptProvider(SvnAuthProviderObject.SslClientCertPrompt promptFunc,
									  IntPtr promptBaton, int retryLimit)
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetPromptProvider(promptFunc, promptBaton, 
																  retryLimit, mGlobalPool));
		}

		public void AddPromptProvider(SvnAuthProviderObject.SslClientCertPwPrompt promptFunc,
									  IntPtr promptBaton, int retryLimit)
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mAuthObjs.Add(SvnAuthProviderObject.GetPromptProvider(promptFunc, promptBaton, 
																  retryLimit, mGlobalPool));
		}
		
		public void OpenAuth()
		{
			if( mAuthObjs == null )
				mAuthObjs = new ArrayList();
			mContext.AuthBaton = SvnAuthBaton.Open(mAuthObjs,mGlobalPool);
			mAuthObjs = null;
		}
		#endregion
		
		#region Member access through properties
		public AprPool GlobalPool
		{
			get
			{
				return(mGlobalPool);
			}
		}
		public AprPool Pool
		{
			get
			{
				return(mPool);
			}
		}
		public SvnClientContext Context
		{
			get
			{
				return(mContext);
			}
		}
		#endregion

		#region Prop utils
		/// <summary>
		/// Translate property if required (only properties with prefix 'svn:' are translated)
		/// </summary>
		/// <param name="propName">Property name</param>
		/// <param name="propVal">Property value</param>
		/// <returns>Translated property value</returns>
		public SvnString PropTranslate(string propName, string propVal)
		{
			if (PropNeedsTranslation(propVal))
			{
				string lfString = propVal.Replace("\n\r", "\n");
				return new SvnString(new AprString(System.Text.Encoding.UTF8.GetBytes(lfString), mPool), mPool);
			}
			else
			{
				return new SvnString(propVal, mPool);
			}
		}

		/// <summary>
		/// Detranslate property if required (only properties with prefix 'svn:' are detranslated)
		/// </summary>
		/// <param name="propName">Property name</param>
		/// <param name="propVal">Property value</param>
		/// <returns>Detranslated property value</returns>
		public string PropDetranslate(string propName, SvnString propVal)
		{
			if (!propVal.IsNull && propVal.Len > 0)
			{
				string lfString = propVal.ToString();
				if (PropNeedsTranslation(lfString))
				{
					lfString = lfString.Replace("\r\n", "\n");
					return new AprString(System.Text.Encoding.UTF8.GetBytes(lfString), mPool).Value;
				}
				return lfString;
			}
			return string.Empty;
		}
		#endregion
	}
}