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
using System.Runtime.InteropServices;
using System.Diagnostics;
using Softec.AprSharp;

namespace Softec.SubversionSharp
{

	///<summary>Embeds all Svn external calls</summary>
    public sealed class Svn
    {
        private const string SvnClientLibrary = "svn_client-1";

        // no instance constructor !
        private Svn() { }

		public enum NodeKind
    	{
			None,
			File,
			Dir,
			Unknown
    	}

		public enum Revision {
			Unspecified,
			Number,
			Date,
			Committed,
			Previous,
			Base,
			Working,
			Head
		}

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		internal delegate IntPtr svn_cancel_func_t(IntPtr baton);

		[System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		internal delegate void svn_ra_progress_notify_func_t(long progress, long total, IntPtr baton, IntPtr pool);
        
        #region Wrapper around APR Pool
        // Sorry, but for now, we call apr directly and do not support
        // actuel svn wrappers around apr pool
        
        public static AprAllocator AllocatorCreate()
        {
    		AprAllocator allocator = AprAllocator.Create();
    		//SVN_ALLOCATOR_RECOMMENDED_MAX_FREE = 4Mb
    		allocator.MaxFree = (4096*1024);
    		
    		return(allocator);
        }

        public static AprPool PoolCreate()
        {
            return(PoolCreate((AprPool)IntPtr.Zero));
        }

        public static AprPool PoolCreate(AprPool pool)
        {
    		AprAllocator allocator = Svn.AllocatorCreate();
            return(AprPool.Create(pool,allocator));
        }
        
        public static AprPool PoolCreate(AprAllocator allocator)
        {
            return(PoolCreate(IntPtr.Zero, allocator));
        }
                
        public static AprPool PoolCreate(AprPool pool, AprAllocator allocator)
        {
        	AprPool newpool = AprPool.Create(pool, allocator);
        	allocator.Owner = pool;
            return(newpool);
        }
        #endregion
        
        #region SvnError
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_error_create(int apr_err, IntPtr child, string message);
        
        [DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_error_clear(IntPtr error);   
        #endregion
        
        #region SvnPath
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr	svn_utf_stringbuf_to_utf8(out IntPtr dest, IntPtr src, IntPtr pool);

		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr	svn_utf_string_to_utf8(out IntPtr dest, IntPtr src, IntPtr pool);

  		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr	svn_utf_cstring_to_utf8(out IntPtr dest, IntPtr src, IntPtr pool);
        #endregion
        
        #region SvnUrl
  		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_path_uri_encode(IntPtr path, IntPtr pool);

  		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_path_uri_decode(IntPtr path, IntPtr pool);
        #endregion
        
        #region SvnString
        [DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_string_create(IntPtr cstring, IntPtr pool);
        [DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_string_create(string cstring, IntPtr pool);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_string_ncreate(IntPtr cstring, uint size, IntPtr pool);
        [DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_string_ncreate(string cstring, uint size, IntPtr pool);

		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_string_create_from_buf(IntPtr strbuf, IntPtr pool);
        
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_string_dup(IntPtr original_string, IntPtr pool);
        #endregion
        
        #region SvnStringBuf
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stringbuf_create(IntPtr cstring, IntPtr pool);
        [DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stringbuf_create(string cstring, IntPtr pool);

		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stringbuf_ncreate(IntPtr cstring, uint size, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stringbuf_ncreate(string cstring, uint size, IntPtr pool);
        
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stringbuf_create_from_string(IntPtr str, IntPtr pool);
        
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stringbuf_dup(IntPtr original_string, IntPtr pool);
        #endregion

        #region SvnStream
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_read_fn_t(IntPtr baton, IntPtr buffer, ref uint len);
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_write_fn_t(IntPtr baton, IntPtr data, ref uint len);
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_close_fn_t(IntPtr baton);
        
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stream_create(IntPtr baton, IntPtr pool);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void 	svn_stream_set_baton(IntPtr stream, IntPtr baton);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void 	svn_stream_set_read(IntPtr stream, svn_read_fn_t read_fn);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void 	svn_stream_set_write(IntPtr stream, svn_write_fn_t write_fn);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void 	svn_stream_set_close(IntPtr stream, svn_close_fn_t close_fn);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stream_empty (IntPtr pool);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stream_from_aprfile(IntPtr file, IntPtr pool);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stream_for_stdout(out IntPtr stream, IntPtr pool);

        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_stream_compressed (IntPtr stream, IntPtr pool);
        #endregion

        #region SvnClientContext
	    [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_client_create_context(out IntPtr ctx, IntPtr pool);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate void svn_wc_notify_func_t(IntPtr baton, IntPtr path, 
        											int action, int kind, 
        											IntPtr mime_type, int content_state, 
        											int prop_state, int revision);
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate void svn_wc_notify_func2_t(IntPtr baton, IntPtr notify, 
													 IntPtr pool);
		#endregion
        											
        #region SvnConfig
	    [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_config_ensure(IntPtr config_dir, IntPtr pool);
        
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr	svn_config_get_config(out IntPtr cfg_hash, IntPtr config_dir, IntPtr pool);
        #endregion
        
        #region SvnAuthProvider
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_auth_simple_prompt_func_t(out IntPtr cred, IntPtr baton, 
        	 												   IntPtr realm, IntPtr username, 
        													   int may_save, IntPtr pool);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_auth_username_prompt_func_t(out IntPtr cred, IntPtr baton, 
															     IntPtr realm, int may_save, 
															     IntPtr pool);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_auth_ssl_server_trust_prompt_func_t(out IntPtr cred, IntPtr baton, 
																	     IntPtr realm, uint failures, 
																	     IntPtr cert_info, 
																	     int may_save, IntPtr pool);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_auth_ssl_client_cert_prompt_func_t(out IntPtr cred, IntPtr baton,
																	    IntPtr realm, int may_save,
																	    IntPtr pool);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_auth_ssl_client_cert_pw_prompt_func_t(out IntPtr cred, 
																		   IntPtr baton,
																  		   IntPtr realm, int may_save,
																  		   IntPtr pool);
																  		 
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_simple_prompt_provider(out IntPtr provider, 
        											svn_auth_simple_prompt_func_t prompt_func, 
        											IntPtr prompt_baton, int retry_limit, IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_username_prompt_provider(out IntPtr provider,
        										svn_auth_username_prompt_func_t prompt_func,
        										IntPtr prompt_baton, int retry_limit, IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_simple_provider(out IntPtr provider,
        										     IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_username_provider(out IntPtr provider,
        											   IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_ssl_server_trust_file_provider(out IntPtr provider, 
        															IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_ssl_client_cert_file_provider(out IntPtr provider,
        														   IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_ssl_client_cert_pw_file_provider(out IntPtr provider, 
        															  IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_ssl_server_trust_prompt_provider(out IntPtr provider, 
        										svn_auth_ssl_server_trust_prompt_func_t prompt_func,
        										IntPtr prompt_baton, IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_ssl_client_cert_prompt_provider(out IntPtr provider,
        										svn_auth_ssl_client_cert_prompt_func_t prompt_func,
        										IntPtr prompt_baton, int retry_limit, IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_client_get_ssl_client_cert_pw_prompt_provider(out IntPtr provider,
        										svn_auth_ssl_client_cert_pw_prompt_func_t prompt_func,
        										IntPtr prompt_baton, int retry_limit, IntPtr pool);
        #endregion
        
        #region SvnAuth
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_auth_open(out IntPtr auth_baton, IntPtr providers, IntPtr pool);
        
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal void svn_auth_set_parameter(IntPtr auth_baton, IntPtr name, IntPtr value);
        
        [DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
        internal IntPtr svn_auth_get_parameter(IntPtr auth_baton, IntPtr name);
		#endregion

        #region SvnClient
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_client_get_commit_log_t(out IntPtr log_message, 
        													 out IntPtr tmp_file, 
        													 IntPtr commit_items, IntPtr baton,
        													 IntPtr pool);
		[System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		internal delegate IntPtr svn_client_get_commit_log2_t(out IntPtr log_message,
															 out IntPtr tmp_file,
															 IntPtr commit_items, IntPtr baton,
															 IntPtr pool);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate void svn_wc_status_func_t(IntPtr baton, IntPtr path, IntPtr status);
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate void svn_wc_status_func2_t(IntPtr baton, IntPtr path, IntPtr status);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_log_message_receiver_t(IntPtr baton, IntPtr changed_paths, 
 															int revision, IntPtr author, 
 															IntPtr date, IntPtr message, 
 															IntPtr pool);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_client_blame_receiver_t(IntPtr baton, long line_no, 
 															 int revision, IntPtr author, 
 															 IntPtr date, IntPtr line, 
 															 IntPtr pool);
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        internal delegate IntPtr svn_info_receiver_t(IntPtr baton, IntPtr path, 
 															 IntPtr info, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)] static extern 
		internal IntPtr svn_client_version(); 

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
        [DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_checkout(out int result_rev, IntPtr URL, 
											IntPtr path, IntPtr revision, int recurse, 
											IntPtr ctx, IntPtr pool);
        [DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_checkout2(out int result_rev, IntPtr URL,
											IntPtr path, IntPtr peg_revision, IntPtr revision, 
											int recurse, int ignore_externals,
											IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_update (out int result_rev, IntPtr path, 
										   IntPtr revision, int recurse,
										   IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_update2 (out IntPtr result_revs, IntPtr paths, 
										   IntPtr revision, int recurse, int ignore_externals,
										   IntPtr ctx, IntPtr pool);
		
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_switch(out int result_rev, IntPtr path, IntPtr url, 
										  IntPtr revision, int recurse, 
										  IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.0 API.")]
		[DllImport(SvnClientLibrary, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_add(IntPtr path, int recursive, 
									   IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_add3(IntPtr path, int recursive, int force, int no_ignore, 
									   IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_mkdir(out IntPtr commit_info, IntPtr paths,
						  	             IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_mkdir2(out IntPtr commit_info, IntPtr paths,
						  	             IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_delete(out IntPtr commit_info, IntPtr paths, int force, 
						  	              IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_delete2(out IntPtr commit_info, IntPtr paths, int force, 
						  	              IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_import(out IntPtr commit_info, 
										  IntPtr path, IntPtr url, int nonrecursive, 
						  	              IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_import2(out IntPtr commit_info, 
										  IntPtr path, IntPtr url, int nonrecursive, int no_ignore,
						  	              IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CallingConvention = CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_commit(out IntPtr commit_info, 
										  IntPtr targets, int nonrecursive,
						  	              IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_commit3(out IntPtr commit_info,
											IntPtr targets, int recurse, int keep_locks,
						  					IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_status(out int result_rev, 
										  IntPtr path, IntPtr revision,
										  svn_wc_status_func_t status_func, IntPtr status_baton, 
										  int descend, int get_all, int update, int no_ignore, 
						  	              IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_status2(out int result_rev, 
											IntPtr path, IntPtr revision,
											svn_wc_status_func2_t status_func, IntPtr status_baton, 
											int recurse, int get_all, int update, int no_ignore, int ignore_externals,
											IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.0 API.")]
		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_log(IntPtr targets, IntPtr start, IntPtr end,
									   int discover_changed_paths, int strict_node_history, 
									   svn_log_message_receiver_t receiver, IntPtr receiver_baton, 
									   IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_log2(IntPtr targets, IntPtr start, IntPtr end,
									   int limit,	
									   int discover_changed_paths, int strict_node_history, 
									   svn_log_message_receiver_t receiver, IntPtr receiver_baton, 
									   IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_blame(IntPtr path_or_url, IntPtr start, IntPtr end, 
										 svn_client_blame_receiver_t receiver, IntPtr receiver_baton, 
										 IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_blame2(IntPtr path_or_url, IntPtr peg_revision, IntPtr start, IntPtr end, 
										 svn_client_blame_receiver_t receiver, IntPtr receiver_baton, 
										 IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.0 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_diff(IntPtr diff_options, 
										IntPtr path1, IntPtr revision1, 
										IntPtr path2, IntPtr revision2, 
										int recurse, int ignore_ancestry, int no_diff_deleted, 
										IntPtr outfile, IntPtr errfile, 
										IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_diff2(IntPtr diff_options, 
										IntPtr path1, IntPtr revision1, 
										IntPtr path2, IntPtr revision2, 
										int recurse, int ignore_ancestry, int no_diff_deleted, int ignore_content_type, 
										IntPtr outfile, IntPtr errfile, 
										IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_diff3(IntPtr diff_options, 
										IntPtr path1, IntPtr revision1, 
										IntPtr path2, IntPtr revision2, 
										int recurse, int ignore_ancestry, int no_diff_deleted,
										int ignore_content_type, IntPtr header_encoding,
										IntPtr outfile, IntPtr errfile, 
										IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_diff_peg(IntPtr diff_options, 
										IntPtr path, 
										IntPtr peg_revision, 
										IntPtr start_revision,  
										IntPtr end_revision, 
										int recurse, int ignore_ancestry, int no_diff_deleted,
										IntPtr outfile, IntPtr errfile, 
										IntPtr ctx, IntPtr pool);
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_diff_peg2(IntPtr diff_options, 
										IntPtr path, 
										IntPtr peg_revision, 
										IntPtr start_revision,  
										IntPtr end_revision, 
										int recurse, int ignore_ancestry, int no_diff_deleted, int ignore_content_type, 
										IntPtr outfile, IntPtr errfile, 
										IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_diff_peg3(IntPtr diff_options, 
										IntPtr path, 
										IntPtr peg_revision, 
										IntPtr start_revision,  
										IntPtr end_revision, 
										int recurse, int ignore_ancestry, int no_diff_deleted, 
										int ignore_content_type, IntPtr header_encoding,
										IntPtr outfile, IntPtr errfile, 
										IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_merge(IntPtr source1, IntPtr revision1, 
										 IntPtr source2, IntPtr revision2,
										 IntPtr target_wcpath, 
										 int recurse, int ignore_ancestry, int force, int dry_run, 
										 IntPtr ctx, IntPtr pool);
		
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_merge_peg(IntPtr source, 
										 IntPtr revision1, 
										 IntPtr revision2,
										 IntPtr peg_revision,
										 IntPtr target_wcpath, 
										 int recurse, int ignore_ancestry, int force, int dry_run, 
										 IntPtr ctx, IntPtr pool);
		
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_cleanup(IntPtr dir, IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_relocate(IntPtr dir, 
											IntPtr from, IntPtr to, int recurse, 
											IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_revert(IntPtr paths, int recursive, IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_resolved(IntPtr path, int recursive, IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_copy(out IntPtr commit_info, 
										IntPtr src_path, IntPtr src_revision, 
										IntPtr dst_path, 
										IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_copy2(out IntPtr commit_info, 
										IntPtr src_path, IntPtr src_revision, 
										IntPtr dst_path, 
										IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_move(out IntPtr commit_info, 
										IntPtr src_path, IntPtr src_revision, 
										IntPtr dst_path, int force, 
										IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_move3(out IntPtr commit_info, 
										IntPtr src_path, 
										IntPtr dst_path, int force, 
										IntPtr ctx, IntPtr pool);
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_propset(string propname, IntPtr propval, IntPtr target, 
										   int recurse,
										   IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_propset2(string propname, IntPtr propval, IntPtr target, 
										   int recurse, int skip_checks,
										   IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_revprop_set(string propname, IntPtr propval, 
											   IntPtr Url, IntPtr revision, 
											   out int set_rev, int force, 
											   IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_propget(out IntPtr props, string propname, IntPtr target, 
										   IntPtr revision, int recurse, 
										   IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_propget2(out IntPtr props, string propname, IntPtr target, 
										   IntPtr peg_revision, IntPtr revision, int recurse, 
										   IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_revprop_get(string propname, out IntPtr propval, 
											   IntPtr URL, IntPtr revision, out int set_rev, 
											   IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_proplist(out IntPtr props, 
											IntPtr target, IntPtr revision, int recurse, 
											IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_proplist2(out IntPtr props, 
											IntPtr target, IntPtr peg_revision, IntPtr revision, int recurse, 
											IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_revprop_list(out IntPtr props,
												IntPtr URL, IntPtr revision, out int set_rev, 
												IntPtr ctx, IntPtr pool);

		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_export(out int result_rev, 
										  IntPtr from, IntPtr to, IntPtr revision, int force, 
										  IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_export3(out int result_rev, 
										  IntPtr from, IntPtr to, IntPtr peg_revision, IntPtr revision, int overwrite, 
										  int ignore_externals, int recurse, IntPtr native_eol,
										  IntPtr ctx, IntPtr pool);
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_ls(out IntPtr dirents, 
									  IntPtr path_or_url, IntPtr revision, int recurse, 
									  IntPtr ctx, IntPtr pool);
		[Obsolete("Provided for backward compatibility with the 1.2 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_ls2(out IntPtr dirents, 
									  IntPtr path_or_url, IntPtr peg_revision, IntPtr revision, int recurse, 
									  IntPtr ctx, IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_ls3(out IntPtr dirents, out IntPtr locks, 
									  IntPtr path_or_url, IntPtr peg_revision, IntPtr revision, int recurse, 
									  IntPtr ctx, IntPtr pool);
		[Obsolete("Provided for backward compatibility with the 1.1 API.")]
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_cat(IntPtr output, 
									   IntPtr path_or_url, IntPtr revision,
									   IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_cat2(IntPtr output, 
									   IntPtr path_or_url, 
									   IntPtr peg_revision,	IntPtr revision,
									   IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_lock(IntPtr targets, IntPtr comment, int steal_lock, IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_unlock(IntPtr targets, int break_lock, IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_info(IntPtr pathOrUrl, IntPtr peg_revision, IntPtr revision, 
										svn_info_receiver_t receiver, IntPtr receiver_baton,
										int recurse, IntPtr ctx, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_url_from_path(out IntPtr url, IntPtr path_or_url, IntPtr pool);

		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_client_uuid_from_url(out IntPtr uuid, IntPtr url, IntPtr ctx, IntPtr pool);

		#endregion                

		#region SvnIO
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_io_open_unique_file(out IntPtr aprFile, out IntPtr unique_name_p, IntPtr path, IntPtr suffix, int delete_on_close, IntPtr pool);
		#endregion

		#region SvnLock
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_lock_create(IntPtr pool);
		[DllImport(SvnClientLibrary, CharSet=CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)] static extern
		internal IntPtr svn_lock_dup(IntPtr lck, IntPtr pool);
		#endregion
    }
}   