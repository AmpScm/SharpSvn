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

#include "stdafx.h"

#include "SvnAll.h"
#include "SvnNames.h"
#include "UnmanagedStructs.h"
#include <svn_config.h>

using namespace SharpSvn;
using namespace System::Threading;
using namespace Microsoft::Win32;
using namespace System::Diagnostics;
using System::IO::Path;

SvnClientContext::SvnClientContext(AprPool ^pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	_pool = pool;
	svn_client_ctx_t *ctx;

	SVN_THROW(svn_client_create_context(&ctx, pool->Handle));

	ctx->client_name = pool->AllocString(SvnBase::_clientName);

	_ctx = ctx;
	_authentication = gcnew SharpSvn::Security::SvnAuthentication(this, pool);
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

void SvnClientContext::SetConfigurationOption(String^ file, String^ section, String^ option, String^ value)
{
	if (!file)
		throw gcnew ArgumentNullException("file");
	else if (!section)
		throw gcnew ArgumentNullException("section");
	else if (!option)
		throw gcnew ArgumentNullException("option");
	else if (!value)
		throw gcnew ArgumentNullException("value");
	else if (State > SvnContextState::ConfigLoaded)
		throw gcnew InvalidOperationException();

	if (!String::Equals(file, SvnConfigNames::ConfigCategory)
		&& !String::Equals(file, SvnConfigNames::ServersCategory))
	{
		throw gcnew ArgumentOutOfRangeException("file");
	}

	if (!_configOverrides)
		_configOverrides = gcnew List<SvnConfigItem^>();

	_configOverrides->Add(gcnew SvnConfigItem(file, section, option, value));
}


void SvnClientContext::EnsureState(SvnContextState requiredState)
{
	if (!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	if (requiredState < State)
		return;

	if (State < SvnContextState::ConfigPrepared && requiredState >= SvnContextState::ConfigPrepared)
	{
		LoadConfigurationDefault();

		System::Diagnostics::Debug::Assert(State == SvnContextState::ConfigPrepared);
	}

	if (State < SvnContextState::ConfigLoaded && requiredState >= SvnContextState::ConfigLoaded)
	{
		ApplyUserDiffConfig();

		if (_pool && _configOverrides && _configOverrides->Count > 0)
		{
			AprPool sp(_pool);
			for each (SvnConfigItem^ item in _configOverrides)
			{
				svn_config_t *cfg = (svn_config_t*)apr_hash_get(CtxHandle->config, sp.AllocString(item->File), APR_HASH_KEY_STRING);

				if (!cfg)
					continue;

				svn_config_set(cfg, _pool->AllocString(item->Section), _pool->AllocString(item->Option),
					_pool->AllocString(item->Value));
			}
		}

		_contextState = SvnContextState::ConfigLoaded;
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
		System::Diagnostics::Debug::Assert(State == SvnContextState::AuthorizationInitialized);
	}
}

void SvnClientContext::EnsureState(SvnContextState requiredState, SvnExtendedState xState)
{
	EnsureState(requiredState);

	if (0 == (int)(xState & ~_xState))
		return; // No changes to apply

	if (0 != (int)((xState & ~_xState) & SvnExtendedState::MimeTypesLoaded))
		ApplyMimeTypes();

	if (0 != (int)((xState & ~_xState) & SvnExtendedState::TortoiseSvnHooksLoaded))
		LoadTortoiseSvnHooks();
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

void SvnClientContext::LoadTortoiseSvnHooks()
{
	if (0 != (int)(_xState & SvnExtendedState::TortoiseSvnHooksLoaded))
		return;

	_xState = _xState | SvnExtendedState::TortoiseSvnHooksLoaded;

	String^ tsvnVersion = nullptr;
	String^ hookScripts = nullptr;
	RegistryKey ^rk = Registry::CurrentUser->OpenSubKey("Software\\TortoiseSVN", false);
	if (rk != nullptr)
	{
		try
		{
			tsvnVersion = dynamic_cast<String^>(rk->GetValue("CurrentVersion"));
			hookScripts = dynamic_cast<String^>(rk->GetValue("hooks"));
		}
		finally
		{
			delete rk;
		}
	}

	if (!tsvnVersion || !hookScripts)
		return;

	/* Do a simple version check to verify if the hook format didn't change */
	tsvnVersion = tsvnVersion->Replace(" ", "")->Replace(',', '.');
	Version^ tsv = gcnew Version(tsvnVersion);
	Version^ shv = SvnClient::SharpSvnVersion;

	if (tsv->Major != shv->Major)
		return;
	int shMinor = shv->Minor / 1000;
	if (tsv->Minor != shMinor && !(tsv->Minor+1 == shMinor && tsv->Build == 99))
		return;

	array<String^>^ hookVals = hookScripts->Replace("\r","")->Split('\n');

	List<SvnClientHook^>^ hooks = gcnew List<SvnClientHook^>();

	const int hookItems = 5;
	for (int i = 0; i < hookVals->Length; i += hookItems)
	{
		if (i + hookItems >= hookVals->Length)
			break;

		String^ hookType = hookVals[i]->Trim();
		String^ hookDir = hookVals[i+1]->Trim();
		String^ hookCmd = hookVals[i+2]->Trim();
		String^ hookWait = hookVals[i+3]->Trim();
		String^ hookShow = hookVals[i+4]->Trim();

		SvnClientHookType type = SvnClientHook::GetHookType(hookType);

		if (type == SvnClientHookType::Undefined)
			continue;
		else if (String::IsNullOrEmpty(hookCmd))
			continue;

		bool wait = String::Equals("true", hookWait, StringComparison::OrdinalIgnoreCase);
		bool show = String::Equals("show", hookShow, StringComparison::OrdinalIgnoreCase);

		hooks->Add(gcnew SvnClientHook(type, hookDir, hookCmd, wait, show));
	}
	hooks->Sort();
	hooks->Reverse();
	_tsvnHooks = hooks->ToArray();
}

bool SvnClientContext::FindHook(String^ path, SvnClientHookType hookType, [Out] SvnClientHook^% hook)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (0 == (int)(_xState & SvnExtendedState::TortoiseSvnHooksLoaded))
		throw gcnew InvalidOperationException();
	else if (!Enum::IsDefined(SvnClientHookType::typeid, hookType) || hookType == SvnClientHookType::Undefined)
		throw gcnew ArgumentOutOfRangeException("hookType");

	hook = nullptr;

	if (_tsvnHooks == nullptr || _tsvnHooks->Length == 0)
		return false;

	path = SvnTools::GetNormalizedFullPath(path);

	for each (SvnClientHook^ h in _tsvnHooks)
	{
		if (h->Type != hookType)
			continue;

		if (h->Path->Length == 0)
		{
			hook = h;
			return true;
		}

		if (path->Length < h->Path->Length)
			continue;
		else if (!path->Substring(0, h->Path->Length)->Equals(h->Path, StringComparison::OrdinalIgnoreCase))
			continue;

		if (path->Length == h->Path->Length || path[h->Path->Length] == '\\')
		{
			hook = h;
			return true;
		}
	}

	return false;
}

bool SvnClientHook::Run(SvnClientContext^ ctx, SvnClientArgs^ args, ...array<String^>^ commandArgs)
{
	if (!ctx)
		throw gcnew ArgumentNullException("ctx");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	if (!commandArgs)
		commandArgs = gcnew array<String^>(0);

	String^ application;
	String^ cmdArgs;
	if (!SvnTools::TrySplitCommandLine(Command, application, cmdArgs))
		return args->HandleResult(ctx, gcnew SvnSystemException(String::Format(SharpSvnStrings::CantParseCommandX, Command)));

	application = System::Environment::ExpandEnvironmentVariables(application);
	cmdArgs = System::Environment::ExpandEnvironmentVariables(cmdArgs);

	Process^ p = gcnew Process();
	p->StartInfo->UseShellExecute = false;
	p->StartInfo->CreateNoWindow = !this->ShowConsole;
	p->StartInfo->FileName = application;
	p->StartInfo->WorkingDirectory = System::IO::Path::GetTempPath();

	for each(String^ s in commandArgs)
	{
		if (!String::IsNullOrEmpty(cmdArgs))
			cmdArgs += " ";

		if (s->IndexOf(' ') > 0)
			cmdArgs += "\"" + s->Replace("\"", "\"\"") + "\"";
		else
			cmdArgs += s;
	}
	p->StartInfo->Arguments = cmdArgs->Trim();

	String^ hookName = this->Type.ToString();

	p->StartInfo->EnvironmentVariables->Add("SharpSvn", SvnClient::SharpSvnVersion->ToString());
	p->StartInfo->EnvironmentVariables->Add("SharpSvn_Hook", hookName);
	p->StartInfo->EnvironmentVariables->Add("SharpSvn_ClientName", SvnBase::_clientName);

	/* Keep the tempfiles alive */
	p->EnableRaisingEvents = true;
	p->Exited += gcnew EventHandler(ctx, &SvnClientContext::NopEventHandler);

	try
	{
		p->Start();
	}
	catch (Exception^ e)
	{
		return args->HandleResult(ctx, gcnew SvnClientHookException(e->Message, e));
	}

	if (!WaitForExit)
		return true;

	while (!p->HasExited)
	{
		if (ctx->CtxHandle->cancel_func)
		{
			svn_error_t *err = ctx->CtxHandle->cancel_func(ctx->CtxHandle->cancel_baton);

			if (err)
				return args->HandleResult(ctx, err);
		}

		if (p->HasExited)
			break;

		Sleep(100);
	}

	bool ok = p->HasExited && (p->ExitCode == 0);

	if (p->HasExited)
		delete p; /* Close process handle */

	return ok;
}

void SvnClientContext::ApplyCustomRemoteConfig()
{
	// Look for a custom SSH client; Windows most common SVN client uses their own setting, so we should to :(
	ApplyCustomSsh();

	_contextState = SvnContextState::CustomRemoteConfigApplied;
}

void SvnClientContext::ApplyUserDiffConfig()
{
	if (_useUserDiff || !CtxHandle->config)
		return;

	svn_config_t *cfg = (svn_config_t*)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

	if (!cfg)
		return;

	svn_config_set(cfg, SVN_CONFIG_SECTION_HELPERS, SVN_CONFIG_OPTION_DIFF_CMD, nullptr);
	svn_config_set(cfg, SVN_CONFIG_SECTION_HELPERS, SVN_CONFIG_OPTION_DIFF3_CMD, nullptr);
}

void SvnClientContext::ApplyOverrideFlags()
{
	if (_useUserDiff || !CtxHandle->config)
		return;

	svn_config_t *cfg = (svn_config_t*)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

	if (!cfg)
	{
		SVN_THROW(svn_config_create(&cfg, FALSE, apr_hash_pool_get(CtxHandle->config)));
		apr_hash_set(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING, cfg);
	}

	if (_keepAllExtensionsOnConflict != SvnOverride::Never)
	{
		const char *rslt = NULL;

		if (_keepAllExtensionsOnConflict == SvnOverride::WhenNotSet)
			svn_config_get(cfg, &rslt, SVN_CONFIG_SECTION_MISCELLANY, SVN_CONFIG_OPTION_PRESERVED_CF_EXTS, NULL);

		if (!rslt)
			svn_config_set(cfg, SVN_CONFIG_SECTION_MISCELLANY, SVN_CONFIG_OPTION_PRESERVED_CF_EXTS, "*");
	}
}


static String^ ReadRegKey(RegistryKey^ key, String^ path, String^ name)
{
	if (!key)
		throw gcnew ArgumentNullException("key");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	RegistryKey^ rk = nullptr;
	try
	{
		rk = key->OpenSubKey(path, false);

		if (!rk)
			return nullptr;

		String^ v = dynamic_cast<String^>(rk->GetValue(name, nullptr));

		if (!String::IsNullOrEmpty(v) && !String::IsNullOrEmpty(v->Trim()))
			return v;
	}
	catch (System::Security::SecurityException^)
	{}
	catch (UnauthorizedAccessException^)
	{}
	finally
	{
		if (rk)
			delete rk;
	}

	return nullptr;
}


void SvnClientContext::ApplyCustomSsh()
{
	if (!CtxHandle->config)
		return;

	svn_config_t *cfg = (svn_config_t*)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

	if (!cfg)
		return;

	String^ customSshConfig = ReadRegKey(Registry::CurrentUser, "Software\\QQn\\SharpSvn\\CurrentVersion\\Handlers", "SSH");

	if (!customSshConfig)
		customSshConfig = ReadRegKey(Registry::LocalMachine, "Software\\QQn\\SharpSvn\\CurrentVersion\\Handlers", "SSH");

	if (!customSshConfig)
		customSshConfig = ReadRegKey(Registry::CurrentUser, "Software\\TortoiseSVN", "SSH");

	if (!customSshConfig)
		customSshConfig = ReadRegKey(Registry::LocalMachine, "Software\\TortoiseSVN", "SSH");

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
		{
			val += len;

			while (*val && isspace((int)*val))
				val++;
		}
		else
			val = apr_pstrdup(pool.Handle, cmd);
	}

	if (val && *val)
	{
		wchar_t* buffer = (wchar_t*)apr_pcalloc(pool.Handle, 1024 * sizeof(wchar_t));
		LPWSTR pFile = nullptr;

		if (strcmp(val, "ssh"))
			return; // Something was configured, use it!

		if (SearchPathW(nullptr, L"ssh", L".exe", 1000, buffer, &pFile) && pFile)
		{
			return; // ssh.exe exists. Use it!
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

				path = SvnTools::PathCombine(Path::GetDirectoryName(path), "SharpPlink-" APR_STRINGIFY(SHARPSVN_PLATFORM_SUFFIX) ".svnExe");

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
	if (State >= SvnContextState::ConfigPrepared)
		throw gcnew InvalidOperationException("Configuration already loaded");

	if (String::IsNullOrEmpty(path))
		path = nullptr;

	AprPool tmpPool(_pool);
	const char* szPath = path ? tmpPool.AllocAbsoluteDirent(path) : nullptr;

	if (ensurePath)
		SVN_THROW(svn_config_ensure(szPath, tmpPool.Handle));

	apr_hash_t* cfg = nullptr;
	SVN_THROW(svn_config_get_config(&cfg, szPath, _pool->Handle));

	if (cfg)
	{
		svn_config_t *config = NULL;
		const char *memory_cache_size_str;

		config = (svn_config_t*)apr_hash_get(cfg, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);
		svn_config_get(config, &memory_cache_size_str, SVN_CONFIG_SECTION_MISCELLANY,
			SVN_CONFIG_OPTION_MEMORY_CACHE_SIZE, NULL);

		if (memory_cache_size_str)
			svn_config_set(config, SVN_CONFIG_SECTION_MISCELLANY, SVN_CONFIG_OPTION_MEMORY_CACHE_SIZE, NULL);
	}

	CtxHandle->config = cfg;

	_contextState = SvnContextState::ConfigPrepared;

	if (!_configPath)
		_configPath = path;
}

void SvnClientContext::LoadConfiguration(String ^path)
{
	LoadConfiguration(path, false);
}

void SvnClientContext::LoadConfigurationDefault()
{
	LoadConfiguration(nullptr, true);
}

void SvnClientContext::UseDefaultConfiguration()
{
	if (State >= SvnContextState::ConfigPrepared)
		throw gcnew InvalidOperationException("Configuration already loaded");

	apr_hash_t* cfg = apr_hash_make(_pool->Handle);
	svn_config_t *cf;

	SVN_THROW(svn_config_create(&cf, FALSE, _pool->Handle));
	apr_hash_set(cfg, SVN_CONFIG_CATEGORY_SERVERS, APR_HASH_KEY_STRING, cf);

	SVN_THROW(svn_config_create(&cf, FALSE, _pool->Handle));
	apr_hash_set(cfg, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING, cf);

	CtxHandle->config = cfg;

	_contextState = SvnContextState::ConfigPrepared;
}

void SvnClientContext::MergeConfiguration(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	if (State < SvnContextState::ConfigLoaded)
		LoadConfigurationDefault();

	AprPool tmpPool(_pool);

	const char* szPath = tmpPool.AllocString(path);

	SVN_THROW(svn_config_get_config(&CtxHandle->config, szPath, _pool->Handle));
}

SvnClientContext::ArgsStore::ArgsStore(SvnClientContext^ client, SvnClientArgs^ args, AprPool^ pool)
{
	if (!args)
		throw gcnew ArgumentNullException("args");
	else if (client->_currentArgs)
		throw gcnew InvalidOperationException(SharpSvnStrings::SvnClientOperationInProgress);

	args->Prepare();
	client->_currentArgs = args;
	_client = client;

	svn_client_ctx_t *ctx = _client->CtxHandle;
	svn_wc_context_t **p_wc_ctx = &ctx->wc_ctx;
	_wc_ctx = *p_wc_ctx;

	_lastContext = SvnClientContext::_activeContext;
	SvnClientContext::_activeContext = _client;

	try
	{
		if (! client->KeepSession && pool != nullptr)
			SVN_THROW(svn_wc_context_create(p_wc_ctx, NULL, pool->Handle, pool->Handle));

		client->HandleProcessing(gcnew SvnProcessingEventArgs(args->CommandType));
	}
	catch(Exception^)
	{
		client->_currentArgs = nullptr;
		SvnClientContext::_activeContext = _lastContext;
		throw;
	}
}

SvnClientContext::ArgsStore::~ArgsStore()
{
	SvnClientContext::_activeContext = _lastContext;
	SvnClientArgs^ args = _client->_currentArgs;
	if (args)
		args->_hooked = false;

	_client->_currentArgs = nullptr;

	svn_client_ctx_t *ctx = _client->CtxHandle;
	ctx->wc_ctx = _wc_ctx;
}

SvnClientContext::NoArgsStore::NoArgsStore(SvnClientContext^ client, AprPool^ pool)
{
	if (client->_currentArgs)
		throw gcnew InvalidOperationException(SharpSvnStrings::SvnClientOperationInProgress);

	_client = client;

	svn_client_ctx_t *ctx = _client->CtxHandle;
	svn_wc_context_t **p_wc_ctx = &ctx->wc_ctx;
	_wc_ctx = *p_wc_ctx;
	_lastContext = SvnClientContext::_activeContext;
	SvnClientContext::_activeContext = _client;


	try
	{
		if (! client->KeepSession && pool != nullptr)
			SVN_THROW(svn_wc_context_create(p_wc_ctx, NULL, pool->Handle, pool->Handle));

	}
	catch(Exception^)
	{
		SvnClientContext::_activeContext = _lastContext;
		throw;
	}
}

SvnClientContext::NoArgsStore::~NoArgsStore()
{
	SvnClientContext::_activeContext = _lastContext;

	svn_client_ctx_t *ctx = _client->CtxHandle;
	ctx->wc_ctx = _wc_ctx;
}

static svn_error_t * the_commit_callback2(const svn_commit_info_t *commit_info, void *baton, apr_pool_t *pool)
{
	SvnClientContext::CommitResultReceiver^ receiver;

	AprPool^ tmpPool = gcnew AprPool(pool, false);
	receiver = AprBaton<SvnClientContext::CommitResultReceiver^>::Get((IntPtr)baton);

	try
	{
		receiver->ProvideCommitResult(commit_info, tmpPool);

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("CommitResult function", e);
	}
}

SvnClientContext::CommitResultReceiver::CommitResultReceiver(SvnClient^ client)
{
	_callback = the_commit_callback2;
	_commitBaton = gcnew AprBaton<CommitResultReceiver^>(this);
	_client = client;
}

SvnClientContext::CommitResultReceiver::~CommitResultReceiver()
{
	_client = nullptr;
	_callback = nullptr;
	delete _commitBaton;
	_commitBaton = nullptr;
}

void SvnClientContext::CommitResultReceiver::ProvideCommitResult(const svn_commit_info_t *commit_info, AprPool^ pool)
{
	_commitResult = SvnCommittedEventArgs::Create(_client, commit_info, pool);


	_client->HandleClientCommitted(_commitResult);
}


HWND __cdecl sharpsvn_get_ui_parent()
{
	SvnClientContext^ ctx = SvnClientContext::_activeContext;

	if (ctx != nullptr)
		try
	{
		SvnBeforeEngineDialogEventArgs ^ea = gcnew SvnBeforeEngineDialogEventArgs();

		ctx->Authentication->InvokeOnBeforeEngineDialog(ea);
		return (HWND)(__int32)ea->Handle;
	}
	catch(...)
	{
		return NULL;
	}
	else
		return NULL;
}

extern "C" {
	typedef HWND (*sharpsvn_get_ui_parent_handler_t)();
	extern sharpsvn_get_ui_parent_handler_t sharpsvn_get_ui_parent_handler;
};

void SvnBase::InstallSslDialogHandler()
{
	sharpsvn_get_ui_parent_handler = sharpsvn_get_ui_parent;
}
