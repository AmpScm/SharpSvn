// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "AprPool.h"

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
	}

	ref class SvnErrorEventArgs;

	/// <summary>Subversion Client Context wrapper; base class of objects using client context</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientContext : public SvnBase
	{
		svn_client_ctx_t *_ctx;
		AprPool^ _pool;
		AprPool^ _authPool;
		int _authCookie;
		SvnContextState _contextState;
		SvnExtendedState _xState;
		initonly SvnAuthentication^ _authentication;

		static initonly Object^ _plinkLock = gcnew Object();
		static String^ _plinkPath;

	internal:
		bool _dontLoadMimeFile;
		bool _dontEnablePlink;
		SvnClientContext(AprPool^ pool);
		virtual void HandleClientError(SvnErrorEventArgs^ e);

	public:
		SvnClientContext(SvnClientContext ^fromContext);

	private:
		~SvnClientContext();

	internal:
		property svn_client_ctx_t *CtxHandle
		{
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			svn_client_ctx_t *get();
		}

		virtual void ApplyCustomRemoteConfig();
	private:
		void ApplyCustomSsh();
		void ApplyMimeTypes();

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
		property SvnAuthentication^ Authenticator
		{
			SvnAuthentication^ get()
			{
				return _authentication;
			}
		}
	};

	public ref class SvnCommandResult abstract
	{
	internal:
		SvnCommandResult()
		{}
	};

}
