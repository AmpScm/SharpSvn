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
		Object^ _workState;

	internal:
		property SvnClientArgs^ CurrentCommandArgs
		{
			SvnClientArgs^ get()
			{
				return _currentArgs;
			}
		}

		property Object^ WorkState
		{
			Object^ get()
			{
				return _workState;
			}
			void set(Object^ value)
			{
				_workState = value;
			}
		}

	internal:
		bool _dontLoadMimeFile;
		bool _dontEnablePlink;
        bool _useUserDiff;
		bool _neverAutomerge;
		SvnClientContext(AprPool^ pool);
		virtual void HandleClientError(SvnErrorEventArgs^ e);
		virtual void HandleProcessing(SvnProcessingEventArgs^ e);

	public:
		SvnClientContext(SvnClientContext ^fromContext);

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

	private protected:
		// Used as auto-dispose class for setting the _currentArgs property
		ref class ArgsStore sealed
		{
			initonly SvnClientContext^ _client;
		public:
			ArgsStore(SvnClientContext^ client, SvnClientArgs^ args);
			~ArgsStore();
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
