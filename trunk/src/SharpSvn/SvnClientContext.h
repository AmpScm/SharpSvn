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

#include "AprPool.h"
#include "AprBaton.h"
#include "EventArgs/SvnEventArgs.h"

using namespace System;
using namespace SharpSvn::Implementation;

namespace SharpSvn {
	namespace Security {
		ref class SvnAuthentication;
	};

	using SharpSvn::Security::SvnAuthentication;

	namespace Implementation
	{
		enum class SvnContextState
		{
			Initial,
			ConfigPrepared,
			ConfigLoaded,
			CustomRemoteConfigApplied,
			AuthorizationInitialized,
		};

		[Flags]
		enum class SvnExtendedState
		{
			MimeTypesLoaded = 0x01,
		};

		ref class SvnLibrary;
	}

	ref class SvnErrorEventArgs;
	ref class SvnProcessingEventArgs;
	ref class SvnClientArgs;
	ref class SvnCommitResult;
	ref class SvnClient;

	/// <summary>Subversion Client Context wrapper; base class of objects using client context</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientContext : public SvnBase
	{
		svn_client_ctx_t *_ctx;
		AprPool^ _pool;
		int _authCookie;
		SvnContextState _contextState;
		SvnExtendedState _xState;
		initonly SvnAuthentication^ _authentication;

		static initonly Object^ _plinkLock = gcnew Object();
		static String^ _plinkPath;

	private:
		// For SvnClient and SvnReposClient
		SvnClientArgs^ _currentArgs;
		bool _keepSession;

	internal:
		// Used config path; used for the authentication cache
		String^ _configPath;

	internal:
		property SvnClientArgs^ CurrentCommandArgs
		{
			SvnClientArgs^ get()
			{
				return _currentArgs;
			}
		}

	internal:
		bool _dontLoadMimeFile;
		bool _dontEnablePlink;
		bool _useUserDiff;
		SvnClientContext(AprPool^ pool);
		virtual void HandleClientError(SvnErrorEventArgs^ e);
		virtual void HandleProcessing(SvnProcessingEventArgs^ e);

	public:
		property bool IsCommandRunning
		{
			bool get()
			{
				return (_currentArgs != nullptr);
			}
		}

		property bool IsDisposed
		{
			bool get()
			{
				return !_ctx;
			}
		}

		property bool KeepSession
		{
			bool get()
			{
				return _keepSession;
			}
			void set(bool value)
			{
				_keepSession = value;
			}
		}

	private:
		~SvnClientContext();

	internal:
		property svn_client_ctx_t *CtxHandle
		{
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			svn_client_ctx_t *get();
		}

	private:
		void ApplyCustomRemoteConfig();
		void ApplyCustomSsh();
		void ApplyMimeTypes();
		void ApplyUserDiffConfig();

	public:
		/// <summary>Loads the subversion configuration from the specified path</summary>
		void LoadConfiguration(String^ path);
		/// <summary>Loads the subversion configuration from the specified path and optionally ensures the path is a subversion config dir by creating default files</summary>
		void LoadConfiguration(String^ path, bool ensurePath);
		/// <summary>Loads the standard subversion configuration and ensures the subversion config dir by creating default files</summary>
		void LoadConfigurationDefault();
		/// <summary>Merges configuration from the specified path into the existing configuration</summary>
		void MergeConfiguration(String^ path);

	internal:
		void EnsureState(SvnContextState requiredState);
		void EnsureState(SvnContextState requiredState, SvnExtendedState xState);

		property SvnContextState State
		{
			SvnContextState get()
			{
				return _contextState;
			}
		}

		/// <summary>Gets the path to SharpSvn's plink. The path is encoded to be safe for subversion configuration settings</summary>
		static property String^ PlinkPath
		{
			String^ get();
		}

	public:
		/// <summary>Gets the <see cref="SvnAuthentication" /> instance managing authentication on behalf of this client</summary>
		property SvnAuthentication^ Authentication
		{
			SvnAuthentication^ get()
			{
				return _authentication;
			}
		}

	internal:
		[ThreadStatic]
		static SvnClientContext ^_activeContext;

	private protected:
		// Used as auto-dispose class for setting the _currentArgs property
		ref class ArgsStore sealed
		{
			initonly SvnClientContext^ _client;
			initonly SvnClientContext^ _lastContext;
			initonly svn_wc_context_t *_wc_ctx;
		public:
			ArgsStore(SvnClientContext^ client, SvnClientArgs^ args, AprPool^ pool);
			~ArgsStore();
		};

		// Used as auto-dispose class for setting a temporary wc_ctx
		ref class NoArgsStore sealed
		{
			initonly SvnClientContext^ _client;
			initonly SvnClientContext^ _lastContext;
			initonly svn_wc_context_t *_wc_ctx;
		public:
			NoArgsStore(SvnClientContext^ client, AprPool^ pool);
			~NoArgsStore();
		};

	internal:
		ref class CommitResultReceiver sealed
		{
			SvnCommitResult^ _commitResult;
			svn_commit_callback2_t _callback;
			AprBaton<CommitResultReceiver^>^ _commitBaton;
			SvnClientContext ^_client;

		public:
			CommitResultReceiver(SvnClientContext^ client);
			~CommitResultReceiver();

		internal:
			property svn_commit_callback2_t CommitCallback
			{
				svn_commit_callback2_t get()
				{
					return _callback;
				}
			}

			property void *CommitBaton
			{
				void* get()
				{
					return (void*)_commitBaton->Handle;
				}
			}

			void ProvideCommitResult(const svn_commit_info_t *commit_info, AprPool^ pool);

		public:
			property SvnCommitResult^ CommitResult
			{
				SvnCommitResult^ get()
				{
					return _commitResult;
				}
			}
		};

	internal:
		generic<typename T>
		where T : SvnEventArgs
		ref class InfoItemCollection : public System::Collections::ObjectModel::Collection<T>
		{
		public:
			InfoItemCollection()
			{}

		internal:
			void HandleItem(Object^ sender, T e)
			{
				UNUSED_ALWAYS(sender);
				e->Detach();
				Add(e);
			}

			property EventHandler<T>^ Handler
			{
				EventHandler<T>^ get()
				{
					return gcnew EventHandler<T>(this, &InfoItemCollection<T>::HandleItem);
				}
			}
		};

		generic<typename T>
		where T : SvnEventArgs
		ref class SingleInfoItemReceiver
		{
			T _value;

		public:
			property T Value
			{
				T get()
				{
					return _value;
				}
			}

		internal:
			void HandleItem(Object^ sender, T e)
			{
				UNUSED_ALWAYS(sender);
				e->Detach();
				_value = e;
			}

			property EventHandler<T>^ Handler
			{
				EventHandler<T>^ get()
				{
					return gcnew EventHandler<T>(this, &SingleInfoItemReceiver<T>::HandleItem);
				}
			}
		};
	};

	public ref class SvnCommandResult abstract
	{
	internal:
		SvnCommandResult()
		{}
	};

}
