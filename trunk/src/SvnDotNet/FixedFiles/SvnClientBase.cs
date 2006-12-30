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
using Softec.AprSharp;

namespace Softec.SubversionSharp
{
    public class SvnClientBase
    {
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
    	public delegate SvnError GetCommitLog(out AprString logMessage, out SvnPath tmpFile,
         						 		   	  AprArray commitItems, IntPtr baton,
         								      AprPool pool);
		public delegate SvnError GetCommitLog2(out AprString logMessage, out SvnPath tmpFile,
											  AprArray commitItems, IntPtr baton,
											  AprPool pool);

 		public delegate SvnError LogMessageReceiver(IntPtr baton, AprHash changed_paths, 
 													int revision, AprString author,
 													AprString date, AprString message,
 													AprPool pool);

 		public delegate SvnError BlameReceiver(IntPtr baton, long line_no, int revision, 
 											   AprString author, AprString date, AprString line, 
 											   AprPool pool);

		public delegate SvnError InfoReceiver(IntPtr baton, SvnPath path, SvnInfo info, AprPool pool);

		public delegate SvnError CancelFunc(IntPtr baton);

		/// <summary>
		/// Callback function for progress notification.
		/// </summary>
		/// <param name="progress">number of bytes already transferred</param>
		/// <param name="total">total number of bytes to transfer or -1 if it's not known</param>
		/// <param name="baton">callback baton</param>
		/// <param name="pool">pool</param>
		public delegate void ProgressNotifyFunc(long progress, long total, IntPtr baton, IntPtr pool);

		#region Checkout
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]            								                								       
		internal static int Checkout(SvnUrl url, SvnPath path, 
								   SvnOptRevision revision, 
								   bool recurse, SvnClientContext ctx, AprPool pool)
		{
			int rev;
			Debug.Write(String.Format("svn_client_checkout({0},{1},{2},{3},{4},{5})...",url,path,revision,recurse,ctx,pool));
			SvnError err = Svn.svn_client_checkout(out rev, url, path, 
												   revision, 
												   (recurse ? 1 :0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			return(rev);
		}

		internal static int Checkout2(SvnUrl url, SvnPath path, 
								   SvnOptRevision peg_revision, SvnOptRevision revision, 
								   bool recurse, bool ignoreExternals, SvnClientContext ctx, AprPool pool)
		{
			int rev;
			Debug.Write(String.Format("svn_client_checkout2({0},{1},{2},{3},{4},{5},{6},{7})...",url,path,peg_revision,revision,recurse,ignoreExternals,ctx,pool));
			SvnError err = Svn.svn_client_checkout2(out rev, url, path, 
												   peg_revision,
												   revision, 
												   (recurse ? 1 :0), (ignoreExternals ? 1 :0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			return(rev);
		}
		#endregion

		#region Update
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static int Update(SvnPath path, 
								 SvnOptRevision revision, 
								 bool recurse, SvnClientContext ctx, AprPool pool)
		{
			int rev;
			Debug.Write(String.Format("svn_client_update({0},{1},{2},{3},{4})...",path,revision,recurse,ctx,pool));
			SvnError err = Svn.svn_client_update(out rev, path, 
												 revision,
												 (recurse ? 1 :0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			return(rev);
		}

		internal static AprArray Update2(AprArray paths, SvnOptRevision revision, bool recurse, bool ignoreExternals, 
										SvnClientContext ctx, AprPool pool)
		{
			IntPtr revs = IntPtr.Zero;
			Debug.Write(String.Format("svn_client_update2({0},{1},{2},{3},{4},{5})...", paths, revision, recurse, ignoreExternals, ctx, pool));

			SvnError err = Svn.svn_client_update2(out revs, paths, revision, (recurse ? 1 : 0), (ignoreExternals ? 1 : 0), ctx, pool);

			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", revs));
			return new AprArray(revs, typeof(int));
		}
		#endregion

		#region Switch
		internal static int Switch(SvnPath path, SvnUrl url, 
								 SvnOptRevision revision, 
								 bool recurse, SvnClientContext ctx, AprPool pool)
		{
			int rev;
			Debug.Write(String.Format("svn_client_switch({0},{1},{2},{3},{4},{5})...",path,url,revision,recurse,ctx,pool));
			SvnError err = Svn.svn_client_switch(out rev, path, url, 
												 revision, 
												 (recurse ? 1 :0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			return(rev);
		}
		#endregion

		#region Add
		[Obsolete("Provided for backward compatibility with the 1.0 API.")]
		internal static void Add(SvnPath path,
							   bool recurse, 
							   SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_add({0},{1},{2},{3})",path,recurse,ctx,pool));
			SvnError err = Svn.svn_client_add(path, 
											  (recurse ? 1 :0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		internal static void Add3(SvnPath path, bool recurse, bool force, bool no_ignore,
							   SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_add3({0},{1},{2},{3},{4},{5})", path, recurse, force, no_ignore, ctx, pool));
			SvnError err = Svn.svn_client_add3(path, (recurse ? 1 : 0), (force ? 1 : 0), (no_ignore ? 1 : 0), ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}
		#endregion

		#region Mkdir
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static SvnClientCommitInfo Mkdir(AprArray paths,  
							   					SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_mkdir({0},{1},{2})...",paths,ctx,pool));
			SvnError err = Svn.svn_client_mkdir(out commitInfo, paths, ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",commitInfo));
			return(commitInfo);
		}
		internal static SvnCommitInfo Mkdir2(AprArray paths,
												SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_mkdir2({0},{1},{2})...", paths, ctx, pool));
			SvnError err = Svn.svn_client_mkdir2(out commitInfo, paths, ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", commitInfo));
			return (commitInfo);
		}
		#endregion

		#region Delete
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static SvnClientCommitInfo Delete(AprArray paths, bool force,
							   					 SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_delete({0},{1},{2},{3})...",paths,force,ctx,pool));
			SvnError err = Svn.svn_client_delete(out commitInfo, paths, (force ? 1 : 0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",commitInfo));
			return(commitInfo);
		}

		internal static SvnCommitInfo Delete2(AprArray paths, bool force,
												 SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_delete2({0},{1},{2},{3})...", paths, force, ctx, pool));
			SvnError err = Svn.svn_client_delete2(out commitInfo, paths, (force ? 1 : 0), ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", commitInfo));
			return (commitInfo);
		}
		#endregion

		#region Import
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static SvnClientCommitInfo Import(SvnPath path, SvnUrl url, bool nonrecursive,  
							   					 SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_import({0},{1},{2},{3},{4})...",path,url,nonrecursive,ctx,pool));
			SvnError err = Svn.svn_client_import(out commitInfo, path, url, (nonrecursive ? 1 : 0), 
												 ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",commitInfo));
			return(commitInfo);
		}

		internal static SvnCommitInfo Import2(SvnPath path, SvnUrl url, bool nonrecursive, bool no_ignore,
												 SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_import2({0},{1},{2},{3},{4},{5})...", path, url, nonrecursive, no_ignore, ctx, pool));
			SvnError err = Svn.svn_client_import2(out commitInfo, path, url, (nonrecursive ? 1 : 0), (no_ignore ? 1 : 0),
												 ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", commitInfo));
			return (commitInfo);
		}
		#endregion

		#region Commit
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static SvnClientCommitInfo Commit(AprArray targets, bool nonrecursive,
							   					 SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_commit({0},{1},{2},{3})...",targets,nonrecursive,ctx,pool));
			SvnError err = Svn.svn_client_commit(out commitInfo, targets, (nonrecursive ? 1 : 0),
												 ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",commitInfo));
			return(commitInfo);
		}

		internal static SvnCommitInfo Commit3(AprArray targets, bool recurse, bool keep_locks,
												 SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_commit3({0},{1},{2},{3},{4})...", targets, recurse, keep_locks, ctx, pool));
			SvnError err = Svn.svn_client_commit3(out commitInfo, targets, (recurse ? 1 : 0), (keep_locks ? 1 : 0),
												 ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", commitInfo));
			return (commitInfo);
		}
		#endregion

		#region Status
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static int Status(SvnPath path,
								 SvnOptRevision revision,
								 SvnWcStatus.Func statusFunc, IntPtr statusBaton,
								 bool recurse, bool getAll, bool update, bool noIgnore,
			   					 SvnClientContext ctx, AprPool pool)
		{
			int rev;
			SvnDelegate statusDelegate = new SvnDelegate(statusFunc);
			Debug.Write(String.Format("svn_client_status({0},{1},{2},{3},{4:X},{5},{6},{7},{8},{9})...",path,revision,statusFunc.Method.Name,statusBaton.ToInt32(),recurse,getAll,update,noIgnore,ctx,pool));
			SvnError err = Svn.svn_client_status(out rev, path, revision,
												 (Svn.svn_wc_status_func_t) statusDelegate.Wrapper,
												 statusBaton,
												 (recurse ? 1 : 0), (getAll ? 1 : 0),
												 (update ? 1 : 0), (noIgnore ? 1 : 0),
												 ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			if( update )
				return(rev);
			else
				return(-1);
		}

		internal static int Status2(SvnPath path,
								 SvnOptRevision revision,
								 SvnWcStatus2.Func statusFunc, IntPtr statusBaton,
								 bool recurse, bool getAll, bool update, bool noIgnore, bool ignore_externals,
			   					 SvnClientContext ctx, AprPool pool)
		{
			int rev;
			SvnDelegate statusDelegate = new SvnDelegate(statusFunc);
			Debug.Write(String.Format("svn_client_status2({0},{1},{2},{3},{4:X},{5},{6},{7},{8},{9}{10})...",path,revision,statusFunc.Method.Name,statusBaton.ToInt32(),recurse,getAll,update,noIgnore,ignore_externals,ctx,pool));
			SvnError err = Svn.svn_client_status2(out rev, path, revision,
												 (Svn.svn_wc_status_func2_t) statusDelegate.Wrapper,
												 statusBaton,
												 (recurse ? 1 : 0), (getAll ? 1 : 0),
												 (update ? 1 : 0), (noIgnore ? 1 : 0),
												 (ignore_externals ? 1 : 0),
												 ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			if( update )
				return(rev);
			else
				return(-1);
		}
		#endregion

		#region Log
		[Obsolete("Provided for backward compatibility with the 1.0 API.")]
		internal static void Log(AprArray targets,
							   SvnOptRevision start, SvnOptRevision end,
							   bool discoverChangedPaths, bool strictNodeHistory,
							   LogMessageReceiver receiver, IntPtr baton,
							   SvnClientContext ctx, AprPool pool)
		{
			SvnDelegate receiverDelegate = new SvnDelegate(receiver);
			Debug.WriteLine(String.Format("svn_client_log({0},{1},{2},{3},{4},{5},{6:X},{7},{8})",targets,start,end,discoverChangedPaths,strictNodeHistory,receiver.Method.Name,baton.ToInt32(),ctx,pool));
			SvnError err = Svn.svn_client_log(targets, start, end,
											  (discoverChangedPaths ? 1 :0),
											  (strictNodeHistory ? 1 :0),
											  (Svn.svn_log_message_receiver_t)receiverDelegate.Wrapper,
											  baton,
											  ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}

		internal static void Log2(AprArray targets,
							   SvnOptRevision start, SvnOptRevision end, int limit,
							   bool discoverChangedPaths, bool strictNodeHistory,
							   LogMessageReceiver receiver, IntPtr baton,
							   SvnClientContext ctx, AprPool pool)
		{
			SvnDelegate receiverDelegate = new SvnDelegate(receiver);
			Debug.WriteLine(String.Format("svn_client_log({0},{1},{2},{3},{4},{5},{6},{7:X},{8},{9})", targets, start, end, limit, discoverChangedPaths, strictNodeHistory, receiver.Method.Name, baton.ToInt32(), ctx, pool));
			SvnError err = Svn.svn_client_log2(targets, start, end, limit,
											  (discoverChangedPaths ? 1 : 0),
											  (strictNodeHistory ? 1 : 0),
											  (Svn.svn_log_message_receiver_t)receiverDelegate.Wrapper,
											  baton,
											  ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}
		#endregion

		#region Blame
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void Blame(SvnPath pathOrUrl,
								 SvnOptRevision start, SvnOptRevision end, 
								 BlameReceiver receiver, IntPtr baton,
							     SvnClientContext ctx, AprPool pool)
		{
			InternalBlame(pathOrUrl, start, end, receiver, baton, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void Blame(SvnUrl pathOrUrl,
								 SvnOptRevision start, SvnOptRevision end, 
								 BlameReceiver receiver, IntPtr baton,
							     SvnClientContext ctx, AprPool pool)
		{
			InternalBlame(pathOrUrl, start, end, receiver, baton, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void InternalBlame(IAprUnmanaged pathOrUrl,
								 		  SvnOptRevision start, SvnOptRevision end, 
										  BlameReceiver receiver, IntPtr baton,
										  SvnClientContext ctx, AprPool pool)
		{
			SvnDelegate receiverDelegate = new SvnDelegate(receiver);
			Debug.WriteLine(String.Format("svn_client_blame({0},{1},{2},{3},{4:X},{5},{6})",pathOrUrl,start,end,receiver.Method.Name,baton.ToInt32(),ctx,pool));
			SvnError err = Svn.svn_client_blame(pathOrUrl.ToIntPtr(), start, end,
											    (Svn.svn_client_blame_receiver_t)receiverDelegate.Wrapper,
											    baton,
											    ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}

		internal static void Blame2(SvnUrl pathOrUrl, SvnOptRevision peg_revision,
								 SvnOptRevision start, SvnOptRevision end, 
								 BlameReceiver receiver, IntPtr baton,
							     SvnClientContext ctx, AprPool pool)
		{
			InternalBlame2(pathOrUrl, peg_revision, start, end, receiver, baton, ctx, pool);
		}

		internal static void Blame2(SvnPath pathOrUrl, SvnOptRevision peg_revision,
								 SvnOptRevision start, SvnOptRevision end, 
								 BlameReceiver receiver, IntPtr baton,
							     SvnClientContext ctx, AprPool pool)
		{
			InternalBlame2(pathOrUrl, peg_revision, start, end, receiver, baton, ctx, pool);
		}

		internal static void InternalBlame2(IAprUnmanaged pathOrUrl, SvnOptRevision peg_revision,
								 		  SvnOptRevision start, SvnOptRevision end, 
										  BlameReceiver receiver, IntPtr baton,
										  SvnClientContext ctx, AprPool pool)
		{
			SvnDelegate receiverDelegate = new SvnDelegate(receiver);
			Debug.WriteLine(String.Format("svn_client_blame2({0},{1},{2},{3},{4},{5:X},{6},{7})",pathOrUrl,peg_revision,start,end,receiver.Method.Name,baton.ToInt32(),ctx,pool));
			SvnError err = Svn.svn_client_blame2(pathOrUrl.ToIntPtr(), peg_revision, start, end,
											    (Svn.svn_client_blame_receiver_t)receiverDelegate.Wrapper,
											    baton,
											    ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region Diff
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static void Diff(AprArray diffOptions,
								SvnPath path1, SvnOptRevision revision1,
								SvnPath path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
								AprFile outFile, AprFile errFile,  
							    SvnClientContext ctx, AprPool pool)
		{
			InternalDiff(diffOptions, path1, revision1, path2, revision2,
						 recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, outFile, errFile, ctx, pool);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static void Diff(AprArray diffOptions,
								SvnUrl path1, SvnOptRevision revision1,
								SvnPath path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
								AprFile outFile, AprFile errFile,  
							    SvnClientContext ctx, AprPool pool)
		{
			InternalDiff(diffOptions, path1, revision1, path2, revision2,
						 recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, outFile, errFile, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static void Diff(AprArray diffOptions,
								SvnPath path1, SvnOptRevision revision1,
								SvnUrl path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
								AprFile outFile, AprFile errFile,  
							    SvnClientContext ctx, AprPool pool)
		{
			InternalDiff(diffOptions, path1, revision1, path2, revision2,
						 recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, outFile, errFile, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static void Diff(AprArray diffOptions,
								SvnUrl path1, SvnOptRevision revision1,
								SvnUrl path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
								AprFile outFile, AprFile errFile,  
							    SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_diff({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11})",diffOptions,path1,revision1,path2,revision2,recurse,ignoreAncestry,noDiffDeleted,outFile,errFile,ctx,pool));
			SvnError err = Svn.svn_client_diff2(diffOptions, path1, revision1, path2, revision2,
											   (recurse ? 1 : 0), (ignoreAncestry ? 1 : 0),
											   (noDiffDeleted ? 1 : 0), 
											   (ignoreContentType ? 1 : 0), 
											   outFile, errFile,
											   ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static void InternalDiff(AprArray diffOptions,
								IAprUnmanaged path1, SvnOptRevision revision1,
								IAprUnmanaged path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
								AprFile outFile, AprFile errFile,  
							    SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_diff({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11})",diffOptions,path1,revision1,path2,revision2,recurse,ignoreAncestry,noDiffDeleted,outFile,errFile,ctx,pool));
			SvnError err = Svn.svn_client_diff2(diffOptions, path1.ToIntPtr(), revision1, 
											   path2.ToIntPtr(), revision2,
											   (recurse ? 1 : 0), (ignoreAncestry ? 1 : 0),
											   (noDiffDeleted ? 1 : 0), 
											   (ignoreContentType ? 1 : 0), 
											   outFile, errFile,
											   ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}

		internal static void Diff3(AprArray diffOptions,
								SvnPath path1, SvnOptRevision revision1,
								SvnPath path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, 
								bool ignoreContentType, SvnData header_encoding,
								AprFile outFile, AprFile errFile,
								SvnClientContext ctx, AprPool pool)
		{
			InternalDiff3(diffOptions, path1, revision1, path2, revision2,
						 recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, header_encoding, outFile, errFile, ctx, pool);
		}

		internal static void Diff3(AprArray diffOptions,
								SvnUrl path1, SvnOptRevision revision1,
								SvnPath path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted,
								bool ignoreContentType, SvnData header_encoding,
								AprFile outFile, AprFile errFile,
								SvnClientContext ctx, AprPool pool)
		{
			InternalDiff3(diffOptions, path1, revision1, path2, revision2,
						 recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, header_encoding, outFile, errFile, ctx, pool);
		}

		internal static void Diff3(AprArray diffOptions,
								SvnPath path1, SvnOptRevision revision1,
								SvnUrl path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted,
								bool ignoreContentType, SvnData header_encoding,
								AprFile outFile, AprFile errFile,
								SvnClientContext ctx, AprPool pool)
		{
			InternalDiff3(diffOptions, path1, revision1, path2, revision2,
						 recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, header_encoding, outFile, errFile, ctx, pool);
		}

		internal static void Diff3(AprArray diffOptions,
								SvnUrl path1, SvnOptRevision revision1,
								SvnUrl path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted,
								bool ignoreContentType, SvnData header_encoding,
								AprFile outFile, AprFile errFile,
								SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_diff3({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13})", diffOptions, path1, revision1, path2, revision2, recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, header_encoding, outFile, errFile, ctx, pool));
			SvnError err = Svn.svn_client_diff3(diffOptions, path1, revision1, path2, revision2,
											   (recurse ? 1 : 0), (ignoreAncestry ? 1 : 0),
											   (noDiffDeleted ? 1 : 0),
											   (ignoreContentType ? 1 : 0),
											   header_encoding,
											   outFile, errFile,
											   ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}

		internal static void InternalDiff3(AprArray diffOptions,
								IAprUnmanaged path1, SvnOptRevision revision1,
								IAprUnmanaged path2, SvnOptRevision revision2,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted,
								bool ignoreContentType, SvnData header_encoding,
								AprFile outFile, AprFile errFile,
								SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_diff3({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13})", diffOptions, path1, revision1, path2, revision2, recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, header_encoding, outFile, errFile, ctx, pool));
			SvnError err = Svn.svn_client_diff3(diffOptions, path1.ToIntPtr(), revision1, path2.ToIntPtr(), revision2,
											   (recurse ? 1 : 0), (ignoreAncestry ? 1 : 0),
											   (noDiffDeleted ? 1 : 0),
											   (ignoreContentType ? 1 : 0),
											   header_encoding,
											   outFile, errFile,
											   ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}
		#endregion

		#region DiffPeg
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void DiffPeg(AprArray diffOptions,
								IAprUnmanaged path, SvnOptRevision peg_revision,
								SvnOptRevision start_revision, SvnOptRevision end_revision,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted,
								AprFile outFile, AprFile errFile,  
							    SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_diff_peg({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11})",diffOptions,path,peg_revision,start_revision,end_revision,recurse,ignoreAncestry,noDiffDeleted,outFile,errFile,ctx,pool));
			SvnError err = Svn.svn_client_diff_peg(diffOptions, path.ToIntPtr(), peg_revision, start_revision, end_revision,
											   (recurse ? 1 : 0), (ignoreAncestry ? 1 : 0),
											   (noDiffDeleted ? 1 : 0), 
											   outFile, errFile,
											   ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static void DiffPeg2(AprArray diffOptions,
								IAprUnmanaged path, SvnOptRevision peg_revision,
								SvnOptRevision start_revision, SvnOptRevision end_revision,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, bool ignoreContentType,
								AprFile outFile, AprFile errFile,  
							    SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_diff_peg2({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11})",diffOptions,path,peg_revision,start_revision,end_revision,recurse,ignoreAncestry,noDiffDeleted,outFile,errFile,ctx,pool));
			SvnError err = Svn.svn_client_diff_peg2(diffOptions, path.ToIntPtr(), peg_revision, start_revision, end_revision,
											   (recurse ? 1 : 0), (ignoreAncestry ? 1 : 0),
											   (noDiffDeleted ? 1 : 0), 
											   (ignoreContentType ? 1 : 0), 
											   outFile, errFile,
											   ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		internal static void DiffPeg3(AprArray diffOptions,
								IAprUnmanaged path, SvnOptRevision peg_revision,
								SvnOptRevision start_revision, SvnOptRevision end_revision,
								bool recurse, bool ignoreAncestry, bool noDiffDeleted, 
								bool ignoreContentType, SvnData header_encoding,
								AprFile outFile, AprFile errFile,
								SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_diff_peg3({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13})", diffOptions, path, peg_revision, start_revision, end_revision, recurse, ignoreAncestry, noDiffDeleted, ignoreContentType, header_encoding, outFile, errFile, ctx, pool));
			SvnError err = Svn.svn_client_diff_peg3(diffOptions, path.ToIntPtr(), peg_revision, start_revision, end_revision,
											   (recurse ? 1 : 0), (ignoreAncestry ? 1 : 0),
											   (noDiffDeleted ? 1 : 0),
											   (ignoreContentType ? 1 : 0),
											   header_encoding,
											   outFile, errFile,
											   ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}
		#endregion

		#region Merge
		internal static void Merge(SvnPath source1, SvnOptRevision revision1,
								 SvnPath source2, SvnOptRevision revision2,
								 SvnPath targetWCPath, bool recurse,
								 bool ignoreAncestry, bool force, bool dryRun,
							     SvnClientContext ctx, AprPool pool)
		{
			InternalMerge(source1, revision1, source2, revision2, targetWCPath, recurse,
						  ignoreAncestry, force, dryRun, ctx, pool);
		}

		internal static void Merge(SvnUrl source1, SvnOptRevision revision1,
								 SvnUrl source2, SvnOptRevision revision2,
								 SvnPath targetWCPath, bool recurse,
								 bool ignoreAncestry, bool force, bool dryRun,
							     SvnClientContext ctx, AprPool pool)
		{
			InternalMerge(source1, revision1, source2, revision2, targetWCPath, recurse,
						  ignoreAncestry, force, dryRun, ctx, pool);
		}

		internal static void InternalMerge(IAprUnmanaged source1, SvnOptRevision revision1,
										  IAprUnmanaged source2, SvnOptRevision revision2,
										  SvnPath targetWCPath, bool recurse,
										  bool ignoreAncestry, bool force, bool dryRun,
										  SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_merge({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10})",source1,revision1,source2,revision2,targetWCPath,recurse,ignoreAncestry,force,dryRun,ctx,pool));
			SvnError err = Svn.svn_client_merge(source1.ToIntPtr(), revision1, 
												source2.ToIntPtr(), revision2,
											    targetWCPath, (recurse ? 1 : 0),
											    (ignoreAncestry ? 1 : 0), (force ? 1 : 0),
											    (dryRun ? 1 : 0),
											    ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region MergePeg
		internal static void MergePeg(IAprUnmanaged source, 
										SvnOptRevision revision1,
										SvnOptRevision revision2,
										SvnOptRevision peg_revision,
										SvnPath targetWCPath, bool recurse,
										bool ignoreAncestry, bool force, bool dryRun,
										SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_merge_peg({0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10})",source,revision1,revision2,peg_revision,targetWCPath,recurse,ignoreAncestry,force,dryRun,ctx,pool));
			SvnError err = Svn.svn_client_merge_peg(source.ToIntPtr(), revision1, 
												revision2, peg_revision,
											    targetWCPath, (recurse ? 1 : 0),
											    (ignoreAncestry ? 1 : 0), (force ? 1 : 0),
											    (dryRun ? 1 : 0),
											    ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region Cleanup
		internal static void CleanUp(SvnPath dir,
							       SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_cleanup({0},{1},{2})",dir,ctx,pool));
			SvnError err = Svn.svn_client_cleanup(dir, ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region Relocate
		internal static void Relocate(SvnPath dir, SvnUrl from, SvnUrl to,
									bool recurse,
							        SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_relocate({0},{1},{2},{3},{4},{5})",dir,from,to,recurse,ctx,pool));
			SvnError err = Svn.svn_client_relocate(dir, from, to, (recurse ? 1 : 0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region Revert
		internal static void Revert(AprArray paths, bool recurse, 								  
							      SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_revert({0},{1},{2},{3})",paths,recurse,ctx,pool));
			SvnError err = Svn.svn_client_revert(paths, (recurse ? 1 : 0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region Resolved
		internal static void Resolved(SvnPath path, bool recurse, 								  
							        SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_resolved({0},{1},{2},{3})",path,recurse,ctx,pool));
			SvnError err = Svn.svn_client_resolved(path, (recurse ? 1 : 0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region Copy
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static void Copy(SvnPath srcPath, SvnOptRevision srcRevision,
								SvnPath dstPath,
								SvnClientContext ctx, AprPool pool)
		{
			InternalCopy(srcPath, srcRevision, dstPath, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static SvnClientCommitInfo Copy(SvnUrl srcPath, SvnOptRevision srcRevision,
											   SvnUrl dstPath,
							   				   SvnClientContext ctx, AprPool pool)
		{
			return InternalCopy(srcPath, srcRevision, dstPath, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static SvnClientCommitInfo InternalCopy(IAprUnmanaged srcPath, SvnOptRevision srcRevision,
														IAprUnmanaged dstPath,
							   							SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_copy({0},{1},{2},{3},{4})...",srcPath,srcRevision,dstPath,ctx,pool));
			SvnError err = Svn.svn_client_copy(out commitInfo, srcPath.ToIntPtr(), srcRevision,
											   dstPath.ToIntPtr(),
											   ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",commitInfo));
			return(commitInfo);
		}

		internal static void Copy2(SvnPath srcPath, SvnOptRevision srcRevision,
								SvnPath dstPath,
								SvnClientContext ctx, AprPool pool)
		{
			InternalCopy2(srcPath, srcRevision, dstPath, ctx, pool);
		}
		internal static SvnCommitInfo Copy2(SvnUrl srcPath, SvnOptRevision srcRevision,
											   SvnUrl dstPath,
											   SvnClientContext ctx, AprPool pool)
		{
			return InternalCopy2(srcPath, srcRevision, dstPath, ctx, pool);
		}
		internal static SvnCommitInfo InternalCopy2(IAprUnmanaged srcPath, SvnOptRevision srcRevision,
														IAprUnmanaged dstPath,
														SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_copy2({0},{1},{2},{3},{4})...", srcPath, srcRevision, dstPath, ctx, pool));
			SvnError err = Svn.svn_client_copy2(out commitInfo, srcPath.ToIntPtr(), srcRevision,
											   dstPath.ToIntPtr(),
											   ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", commitInfo));
			return (commitInfo);
		}
		#endregion

		#region Move
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void Move(SvnPath srcPath, SvnOptRevision srcRevision,
								SvnPath dstPath, bool force,
								SvnClientContext ctx, AprPool pool)
		{
			InternalMove(srcPath, srcRevision, dstPath, force, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static SvnClientCommitInfo Move(SvnUrl srcPath, SvnOptRevision srcRevision,
											   SvnUrl dstPath, bool force,
							   				   SvnClientContext ctx, AprPool pool)
		{
			return InternalMove(srcPath, srcRevision, dstPath, force, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static SvnClientCommitInfo InternalMove(IAprUnmanaged srcPath, 
													   SvnOptRevision srcRevision,
													   IAprUnmanaged dstPath, bool force,
							   						   SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_move({0},{1},{2},{3},{4},{5})...",srcPath,srcRevision,dstPath,force,ctx,pool));
			SvnError err = Svn.svn_client_move(out commitInfo, srcPath.ToIntPtr(), srcRevision,
											   dstPath.ToIntPtr(), (force ? 1 : 0),
											   ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",commitInfo));
			return(commitInfo);
		}

		internal static void Move3(SvnPath srcPath, SvnPath dstPath, bool force,
								SvnClientContext ctx, AprPool pool)
		{
			InternalMove3(srcPath, dstPath, force, ctx, pool);
		}

		internal static SvnCommitInfo Move3(SvnUrl srcPath, SvnUrl dstPath, bool force,
											   SvnClientContext ctx, AprPool pool)
		{
			return InternalMove3(srcPath, dstPath, force, ctx, pool);
		}

		internal static SvnCommitInfo InternalMove3(IAprUnmanaged srcPath,
												 IAprUnmanaged dstPath, bool force,
												 SvnClientContext ctx, AprPool pool)
		{
			IntPtr commitInfo;
			Debug.Write(String.Format("svn_client_move3({0},{1},{2},{3},{4})...", srcPath, dstPath, force, ctx, pool));
			SvnError err = Svn.svn_client_move3(out commitInfo, srcPath.ToIntPtr(),
											   dstPath.ToIntPtr(), (force ? 1 : 0),
											   ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", commitInfo));
			return (commitInfo);
		}
		#endregion

		#region PropSet
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void PropSet(string propName, SvnString propVal, SvnPath target, 
								   bool recurse, AprPool pool)		
		{
			InternalPropSet(propName, propVal, target, recurse, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void PropSet(string propName, SvnString propVal, SvnUrl target, 
								   bool recurse, AprPool pool)		
		{
			InternalPropSet(propName, propVal, target, recurse, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void InternalPropSet(string propName, SvnString propVal, IAprUnmanaged target, 
											bool recurse, AprPool pool)		
		{
			Debug.WriteLine(String.Format("svn_client_propset({0},{1},{2},{3},{4})",propName,propVal,target,recurse,pool));
			SvnError err = Svn.svn_client_propset(propName, propVal, target.ToIntPtr(),
												  (recurse ? 1 : 0), pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}

		internal static void PropSet2(string propName, SvnString propVal, SvnPath target,
								   bool recurse, bool skip_checks, SvnClientContext ctx, AprPool pool)
		{
			InternalPropSet2(propName, propVal, target, recurse, skip_checks, ctx, pool);
		}

		internal static void PropSet2(string propName, SvnString propVal, SvnUrl target,
								   bool recurse, bool skip_checks, SvnClientContext ctx, AprPool pool)
		{
			InternalPropSet2(propName, propVal, target, recurse, skip_checks, ctx, pool);
		}

		internal static void InternalPropSet2(string propName, SvnString propVal, IAprUnmanaged target,
											bool recurse, bool skip_checks, SvnClientContext ctx, AprPool pool)
		{
			Debug.WriteLine(String.Format("svn_client_propset2({0},{1},{2},{3},{4},{5},{6})", propName, propVal, target, recurse, skip_checks, ctx, pool));
			SvnError err = Svn.svn_client_propset2(propName, propVal, target.ToIntPtr(),
												  (recurse ? 1 : 0), (skip_checks ? 1 : 0), ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}
		#endregion

		#region RevPropSet
		internal static int RevPropSet(string propName, SvnString propVal,
								  	 SvnUrl url, SvnOptRevision revision, bool force,
								  	 SvnClientContext ctx, AprPool pool)		
		{
			int rev;
			Debug.Write(String.Format("svn_client_revprop_set({0},{1},{2},{3},{4},{5},{6})...",propName,propVal,url,revision,force,ctx,pool));
			SvnError err = Svn.svn_client_revprop_set(propName, propVal, url, revision,
													  out rev, (force ? 1 : 0),
													  ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			return(rev);
		}
		#endregion

		#region PropGet
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprHash PropGet(string propName, SvnPath target,
									  SvnOptRevision revision, bool recurse, 
								  	  SvnClientContext ctx, AprPool pool)		
		{
			return InternalPropGet(propName, target, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprHash PropGet(string propName, SvnUrl target,
									  SvnOptRevision revision, bool recurse, 
								  	  SvnClientContext ctx, AprPool pool)		
		{
			return InternalPropGet(propName, target, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprHash InternalPropGet(string propName, IAprUnmanaged target,
											   SvnOptRevision revision, bool recurse, 
								  			   SvnClientContext ctx, AprPool pool)		
		{
			IntPtr h;
			Debug.Write(String.Format("svn_client_propget({0},{1},{2},{3},{4},{5})...",propName,target,revision,recurse,ctx,pool));
			SvnError err = Svn.svn_client_propget(out h, propName, target.ToIntPtr(), revision,
												  (recurse ? 1 : 0),
												  ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",h));
			return(h);
		}

		internal static AprHash PropGet2(string propName, SvnPath target,
									  SvnOptRevision peg_revision, SvnOptRevision revision, bool recurse,
									  SvnClientContext ctx, AprPool pool)
		{
			return InternalPropGet2(propName, target, peg_revision, revision, recurse, ctx, pool);
		}

		internal static AprHash PropGet2(string propName, SvnUrl target,
									  SvnOptRevision peg_revision, SvnOptRevision revision, bool recurse,
									  SvnClientContext ctx, AprPool pool)
		{
			return InternalPropGet2(propName, target, peg_revision, revision, recurse, ctx, pool);
		}

		internal static AprHash InternalPropGet2(string propName, IAprUnmanaged target,
											   SvnOptRevision peg_revision, SvnOptRevision revision, bool recurse,
											   SvnClientContext ctx, AprPool pool)
		{
			IntPtr h;
			Debug.Write(String.Format("svn_client_propget2({0},{1},{2},{3},{4},{5},{6})...", propName, target, peg_revision, revision, recurse, ctx, pool));
			SvnError err = Svn.svn_client_propget2(out h, propName, target.ToIntPtr(), 
												  peg_revision, revision, 
												  (recurse ? 1 : 0),
												  ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", h));
			return (h);
		}
		#endregion

		#region RevPropGet
		internal static SvnString RevPropGet(string propName, SvnUrl url,
										   SvnOptRevision revision, out int setRev, 
										   SvnClientContext ctx, AprPool pool)		
		{
			IntPtr s;
			Debug.Write(String.Format("svn_client_revprop_get({0},{1},{2},{3},{4})...",propName,url,revision,ctx,pool));
			SvnError err = Svn.svn_client_revprop_get(propName, out s, url, revision, out setRev,
												      ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0},{1})",s,setRev));
			return(s);
		}
		#endregion 

		#region PropList
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprArray PropList(SvnPath target,
									   SvnOptRevision revision, bool recurse, 
								  	   SvnClientContext ctx, AprPool pool)		
		{
			return InternalPropList(target, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprArray PropList(SvnUrl target,
									   SvnOptRevision revision, bool recurse, 
								  	   SvnClientContext ctx, AprPool pool)		
		{
			return InternalPropList(target, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprArray InternalPropList(IAprUnmanaged target,
									   			SvnOptRevision revision, bool recurse, 
								  	   			SvnClientContext ctx, AprPool pool)		
		{
			IntPtr h = IntPtr.Zero;
			Debug.Write(String.Format("svn_client_proplist({0},{1},{2},{3},{4})...",target,revision,recurse,ctx,pool));
			SvnError err = Svn.svn_client_proplist(out h, target.ToIntPtr(), revision,
												   (recurse ? 1 : 0),
												   ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",h));
			return new AprArray(h);
		}

		internal static AprArray PropList2(SvnPath target,
									   SvnOptRevision peg_revision, SvnOptRevision revision, bool recurse,
									   SvnClientContext ctx, AprPool pool)
		{
			return InternalPropList2(target, peg_revision, revision, recurse, ctx, pool);
		}

		internal static AprArray PropList2(SvnUrl target,
									   SvnOptRevision peg_revision, SvnOptRevision revision, bool recurse,
									   SvnClientContext ctx, AprPool pool)
		{
			return InternalPropList2(target, peg_revision, revision, recurse, ctx, pool);
		}

		internal static AprArray InternalPropList2(IAprUnmanaged target,
												SvnOptRevision peg_revision, SvnOptRevision revision, bool recurse,
												SvnClientContext ctx, AprPool pool)
		{
			IntPtr h = IntPtr.Zero;
			Debug.Write(String.Format("svn_client_proplist2({0},{1},{2},{3},{4},{5})...", target, peg_revision, revision, recurse, ctx, pool));
			SvnError err = Svn.svn_client_proplist2(out h, target.ToIntPtr(), 
													peg_revision, revision,
													(recurse ? 1 : 0),
													ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", h));
			return new AprArray(h);
		}
		#endregion

		#region RevPropList
		internal static AprHash RevPropList(SvnUrl url,
										  SvnOptRevision revision, out int setRev, 
										  SvnClientContext ctx, AprPool pool)		
		{
			IntPtr h;
			Debug.Write(String.Format("svn_client_revprop_list({0},{1},{2},{3})...",url,revision,ctx,pool));
			SvnError err = Svn.svn_client_revprop_list(out h, url, revision, out setRev,
												       ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0},{1})",h,setRev));
			return(h);
		}
		#endregion

		#region Export
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void Export(SvnPath from, SvnPath to, 
								  SvnOptRevision revision, 
								  bool force, SvnClientContext ctx, AprPool pool)
		{
			InternalExport(from, to, revision, force, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static int Export(SvnUrl from, SvnPath to, 
								 SvnOptRevision revision, 
								 bool force, SvnClientContext ctx, AprPool pool)
		{
			return InternalExport(from, to, revision, force, ctx, pool);
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static int InternalExport(IAprUnmanaged from, SvnPath to, 
										  SvnOptRevision revision, 
										  bool force, SvnClientContext ctx, AprPool pool)
		{
			int rev;
			Debug.Write(String.Format("svn_client_export({0},{1},{2},{3},{4},{5})...",from,to,revision,force,ctx,pool));
			SvnError err = Svn.svn_client_export(out rev, from.ToIntPtr(), to, 
												 revision, 
												 (force ? 1 :0), ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			return(rev);
		}

		internal static void Export3(SvnPath from, SvnPath to, 
								  SvnOptRevision peg_revision, 
								  SvnOptRevision revision, 
								  bool overwrite, bool ignore_externals, bool recurse,
								  SvnClientContext ctx, AprPool pool)
		{
			InternalExport3(from, to, peg_revision, revision, overwrite, ignore_externals, recurse, ctx, pool);
		}

		internal static int Export3(SvnUrl from, SvnPath to, 
								  SvnOptRevision peg_revision, 
								  SvnOptRevision revision, 
								  bool overwrite, bool ignore_externals, bool recurse,
								  SvnClientContext ctx, AprPool pool)
		{
			return InternalExport3(from, to, peg_revision, revision, overwrite, ignore_externals, recurse, ctx, pool);
		}

		internal static int InternalExport3(IAprUnmanaged from, SvnPath to, 
											 SvnOptRevision peg_revision, 
											 SvnOptRevision revision, 
											 bool overwrite, bool ignore_externals, bool recurse, 
											 SvnClientContext ctx, AprPool pool)
		{
			int rev;
			Debug.Write(String.Format("svn_client_export3({0},{1},{2},{3},{4},{5},{6},{7},{8})...",from,to,peg_revision,revision,overwrite,ignore_externals,recurse,ctx,pool));
			SvnError err = Svn.svn_client_export3(out rev, from.ToIntPtr(), to, 
												 peg_revision, revision, 
												 (overwrite ? 1 : 0),
												 (ignore_externals ? 1 : 0), (recurse ? 1: 0),
												 IntPtr.Zero,
												 ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",rev));
			return(rev);
		}
		#endregion

		#region List
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprHash List(SvnPath pathOrUrl,
								   SvnOptRevision revision, bool recurse, 
								   SvnClientContext ctx, AprPool pool)		
		{
			return InternalList(pathOrUrl, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprHash List(SvnUrl pathOrUrl,
								   SvnOptRevision revision, bool recurse, 
								   SvnClientContext ctx, AprPool pool)		
		{
			return InternalList(pathOrUrl, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static AprHash InternalList(IAprUnmanaged pathOrUrl,
								   			SvnOptRevision revision, bool recurse, 
											SvnClientContext ctx, AprPool pool)		
		{
			IntPtr h;
			Debug.Write(String.Format("svn_client_list({0},{1},{2},{3},{4})...",pathOrUrl,revision,recurse,ctx,pool));
			SvnError err = Svn.svn_client_ls(out h, pathOrUrl.ToIntPtr(), revision, (recurse ? 1 : 0),
											 ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",h));
			return(h);
		}

		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static AprHash List2(SvnPath pathOrUrl, SvnOptRevision peg_revision, 
								   SvnOptRevision revision, bool recurse, 
								   SvnClientContext ctx, AprPool pool)		
		{
			return InternalList2(pathOrUrl, peg_revision, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static AprHash List2(SvnUrl pathOrUrl, SvnOptRevision peg_revision, 
								   SvnOptRevision revision, bool recurse, 
								   SvnClientContext ctx, AprPool pool)		
		{
			return InternalList2(pathOrUrl, peg_revision, revision, recurse, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		internal static AprHash InternalList2(IAprUnmanaged pathOrUrl, SvnOptRevision peg_revision, 
								   			SvnOptRevision revision, bool recurse, 
											SvnClientContext ctx, AprPool pool)		
		{
			IntPtr h;
			Debug.Write(String.Format("svn_client_ls2({0},{1},{2},{3},{4},{5})...",pathOrUrl,peg_revision,revision,recurse,ctx,pool));
			SvnError err = Svn.svn_client_ls2(out h, pathOrUrl.ToIntPtr(), peg_revision, revision, (recurse ? 1 : 0),
											 ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",h));
			return(h);
		}

		internal static AprHash List3(AprHash locks, SvnPath pathOrUrl, SvnOptRevision peg_revision,
								   SvnOptRevision revision, bool recurse,
								   SvnClientContext ctx, AprPool pool)
		{
			return InternalList3(locks, pathOrUrl, peg_revision, revision, recurse, ctx, pool);
		}

		internal static AprHash List3(AprHash locks, SvnUrl pathOrUrl, SvnOptRevision peg_revision,
								   SvnOptRevision revision, bool recurse,
								   SvnClientContext ctx, AprPool pool)
		{
			return InternalList3(locks, pathOrUrl, peg_revision, revision, recurse, ctx, pool);
		}

		internal static AprHash InternalList3(AprHash locks, IAprUnmanaged pathOrUrl, SvnOptRevision peg_revision,
											SvnOptRevision revision, bool recurse,
											SvnClientContext ctx, AprPool pool)
		{
			IntPtr h;
			IntPtr l = locks;
			Debug.Write(String.Format("svn_client_ls3({0},{1},{2},{3},{4},{5})...", pathOrUrl, peg_revision, revision, recurse, ctx, pool));
			SvnError err = Svn.svn_client_ls3(out h, out l, pathOrUrl.ToIntPtr(), peg_revision, revision, (recurse ? 1 : 0),
											 ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})", h));
			return (h);
		}
		#endregion

		#region Cat
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void Cat(SvnStream stream, SvnPath pathOrUrl,
							   SvnOptRevision revision, 
							   SvnClientContext ctx, AprPool pool)		
		{		
			InternalCat(stream, pathOrUrl, revision, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void Cat(SvnStream stream, SvnUrl pathOrUrl,
							   SvnOptRevision revision, 
							   SvnClientContext ctx, AprPool pool)		
		{		
			InternalCat(stream, pathOrUrl, revision, ctx, pool);		
		}
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		internal static void InternalCat(SvnStream stream, IAprUnmanaged pathOrUrl,
										SvnOptRevision revision, 
										SvnClientContext ctx, AprPool pool)		
		{		
			Debug.WriteLine(String.Format("svn_client_cat({0},{1},{2},{3},{4})",stream,pathOrUrl,revision,ctx,pool));
			SvnError err = Svn.svn_client_cat(stream, pathOrUrl.ToIntPtr(), revision, ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}

		internal static void Cat2(SvnStream stream, SvnUrl pathOrUrl,
							   SvnOptRevision peg_revision,	
							   SvnOptRevision revision, 
							   SvnClientContext ctx, AprPool pool)		
		{		
			InternalCat2(stream, pathOrUrl, peg_revision, revision, ctx, pool);		
		}

		internal static void Cat2(SvnStream stream, SvnPath pathOrUrl,
							   SvnOptRevision peg_revision,	
							   SvnOptRevision revision, 
							   SvnClientContext ctx, AprPool pool)		
		{		
			InternalCat2(stream, pathOrUrl, peg_revision, revision, ctx, pool);		
		}

		internal static void InternalCat2(SvnStream stream, IAprUnmanaged pathOrUrl,
										SvnOptRevision peg_revision, SvnOptRevision revision, 
										SvnClientContext ctx, AprPool pool)		
		{		
			Debug.WriteLine(String.Format("svn_client_cat2({0},{1},{2},{3},{4},{5})",stream,pathOrUrl,peg_revision,revision,ctx,pool));
			SvnError err = Svn.svn_client_cat2(stream, pathOrUrl.ToIntPtr(), peg_revision, revision, ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
		}
		#endregion

		#region Lock/Unlock
		internal static void InternalLock(AprArray targets, string comment, bool steal_lock, SvnClientContext ctx, AprPool pool)
		{
			System.Text.UTF8Encoding encoder = new System.Text.UTF8Encoding();
			byte[] utf8comment = encoder.GetBytes(comment);
			SvnError err = Svn.svn_client_lock(targets, new AprString(utf8comment, pool), (steal_lock ? 1 : 0), ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}

		internal static void InternalUnLock(AprArray targets, bool break_lock, SvnClientContext ctx, AprPool pool)
		{
			SvnError err = Svn.svn_client_unlock(targets, (break_lock ? 1 : 0), ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}
		#endregion

		#region Info
		internal static void InternalInfo(IAprUnmanaged pathOrUrl,
										  SvnOptRevision peg_revision, SvnOptRevision revision,
										  InfoReceiver receiver, IntPtr receiver_baton,
										  bool recurse, SvnClientContext ctx, AprPool pool)
		{
			SvnDelegate receiverDelegate = new SvnDelegate(receiver);
			Debug.WriteLine(String.Format("svn_client_info({0},{1},{2},{3},{4:X},{5},{6},{7})", pathOrUrl, peg_revision, revision, receiver.Method.Name, receiver_baton.ToInt32(), recurse, ctx, pool));
			SvnError err = Svn.svn_client_info(pathOrUrl.ToIntPtr(), peg_revision, revision,
												(Svn.svn_info_receiver_t)receiverDelegate.Wrapper,
												receiver_baton, recurse ? 1 : 0,
												ctx, pool);
			if (!err.IsNoError)
				throw new SvnException(err);
		}
		#endregion

		internal static string InternalUrlFromPath(IAprUnmanaged pathOrUrl, AprPool pool)
		{
			IntPtr s;
			Debug.Write(String.Format("svn_client_url_from_path({0},{1})...",pathOrUrl,pool));
			SvnError err = Svn.svn_client_url_from_path(out s, pathOrUrl.ToIntPtr(), pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",s));
			string url = new SvnUrl(s).Value;
			return (url == null? string.Empty : url);
		}

		internal static string UuidFromUrl(SvnUrl url, SvnClientContext ctx, AprPool pool)
		{
			IntPtr s;
			Debug.Write(String.Format("svn_client_uuid_from_url({0},{1})...",url,ctx,pool));
			SvnError err = Svn.svn_client_uuid_from_url(out s, url, ctx, pool);
			if( !err.IsNoError )
				throw new SvnException(err);
			Debug.WriteLine(String.Format("Done({0})",s));
			string uuid = new AprString(s).Value;
			return (uuid == null ? string.Empty : uuid);
		}

		public static SvnVersion Version()
		{
			IntPtr v;
			v = Svn.svn_client_version();
			return v;
		}

		#region Prop utils
		/// <summary>
		/// Return 'true' if propName represents the name of a Subversion property.
		/// </summary>
		/// <param name="propName">Name of the svn property</param>
		/// <returns></returns>
		public static bool PropIsSvnProp(string propName)
		{
			return propName.StartsWith("svn:");
		}

		/// <summary>
		/// If propName requires that its value be stored as UTF8/LF in the
		/// repository, then return 'true'.  Else return 'false'.
		/// </summary>
		/// <param name="propName">Name of the svn property</param>
		/// <returns></returns>
		public static bool PropNeedsTranslation(string propName)
		{
			return PropIsSvnProp(propName);
		}
		#endregion
	}
}