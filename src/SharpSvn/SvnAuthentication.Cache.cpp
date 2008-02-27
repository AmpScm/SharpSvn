// $Id: SvnAuthentication.cpp 293 2008-02-19 17:23:17Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"
#include "SvnClientContext.h"
#include "SvnAuthentication.h"

//////////////////////////////////////////////////////////////////////////////////////
// All this depends on intimate knowledge of implementation within 
// subversion/libsvn_subr/auth.c. The sharpsvn_svn_auth_get_credentials_cache
// method is patched into that method to make it possible to copy credentials
// between different clients
//
// We can do this as we are static compiled against a specific version of subversion
//

extern "C" {
	apr_hash_t* sharpsvn_svn_auth_get_credentials_cache(svn_auth_baton_t *auth_baton);
}


void SvnAuthentication::CopyAuthenticationCache(SharpSvn::SvnClientContext ^client)
{
	if (!client)
		throw gcnew ArgumentNullException("client");

	svn_client_ctx_t* ctx = client->CtxHandle;
	if(!ctx || !ctx->auth_baton)
		return; // Nothing to copy

	apr_hash_t* cache = sharpsvn_svn_auth_get_credentials_cache(ctx->auth_baton);

	if(!cache || !apr_hash_count(cache))
		return;

	if(!_currentBaton)
		_clientContext->EnsureState(SvnContextState::AuthorizationInitialized);

	if(!_currentBaton)
		return;

	apr_hash_t *toCache = sharpsvn_svn_auth_get_credentials_cache(_currentBaton);

	if(!toCache)
		return;

	clone_credentials(get_cache(ctx->auth_baton), toCache, _authPool);
}

void SvnAuthentication::ClearAuthenticationCache()
{
	if(!_currentBaton)
		return;
		
	apr_hash_t *hash = get_cache(_currentBaton);

	if(hash)
		sharpsvn_apr_hash_clear(hash);
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
	if(!baton)
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

		if(!pValue)
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

		if(pNewValue)
			apr_hash_set(hash_to, pKey, len, pNewValue);
	}

	return hash_to;
}