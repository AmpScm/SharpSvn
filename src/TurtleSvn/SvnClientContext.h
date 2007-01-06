
#pragma once

#include "AprPool.h"

using namespace System;
using namespace TurtleSvn::Apr;

namespace TurtleSvn {
	namespace Security {
		ref class SvnAuthentication;
	};

	using TurtleSvn::Security::SvnAuthentication;

	public enum class SvnContextState
	{
		Initial,
		ConfigLoaded,
		AuthorizationInitialized
	};

	public ref class SvnClientContext : public SvnHandleBase
	{
		svn_client_ctx_t *_ctx;
		AprPool^ _pool;
		AprPool^ _authPool;
		int _authCookie;
		SvnContextState _contextState;
		initonly SvnAuthentication^ _authentication;
	internal:
		SvnClientContext(AprPool^ pool);

	public:
		SvnClientContext(SvnClientContext ^fromContext);

	private:
		~SvnClientContext();

	internal:
		property svn_client_ctx_t *CtxHandle
		{
			svn_client_ctx_t *get();
		}

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
		void EnsureState(SvnContextState state);

		property SvnContextState State
		{
			SvnContextState get()
			{
				return _contextState;
			}
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
}
