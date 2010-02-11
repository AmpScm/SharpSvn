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
#include "SvnClientContext.h"
#include "SvnAuthentication.h"

#include <wincred.h>
#include <svn_hash.h>

using System::Text::RegularExpressions::Match;
using System::Collections::ObjectModel::Collection;
using System::IO::DirectoryInfo;
using System::IO::FileInfo;

//////////////////////////////////////////////////////////////////////////////////////
// All this depends on intimate knowledge of implementation within
// subversion/libsvn_subr/auth.c. The sharpsvn_svn_auth_get_credentials_cache
// method is patched into that method to make it possible to copy credentials
// between different SvnClient
//
// We can do this as we are static compiled against a specific version of subversion
// But we can't guarantee this will be compatible with future subversion releases
// (or even patch releases)

#include "svn-internal/libsvn_subr/config_impl.h"

extern "C" {
	apr_hash_t* sharpsvn_svn_auth_get_credentials_cache(svn_auth_baton_t *auth_baton);
}


void SvnAuthentication::CopyAuthenticationCache(SharpSvn::SvnClientContext ^client)
{
	if (!client)
		throw gcnew ArgumentNullException("client");

	svn_client_ctx_t* ctx = client->CtxHandle;
	if (!ctx || !ctx->auth_baton)
		return; // Nothing to copy

	apr_hash_t* cache = sharpsvn_svn_auth_get_credentials_cache(ctx->auth_baton);

	if (!cache || !apr_hash_count(cache))
		return;

	if (!_currentBaton)
		_clientContext->EnsureState(SvnContextState::AuthorizationInitialized);

	if (!_currentBaton)
		return;

	apr_hash_t *toCache = sharpsvn_svn_auth_get_credentials_cache(_currentBaton);

	if (!toCache)
		return;

	clone_credentials(get_cache(ctx->auth_baton), toCache, _authPool);
}

void SvnAuthentication::ClearAuthenticationCache()
{
	if (_clientContext->CurrentCommandArgs)
		throw gcnew InvalidOperationException(); // Busy in request
	else if (!_currentBaton)
		return;

	apr_hash_t *hash = get_cache(_currentBaton);

	if (hash)
		apr_hash_clear(hash);
}


static bool _MatchPrefix(const char* key, const char* needle, size_t needleLen)
{
	return !strncmp(key, needle, needleLen);
}

#define MatchPrefix(key, needle) _MatchPrefix(key, needle ":", sizeof(needle))


static svn_auth_cred_simple_t* clone_cred_usernamepassword(const svn_auth_cred_simple_t* from, AprPool^ to)
{
	svn_auth_cred_simple_t* r = (svn_auth_cred_simple_t*)to->AllocCleared(sizeof(svn_auth_cred_simple_t));

	r->username = from->username ? apr_pstrdup(to->Handle, from->username) : nullptr;
	r->password = from->password ? apr_pstrdup(to->Handle, from->password) : nullptr;
	r->may_save = from->may_save;

	return r;
}

static svn_auth_cred_username_t* clone_cred_username(const svn_auth_cred_username_t* from, AprPool^ to)
{
	svn_auth_cred_username_t* r = (svn_auth_cred_username_t*)to->AllocCleared(sizeof(svn_auth_cred_username_t));

	r->username = from->username ? apr_pstrdup(to->Handle, from->username) : nullptr;
	r->may_save = from->may_save;

	return r;
}

static svn_auth_cred_ssl_client_cert_t* clone_cred_ssl_clientcert(const svn_auth_cred_ssl_client_cert_t* from, AprPool^ to)
{
	svn_auth_cred_ssl_client_cert_t* r = (svn_auth_cred_ssl_client_cert_t*)to->AllocCleared(sizeof(svn_auth_cred_ssl_client_cert_t));

	r->cert_file = from->cert_file ? apr_pstrdup(to->Handle, from->cert_file) : nullptr;
	r->may_save = from->may_save;

	return r;
}

static svn_auth_cred_ssl_client_cert_pw_t* clone_cred_ssl_clientcertpw(const svn_auth_cred_ssl_client_cert_pw_t* from, AprPool^ to)
{
	svn_auth_cred_ssl_client_cert_pw_t* r = (svn_auth_cred_ssl_client_cert_pw_t*)to->AllocCleared(sizeof(svn_auth_cred_ssl_client_cert_pw_t));

	r->password = from->password ? apr_pstrdup(to->Handle, from->password) : nullptr;
	r->may_save = from->may_save;

	return r;
}

static svn_auth_cred_ssl_server_trust_t* clone_cred_ssl_servercert(const svn_auth_cred_ssl_server_trust_t* from, AprPool^ to)
{
	svn_auth_cred_ssl_server_trust_t* r = (svn_auth_cred_ssl_server_trust_t*)to->AllocCleared(sizeof(svn_auth_cred_ssl_server_trust_t));

	r->accepted_failures = from->accepted_failures;
	r->may_save = from->may_save;

	return r;
}

apr_hash_t* SvnAuthentication::get_cache(svn_auth_baton_t* baton)
{
	if (!baton)
		throw gcnew ArgumentNullException("baton");

	return sharpsvn_svn_auth_get_credentials_cache(baton);
}

apr_hash_t* SvnAuthentication::clone_credentials(apr_hash_t *from, apr_hash_t *to, AprPool^ pool)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");
	// to can be null

	apr_hash_t* hash_to = to ? to : apr_hash_make(pool->Handle);
	const char* pKey;
	apr_ssize_t len;
	void* pValue;
	void* pNewValue;

	AprPool tmpPool(pool);

	for (apr_hash_index_t *hi = apr_hash_first(tmpPool.Handle, from); hi; hi = apr_hash_next(hi))
	{
		apr_hash_this(hi, (const void**)&pKey, &len, &pValue);

		pNewValue = nullptr;

		if (!pValue)
			continue;

		if (MatchPrefix(pKey, SVN_AUTH_CRED_SIMPLE))
			pNewValue = clone_cred_usernamepassword((const svn_auth_cred_simple_t*)pValue, pool);
		else if (MatchPrefix(pKey, SVN_AUTH_CRED_USERNAME))
			pNewValue = clone_cred_username((const svn_auth_cred_username_t*)pValue, pool);
		else if (MatchPrefix(pKey, SVN_AUTH_CRED_SSL_CLIENT_CERT))
			pNewValue = clone_cred_ssl_clientcert((const svn_auth_cred_ssl_client_cert_t*)pValue, pool);
		else if (MatchPrefix(pKey, SVN_AUTH_CRED_SSL_CLIENT_CERT_PW))
			pNewValue = clone_cred_ssl_clientcertpw((const svn_auth_cred_ssl_client_cert_pw_t*)pValue, pool);
		else if (MatchPrefix(pKey, SVN_AUTH_CRED_SSL_SERVER_TRUST))
			pNewValue = clone_cred_ssl_servercert((const svn_auth_cred_ssl_server_trust_t*)pValue, pool);
		/* else: Unknown -> Don't copy */

		if (pNewValue)
		{
			// Create a 0 terminated copy of key
			char *pNewKey = (char*)apr_pcalloc(pool->Handle, len+1);
			memcpy(pNewKey, pKey, len);
			apr_hash_set(hash_to, pNewKey, len, pNewValue);
		}
	}

	return hash_to;
}

SvnAuthenticationCacheItem::SvnAuthenticationCacheItem(SvnAuthenticationCacheType type, String^ realm, String^ filename)
{
	if (String::IsNullOrEmpty(filename))
		throw gcnew ArgumentNullException("filename");
	else if (String::IsNullOrEmpty(realm))
		throw gcnew ArgumentNullException("realm");

	_filename = filename;
	_type = type;
	_realm = realm;
}

SvnAuthenticationCacheItem::SvnAuthenticationCacheItem(SvnAuthenticationCacheType type, Uri^ realmUri)
{
	if (!realmUri)
		throw gcnew ArgumentNullException("realmUri");
	
	// _filename = nullptr;
	_type = type;
	_realm = realmUri->ToString();
	_realmUri = realmUri;
}

System::Uri^ SvnAuthenticationCacheItem::RealmUri::get()
{
	if (_realmUri || !Realm)
		return _realmUri;

	Match^ m = SvnAuthenticationEventArgs::_reRealmUri->Match(Realm);

	Uri^ uri;

	if (m->Success)
	{
		String^ uriValue = m->Groups[1]->Value;

		if (uriValue && !uriValue->EndsWith("/", StringComparison::Ordinal))
			uriValue += "/";

		if (Uri::TryCreate(uriValue, UriKind::Absolute, uri))
			_realmUri = uri;
	}

	if (Realm->Contains("://") && (0 > Realm->IndexOf(' ')))
		Uri::TryCreate(Realm, UriKind::Absolute, _realmUri);

	return _realmUri;
}

void SvnAuthenticationCacheItem::Delete()
{
	if (!String::IsNullOrEmpty(_filename))
	{
		if (System::IO::File::Exists(_filename))
			System::IO::File::Delete(_filename);
	}
	else
	{
		String^ target = RealmUri->AbsoluteUri;
		pin_ptr<const wchar_t> pTarget = PtrToStringChars(target);

		CredDeleteW(pTarget, CRED_TYPE_GENERIC, 0);
	}
}

bool SvnAuthenticationCacheItem::IsDeleted::get()
{
	if (!String::IsNullOrEmpty(_filename))
		return System::IO::File::Exists(_filename);

	return false;
}


Collection<SvnAuthenticationCacheItem^>^
SvnAuthentication::GetCachedItems(SvnAuthenticationCacheType type)
{
	_clientContext->EnsureState(SharpSvn::Implementation::SvnContextState::AuthorizationInitialized);

	AprPool pool(SvnBase::SmallThreadPool);

	const char* cfg = nullptr;
	SVN_THROW(svn_config_get_user_config_path(
		&cfg,
		_clientContext->_configPath ? pool.AllocPath(_clientContext->_configPath) : nullptr,
		SVN_CONFIG__AUTH_SUBDIR,
		pool.Handle));

	const char* append = nullptr;;
	switch (type)
	{
	case SvnAuthenticationCacheType::UserName:
		append = SVN_AUTH_CRED_USERNAME;
		break;
	case SvnAuthenticationCacheType::UserNamePassword:
		append = SVN_AUTH_CRED_SIMPLE;
		break;
	case SvnAuthenticationCacheType::SslServerTrust:
		append = SVN_AUTH_CRED_SSL_SERVER_TRUST;
		break;
	case SvnAuthenticationCacheType::SslClientCertificate:
		append = SVN_AUTH_CRED_SSL_CLIENT_CERT;
		break;
	case SvnAuthenticationCacheType::SslClientCertificatePassword:
		append = SVN_AUTH_CRED_SSL_CLIENT_CERT_PW;
		break;
	case SvnAuthenticationCacheType::WindowsSshCredentials:
		return GetSshCredentials();
	}

	if (!cfg || !append)
		return gcnew Collection<SvnAuthenticationCacheItem^>();

	cfg = svn_path_join(cfg, append, pool.Handle);

	DirectoryInfo^ dir = gcnew DirectoryInfo(Utf8_PtrToString(cfg));

	if (!dir->Exists)
		return gcnew Collection<SvnAuthenticationCacheItem^>();

	List<SvnAuthenticationCacheItem^>^ items = gcnew List<SvnAuthenticationCacheItem^>();
	AprPool pl(%pool);
	for each(FileInfo^ file in dir->GetFiles("*"))
	{
		if (file->Name->Length != 32)
			continue;

		bool ok = true;
		for(int i = 0; i < 32; i++)
		{
			if (0 > ((String^)"0123456789abcdef")->IndexOf(file->Name[i]))
			{
				ok = false;
				break;
			}
		}
		if (!ok)
			continue;

		pl.Clear(); // Clear before running to clear old state

		svn_stream_t* stream = nullptr;
		svn_error_t* r = svn_stream_open_readonly(&stream, pool.AllocPath(file->FullName), pl.Handle, pl.Handle);

		apr_hash_t* hash = nullptr;
		if (!r)
		{
			hash = apr_hash_make(pl.Handle);

			r = svn_hash_read2(hash, stream, SVN_HASH_TERMINATOR, pl.Handle);

			svn_error_clear(svn_stream_close(stream));
		}

		if (r)
		{
			svn_error_clear(r);
			continue;
		}
		else if (!hash)
			continue;

		svn_string_t* pRealm = (svn_string_t*)apr_hash_get(hash, SVN_CONFIG_REALMSTRING_KEY, APR_HASH_KEY_STRING);

		if(!pRealm)
			continue;

		String^ realm = Utf8_PtrToString(pRealm->data, (int)pRealm->len);
		if (realm)
		{
			items->Add(gcnew SvnAuthenticationCacheItem(type, realm, file->FullName));
		}
	}

	return gcnew Collection<SvnAuthenticationCacheItem^>(items->AsReadOnly());
}

Collection<SvnAuthenticationCacheItem^>^ SvnAuthentication::GetSshCredentials()
{
	PCREDENTIALW* pCredentials;
	DWORD nCredentials;

	Collection<SvnAuthenticationCacheItem^>^ result = gcnew Collection<SvnAuthenticationCacheItem^>();

	if (System::Environment::OSVersion->Version < gcnew Version(5, 1))
		return result; // Not supported on Windows 2000

	if (!CredEnumerateW(NULL, 0, &nCredentials, &pCredentials))
		return result; // No cache available
	
	try
	{
		for (DWORD i = 0; i < nCredentials; i++)
		{
			if (pCredentials[i]->Type != CRED_TYPE_GENERIC)
				continue;

			String^ target = gcnew String(pCredentials[i]->TargetName);
			Uri^ targetUri;

			if (!System::Uri::TryCreate(target, UriKind::Absolute, targetUri) || targetUri->Scheme != "ssh")
				continue;

			result->Add(gcnew SvnAuthenticationCacheItem(SvnAuthenticationCacheType::WindowsSshCredentials, targetUri));
		}
		return nullptr;
	}
	finally
	{
		CredFree(pCredentials);
	}
}
