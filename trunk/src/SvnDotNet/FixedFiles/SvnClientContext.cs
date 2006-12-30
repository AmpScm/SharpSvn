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
using System.Runtime.InteropServices;

namespace Softec.SubversionSharp
{
	/// <summary>
	/// A client context structure, which holds client specific callbacks, 
	/// batons, serves as a cache for configuration options, and other various 
	/// and sundry things.
	/// </summary>
    public unsafe class SvnClientContext : IAprUnmanaged
    {
        private svn_client_ctx_t *mClientContext;
        private SvnAuthBaton mAuthBaton;
        private SvnDelegate mNotifyFunc;
		private SvnDelegate mNotifyFunc2;
		private SvnDelegate mLogMsgFunc;
		private SvnDelegate mLogMsgFunc2;
        private SvnDelegate mCancelFunc;
		private SvnDelegate mProgressFunc;

        [StructLayout( LayoutKind.Sequential, Pack=4 )]
        private struct svn_client_ctx_t
        {  
			public IntPtr auth_baton;
			[Obsolete("Provided for backward compatibility with the 1.1 API")]
			public IntPtr notify_func;
			[Obsolete("Provided for backward compatibility with the 1.1 API")]
			public IntPtr notify_baton;
			[Obsolete("Provided for backward compatibility with the 1.2 API")]
			public IntPtr log_msg_func;
			[Obsolete("Provided for backward compatibility with the 1.2 API")]
			public IntPtr log_msg_baton;
			public IntPtr config;
			public IntPtr cancel_func;
			public IntPtr cancel_baton;
			public IntPtr notify_func2;
			public IntPtr notify_baton2;
			public IntPtr log_msg_func2;
			public IntPtr log_msg_baton2;
			public IntPtr progress_func;
			public IntPtr progress_baton;
        }

        #region Generic embedding functions of an IntPtr
        private SvnClientContext(svn_client_ctx_t *ptr)
        {
            mClientContext = ptr;
            mAuthBaton = new SvnAuthBaton();
        	mNotifyFunc = SvnDelegate.NullFunc;
			mNotifyFunc2 = SvnDelegate.NullFunc;
			mLogMsgFunc = SvnDelegate.NullFunc;
       		mCancelFunc = SvnDelegate.NullFunc;
        }
        
        public SvnClientContext(IntPtr ptr)
        {
            mClientContext = (svn_client_ctx_t *) ptr.ToPointer();
            mAuthBaton = new SvnAuthBaton();
        	mNotifyFunc = SvnDelegate.NullFunc;
			mNotifyFunc2 = SvnDelegate.NullFunc;
			mLogMsgFunc = SvnDelegate.NullFunc;
       		mCancelFunc = SvnDelegate.NullFunc;
        }
        
        public bool IsNull
        {
        	get
        	{
            	return( mClientContext == null );
            }
        }

        private void CheckPtr()
        {
            if( IsNull )
                throw new AprNullReferenceException(); 
        }

        public void ClearPtr()
        {
            mClientContext = null;
        }

        public IntPtr ToIntPtr()
        {
            return new IntPtr(mClientContext);
        }
        
		public bool ReferenceEquals(IAprUnmanaged obj)
		{
			return(obj.ToIntPtr() == ToIntPtr());
		}
		
        public static implicit operator IntPtr(SvnClientContext clientContext)
        {
            return new IntPtr(clientContext.mClientContext);
        }
        
        public static implicit operator SvnClientContext(IntPtr ptr)
        {
            return new SvnClientContext(ptr);
        }

        public override string ToString()
        {
            return("[svn_client_context_t:"+(new IntPtr(mClientContext)).ToInt32().ToString("X")+"]");
        }
        #endregion

        #region Wrapper methods
        public static SvnClientContext Create(AprPool pool)
        {
        	IntPtr ptr;
            
            Debug.Write(String.Format("svn_client_create_context({0})...",pool));
            SvnError err = Svn.svn_client_create_context(out ptr, pool);
            if(!err.IsNoError)
                throw new SvnException(err);
            Debug.WriteLine(String.Format("Done({0:X})",((Int32)ptr)));

            return(ptr);
        }
        #endregion

        #region Wrapper Properties
		/// <summary>
		/// main authentication baton.
		/// </summary>
        public SvnAuthBaton AuthBaton
		{
			get
			{
				CheckPtr();
				return(mAuthBaton);
			}
			set
			{
				CheckPtr();
				mAuthBaton = value;
				mClientContext->auth_baton = mAuthBaton;
			}
		}
		/// <summary>
		/// notification callback function.
		/// </summary>
		[Obsolete("Provided for backward compatibility with the 1.1 API")]
		public SvnDelegate NotifyFunc
		{
			get
			{
				CheckPtr();
				return(mNotifyFunc);
			}
			set
			{
				CheckPtr();
				mNotifyFunc = value;
				mClientContext->notify_func = mNotifyFunc.WrapperPtr;
			}
		}
		/// <summary>
		/// notification callback baton
		/// </summary>
		[Obsolete("Provided for backward compatibility with the 1.1 API")]
		public IntPtr NotifyBaton
		{
			get
			{
				CheckPtr();
				return(mClientContext->notify_baton);

			}
			set
			{
				CheckPtr();
				mClientContext->notify_baton = value;
			}
		}
		/// <summary>
		/// Log message callback function.  NULL means that Subversion
		/// should try not attempt to fetch a log message.
		/// </summary>
		[Obsolete("Provided for backward compatibility with the 1.2 API")]
		public SvnDelegate LogMsgFunc
		{
			get
			{
				CheckPtr();
				return(mLogMsgFunc);
			}
			set
			{
				CheckPtr();
				mLogMsgFunc = value;
				mClientContext->log_msg_func = mLogMsgFunc.WrapperPtr;
			}
		}
		/// <summary>
		/// log message callback baton
		/// </summary>
		[Obsolete("Provided for backward compatibility with the 1.2 API")]
		public IntPtr LogMsgBaton
		{
			get
			{
				CheckPtr();
				return(mClientContext->log_msg_baton);

			}
			set
			{
				CheckPtr();
				mClientContext->log_msg_baton = value;
			}
		}
		/// <summary>
		/// hash mapping of configuration file names to SvnConfig's.
		/// For example, the '~/.subversion/config' file's contents should have
		/// the key "config".
		/// May be left unset (or set to NULL) to use the built-in default settings
		/// and not use any configuration.
		/// </summary>
        public AprHash Config
		{
			get
			{
				CheckPtr();
				return(mClientContext->config);
			}
			set
			{
				CheckPtr();
				mClientContext->config = value;
			}
		}
        
		/// <summary>
		/// a callback to be used to see if the client wishes to cancel the running operation.
		/// </summary>
        public SvnDelegate CancelFunc
		{
			get
			{
				CheckPtr();
				return(mCancelFunc);
			}
			set
			{
				CheckPtr();
				mCancelFunc = value;
				mClientContext->cancel_func = mCancelFunc.WrapperPtr;
			}
		}

		/// <summary>
		/// a baton to pass to the cancellation callback.
		/// </summary>
		public IntPtr CancelBaton
		{
			get
			{
				CheckPtr();
				return(mClientContext->cancel_baton);

			}
			set
			{
				CheckPtr();
				mClientContext->cancel_baton = value;
			}
		}

		/// <summary>
		/// notification callback function.
		/// </summary>
		public SvnDelegate NotifyFunc2
		{
			get
			{
				CheckPtr();
				return (mNotifyFunc2);
			}
			set
			{
				CheckPtr();
				mNotifyFunc2 = value;
				mClientContext->notify_func2 = mNotifyFunc2.WrapperPtr;
			}
		}
		/// <summary>
		/// notification callback baton for NotifyFunc2
		/// </summary>
		public IntPtr NotifyBaton2
		{
			get
			{
				CheckPtr();
				return (mClientContext->notify_baton2);

			}
			set
			{
				CheckPtr();
				mClientContext->notify_baton2 = value;
			}
		}

		/// <summary>
		/// Log message callback function.  NULL means that Subversion
		/// should try not attempt to fetch a log message.
		/// </summary>
		public SvnDelegate LogMsgFunc2
		{
			get
			{
				CheckPtr();
				return (mLogMsgFunc2);
			}
			set
			{
				CheckPtr();
				mLogMsgFunc2 = value;
				mClientContext->log_msg_func2 = mLogMsgFunc2.WrapperPtr;
			}
		}

		/// <summary>
		/// log message callback baton
		/// </summary>
		public IntPtr LogMsgBaton2
		{
			get
			{
				CheckPtr();
				return (mClientContext->log_msg_baton2);

			}
			set
			{
				CheckPtr();
				mClientContext->log_msg_baton2 = value;
			}
		}

		/// <summary>
		/// Notification callback for network progress information.
		/// May be NULL if not used.
		/// </summary>
		public SvnDelegate ProgressFunc
		{
			get
			{
				CheckPtr();
				return (mProgressFunc);
			}
			set
			{
				CheckPtr();
				mProgressFunc = value;
				mClientContext->progress_func = mProgressFunc.WrapperPtr;
			}
		}
		/// <summary>
		/// Callback baton for ProgressFunc
		/// </summary>
		public IntPtr ProgressBaton
		{
			get
			{
				CheckPtr();
				return (mClientContext->progress_baton);

			}
			set
			{
				CheckPtr();
				mClientContext->progress_baton = value;
			}
		}

		#endregion
    }
}