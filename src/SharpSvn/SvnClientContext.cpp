// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"
#include "UnmanagedStructs.h"
#include <svn_config.h>

using namespace SharpSvn;
using namespace System::Threading;
using namespace Microsoft::Win32;
using System::IO::Path;

SvnClientContext::SvnClientContext(AprPool ^pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	_pool = pool;
	svn_client_ctx_t *ctx;

	svn_error_t *r = svn_client_create_context(&ctx, pool->Handle);

	if (r)
		throw SvnException::Create(r);

	ctx->client_name = pool->AllocString(SvnBase::_clientName);

	_ctx = ctx;
	_authentication = gcnew SharpSvn::Security::SvnAuthentication(this, pool);
}

SvnClientContext::SvnClientContext(SvnClientContext ^fromContext)
{
	if (!fromContext)
		throw gcnew ArgumentNullException("fromContext");

	fromContext->_pool->Ensure();
	_pool = fromContext->_pool;

	svn_client_ctx_t *ctx;

	svn_error_t *r = svn_client_create_context(&ctx, _pool->Handle);

	if (r)
		throw SvnException::Create(r);

	_ctx = ctx;
	_authentication = gcnew SharpSvn::Security::SvnAuthentication(this, _pool);
}

SvnClientContext::~SvnClientContext()
{
	_ctx = nullptr;
	_pool = nullptr;
}

svn_client_ctx_t *SvnClientContext::CtxHandle::get()
{
	if (!_ctx)
		throw gcnew ObjectDisposedException("SvnClientContext");

	_pool->Ensure();

	return _ctx;
}

void SvnClientContext::HandleClientError(SvnErrorEventArgs^ e)
{
	UNUSED_ALWAYS(e);
	/* NOOP at SvnClientContext level */
}

void SvnClientContext::HandleProcessing(SvnProcessingEventArgs^ e)
{
	UNUSED_ALWAYS(e);
	/* NOOP at SvnClientContext level */
}

void SvnClientContext::EnsureState(SvnContextState requiredState)
{
	if (!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	if (requiredState < State)
		return;

	if (State < SvnContextState::ConfigLoaded && requiredState >= SvnContextState::ConfigLoaded)
	{
		LoadConfigurationDefault();

		System::Diagnostics::Debug::Assert(State == SvnContextState::ConfigLoaded);
	}

	if (requiredState >= SvnContextState::CustomRemoteConfigApplied && State < SvnContextState::CustomRemoteConfigApplied)
	{
		ApplyCustomRemoteConfig();

		System::Diagnostics::Debug::Assert(State == SvnContextState::CustomRemoteConfigApplied);
	}

	if (requiredState >= SvnContextState::AuthorizationInitialized)
	{
		if (State < SvnContextState::AuthorizationInitialized)
		{
			int authCookie;

			CtxHandle->auth_baton = Authentication->GetAuthorizationBaton(authCookie);
			_authCookie = authCookie;

			_contextState = SvnContextState::AuthorizationInitialized;
		}
		else
		{
			if (_authCookie != Authentication->Cookie)
			{
				// Authenticator configuration has changed; reload the baton and its backend

				_contextState = SvnContextState::ConfigLoaded;
				CtxHandle->auth_baton = nullptr;

				int authCookie;

				CtxHandle->auth_baton = Authentication->GetAuthorizationBaton(authCookie);
				_authCookie = authCookie;

				_contextState = SvnContextState::AuthorizationInitialized;
			}
		}

		// TODO: Initialize Plink for ssh sessions?
	}
}

void SvnClientContext::EnsureState(SvnContextState requiredState, SvnExtendedState xState)
{
	EnsureState(requiredState);

	if (0 == (int)(xState & ~_xState))
		return; // No changes to apply

	if (0 != (int)((xState & ~_xState) & SvnExtendedState::MimeTypesLoaded))
		ApplyMimeTypes();
}

void SvnClientContext::ApplyMimeTypes()
{
	if (0 != (int)(_xState & SvnExtendedState::MimeTypesLoaded))
		return;

	_xState = _xState | SvnExtendedState::MimeTypesLoaded;

	// The following code matches those in subversion/subversion/main.c
	// Note: We allocate everything in the context pool

	svn_config_t *cfg = (svn_config_t*)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

	const char *mimetypes_file;

	svn_config_get(cfg, &mimetypes_file,
		SVN_CONFIG_SECTION_MISCELLANY,
		SVN_CONFIG_OPTION_MIMETYPES_FILE, FALSE);

	if (mimetypes_file && *mimetypes_file)
	{
		svn_error_t* err;

		if ((err = svn_io_parse_mimetypes_file(&(CtxHandle->mimetypes_map), mimetypes_file, _pool->Handle)))
		{
			SvnClientConfigurationException^ exception =
				gcnew SvnClientConfigurationException(SharpSvnStrings::LoadingMimeTypesMapFileFailed, SvnException::Create(err));

			SvnErrorEventArgs^ ee = gcnew SvnErrorEventArgs(exception);

			HandleClientError(ee);

			if (!ee->Cancel)
				throw exception;
		}
	}
}

void SvnClientContext::ApplyCustomRemoteConfig()
{
	// Look for a custom SSH client; Windows most common SVN client uses their own setting, so we should to :(
	ApplyCustomSsh();

	_contextState = SvnContextState::CustomRemoteConfigApplied;
}


void SvnClientContext::ApplyCustomSsh()
{
	svn_config_t *cfg = CtxHandle->config ? (svn_config_t *)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING) : nullptr;

	if (!cfg)
		return;

	String^ customSshConfig;

	try
	{
		// Allow overriding the TortoiseSVN setting at our own level.
		// Probably never used, but allow overriding Tortoise settings anyway
		customSshConfig = dynamic_cast<String^>(Registry::CurrentUser->GetValue("Software\\QQn\\SharpSvn\\CurrentVersion\\Handlers\\SSH", nullptr));

		if (!customSshConfig)
			customSshConfig = dynamic_cast<String^>(Registry::LocalMachine->GetValue("Software\\QQn\\SharpSvn\\CurrentVersion\\Handlers\\SSH", nullptr));
	}
	catch (System::Security::SecurityException^) // Exceptions should never happen. CurrentUser is written by Current User
	{ customSshConfig = nullptr; }
	catch (UnauthorizedAccessException^)
	{ customSshConfig = nullptr; }

	if (!customSshConfig)
	{
		try
		{
			// Use the TortoiseSVN setting
			customSshConfig = dynamic_cast<String^>(Registry::CurrentUser->GetValue("Software\\TortoiseSVN\\SSH", nullptr));
		}
		catch (System::Security::SecurityException^) // Exceptions should never happen. CurrentUser is written by Current User
		{ customSshConfig = nullptr; }
		catch (UnauthorizedAccessException^)
		{ customSshConfig = nullptr; }
	}

	if (customSshConfig)
	{
		// allocate in Ctx pool!
		svn_config_set(cfg, SVN_CONFIG_SECTION_TUNNELS, "ssh", _pool->AllocString(customSshConfig->Replace('\\', '/')));
		return;
	}

	if (_dontEnablePlink)
		return;

	AprPool pool(_pool);

	// Get the setting from the subversion config, to make sure it makes any sense (The default is "", which we must always replace)

	const char* cmd = nullptr;
	const char* val = nullptr;

	if (!cfg)
		return;

	svn_config_get(cfg, &val, SVN_CONFIG_SECTION_TUNNELS, "ssh", "$SVN_SSH ssh");

	if (val && val[0] == '$')
	{
		// svn/client.c: If the scheme definition begins with "$varname", it means there is an
		//                   environment variable which can override the command.

		val++;
		int len = (int)strcspn(val, " ");
		char* var = apr_pstrmemdup(pool.Handle, val, len);
		cmd = getenv(var);

		if (!cmd)
			val += len;
	}

	if (!cmd && val)
	{
		char** argv = nullptr;
		if (!apr_tokenize_to_argv(val, &argv, pool.Handle) && argv && argv[0])
			cmd = argv[0];
	}

	if (cmd)
	{
		String^ sCmd = Utf8_PtrToString(cmd); // We have an utf8 encoded string and like to use the unicode windows api
		wchar_t* buffer = (wchar_t*)apr_pcalloc(pool.Handle, 1024 * sizeof(wchar_t));
		wchar_t* app = (wchar_t*)pool.Alloc((sCmd->Length+1) * sizeof(wchar_t));
		wchar_t* pFile = nullptr;

		for (int i = 0; i < sCmd->Length; i++)
			app[i] = sCmd[i];

		app[sCmd->Length] = 0;

		if (SearchPathW(nullptr, app, L".exe", 1000, buffer, &pFile) && pFile)
		{
			return; // The specified executable exists. Use it!
		}
	}
	// Ok: registry unset, setting invalid. Let's set our own plink handler

	String^ plinkPath = PlinkPath;

	if (plinkPath)
	{
		// Allocate in Ctx pool
		svn_config_set(cfg, SVN_CONFIG_SECTION_TUNNELS, "ssh", _pool->AllocString(plinkPath));
	}
}

String^ SvnClientContext::PlinkPath::get()
{
	Monitor::Enter(_plinkLock);
	try
	{
		if (!_plinkPath)
		{
			_plinkPath = "";
			Uri^ codeBase;

			if (Uri::TryCreate(SvnClientContext::typeid->Assembly->CodeBase, UriKind::Absolute, codeBase) && (codeBase->IsUnc || codeBase->IsFile))
			{
				String^ path = SvnTools::GetNormalizedFullPath(codeBase->LocalPath);

				path = Path::Combine(Path::GetDirectoryName(path), "SharpPlink-" APR_STRINGIFY(SHARPSVN_PLATFORM_SUFFIX) ".svnExe");

				if (System::IO::File::Exists(path))
				{
					_plinkPath = path->Replace(System::IO::Path::DirectorySeparatorChar, '/');

					if (_plinkPath->Contains(" "))
						_plinkPath = "\"" + _plinkPath + "\"";
				}
			}
		}

		return String::IsNullOrEmpty(_plinkPath) ? nullptr : _plinkPath;
	}
	finally
	{
		Monitor::Exit(_plinkLock);
	}
}

void SvnClientContext::LoadConfiguration(String ^path, bool ensurePath)
{
	if (State >= SvnContextState::ConfigLoaded)
		throw gcnew InvalidOperationException("Configuration already loaded");

	if (String::IsNullOrEmpty(path))
		path = nullptr;

	AprPool tmpPool(_pool);
	const char* szPath = path ? tmpPool.AllocString(path) : nullptr;

	svn_error_t* err = nullptr;

	if (ensurePath)
	{
		err = svn_config_ensure(szPath, tmpPool.Handle);

		if (err)
			throw SvnException::Create(err);
	}

	apr_hash_t* cfg = nullptr;
	err = svn_config_get_config(&cfg, szPath, _pool->Handle);

	if (err)
		throw SvnException::Create(err);

	CtxHandle->config = cfg;

	_contextState = SvnContextState::ConfigLoaded;
}

void SvnClientContext::LoadConfiguration(String ^path)
{
	LoadConfiguration(path, false);
}

void SvnClientContext::LoadConfigurationDefault()
{
	LoadConfiguration(nullptr, true);
}

void SvnClientContext::MergeConfiguration(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	if (State < SvnContextState::ConfigLoaded)
		LoadConfigurationDefault();

	AprPool tmpPool(_pool);

	const char* szPath = tmpPool.AllocString(path);

	svn_error_t* err = svn_config_get_config(&CtxHandle->config, szPath, _pool->Handle);

	if (err)
		throw SvnException::Create(err);
}

SvnClientContext::ArgsStore::ArgsStore(SvnClientContext^ client, SvnClientArgs^ args)
{
	if (!args)
		throw gcnew ArgumentNullException("args");
	else if (client->_currentArgs)
		throw gcnew InvalidOperationException(SharpSvnStrings::SvnClientOperationInProgress);

	args->Prepare();
	client->_currentArgs = args;
	_client = client;
	try
	{
		client->HandleProcessing(gcnew SvnProcessingEventArgs(args->CommandType));
	}
	catch(Exception^)
	{
		client->_currentArgs = nullptr;
		throw;
	}
}
