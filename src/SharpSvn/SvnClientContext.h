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

	ref class SvnErrorEventArgs;
	ref class SvnProcessingEventArgs;
	ref class SvnClientArgs;
	ref class SvnCommitResult;
    ref class SvnCommittedEventArgs;
	ref class SvnClient;

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
			None = 0,
			MimeTypesLoaded = 0x01,
			TortoiseSvnHooksLoaded = 0x02,
		};

		ref class SvnLibrary;

		ref class SvnConfigItem sealed
		{
			initonly String ^_file;
			initonly String ^_section;
			initonly String ^_option;
			initonly String ^_value;

		public:
			SvnConfigItem(String^ file, String^ section, String^ option, String^ value)
			{
				if (!file)
					throw gcnew ArgumentNullException("file");
				else if (!section)
					throw gcnew ArgumentNullException("section");
				else if (!option)
					throw gcnew ArgumentNullException("option");
				else if (!value)
					throw gcnew ArgumentNullException("value");

				_file = file;
				_section = section;
				_option = option;
				_value = value;
			}

			property String^ File
			{
				String^ get()
				{
					return _file;
				}
			}

			property String^ Section
			{
				String^ get()
				{
					return _section;
				}
			}

			property String^ Option
			{
				String^ get()
				{
					return _option;
				}
			}

			property String^ Value
			{
				String^ get()
				{
					return _value;
				}
			}
		};

		/* This order matches the ordering of SvnClientHook::_hookTypes */
		enum class SvnClientHookType
		{
			Undefined = -1,
			StartCommit = 0,
			PreCommit,
			PostCommit,
			StartUpdate,
			PreUpdate,
			PostUpdate,
			PreConnect,
		};

		ref class SvnClientHook sealed : IComparable<SvnClientHook^>, IEquatable<SvnClientHook^>, IComparable
		{
			initonly SvnClientHookType _type;
			initonly String^ _dir; 
			initonly String^ _cmd; 
			initonly bool _wait;
			initonly bool _show;

		public:
			SvnClientHook(SvnClientHookType type, String^ dir, String^ cmd, bool wait, bool show)
			{
				if (!dir)
					throw gcnew ArgumentNullException("dir");
				else if (String::IsNullOrEmpty(cmd))
					throw gcnew ArgumentNullException("cmd");

				_type = type;
				_dir = String::IsNullOrEmpty(dir) ? "" : SvnTools::GetNormalizedFullPath(dir);
				_cmd = cmd;
				_wait = wait;
				_show = show;
			}

			property SvnClientHookType Type
			{
				SvnClientHookType get()
				{
					return _type;
				}
			}

			property String^ Path
			{
				String^ get()
				{
					return _dir;
				}
			}

			property String^ Command
			{
				String^ get()
				{
					return _cmd;
				}
			}

			property bool WaitForExit
			{
				bool get()
				{
					return _wait;
				}
			}

			property bool ShowConsole
			{
				bool get()
				{
					return _show;
				}
			}

		private:
		/* This order matches the ordering of SvnClientHookTypes */
			static initonly array<String^>^ _hookTypes = gcnew array<String^> 
			{ 
				"start_commit_hook",
				"pre_commit_hook",
				"post_commit_hook",
				"start_update_hook",
				"pre_update_hook",
				"post_update_hook",
				"pre_connect_hook",
			};

		internal:
			static SvnClientHookType GetHookType(String^ hookString)
			{
				if (!hookString)
					throw gcnew ArgumentNullException("hookString");

				int idx = Array::IndexOf(_hookTypes, hookString);

				if (idx >= 0)
					return (SvnClientHookType)idx;
				else
					return SvnClientHookType::Undefined;
			}

		public:
			virtual int CompareTo(Object^ other)
			{
				return CompareTo(dynamic_cast<SvnClientHook^>(other));
			}

			virtual int CompareTo(SvnClientHook^ other)
			{
				if (other == nullptr)
					return -1;

				int n = Path->CompareTo(other->Path);

				if (n == 0)
					return (int)Type - (int)other->Type;
				else
					return n;
			}

			virtual bool Equals(Object^ other) override
			{
				return Equals(dynamic_cast<SvnClientHook^>(other));
			}

			virtual bool Equals(SvnClientHook^ other)
			{
				if (other == nullptr)
					return false;

				if (!Path->Equals(other->Path))
					return false;

				return Type == other->Type;
			}

			virtual int GetHashCode() override
			{
				return Path->GetHashCode() ^ Type.GetHashCode();
			}

		internal:
			bool Run(SvnClientContext^ ctx, SvnClientArgs^ args, ...array<String^>^ commandArgs);
		};
	}

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
		System::Collections::Generic::List<SvnConfigItem^>^ _configOverrides;
		array<SvnClientHook^>^ _tsvnHooks;

	internal:
		property svn_client_ctx_t *CtxHandle
		{
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			svn_client_ctx_t *get();
		}

		void SetConfigurationOption(String^ file, String^ section, String^ option, String^ value);

	private:
		void ApplyCustomRemoteConfig();
		void ApplyCustomSsh();
		void ApplyMimeTypes();
		void ApplyUserDiffConfig();
		void LoadTortoiseSvnHooks();

	internal:
		bool FindHook(String^ path, SvnClientHookType hookType, [Out] SvnClientHook^% hook);

	public:
		/// <summary>Loads the subversion configuration from the specified path</summary>
		void LoadConfiguration(String^ path);
		/// <summary>Loads the subversion configuration from the specified path and optionally ensures the path is a subversion config dir by creating default files</summary>
		void LoadConfiguration(String^ path, bool ensurePath);
		/// <summary>Loads the standard subversion configuration and ensures the subversion config dir by creating default files</summary>
		void LoadConfigurationDefault();
		/// <summary>Merges configuration from the specified path into the existing configuration</summary>
		void MergeConfiguration(String^ path);

		/// <summary>Initializes a default configuration. Avoids loading a configuration at a later time</summary>
		void UseDefaultConfiguration();

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

        void NopEventHandler(Object^ sender, EventArgs^ e)
        {
            UNUSED_ALWAYS(sender);
            UNUSED_ALWAYS(e);
            GC::KeepAlive(this); // Keep reference online
        }
	};

    public ref class SvnCommandResult abstract : SvnEventArgs
	{
	internal:
		SvnCommandResult()
		{}
	};

}
