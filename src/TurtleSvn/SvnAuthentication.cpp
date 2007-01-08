#include "stdafx.h"

#include "SvnAll.h"
#include "SvnObjBaton.h"
#include "SvnAuthentication.h"

using namespace TurtleSvn;
using namespace TurtleSvn::Apr;
using namespace TurtleSvn::Security;
using System::Text::StringBuilder;

struct svn_auth_baton_t
{};


void SvnAuthentication::Clear()
{
	_wrappers->Clear();
	_handlers->Clear();
}

void SvnAuthentication::AddSubversionFileHandlers()
{
	UsernameHandlers						+= SubversionFileUsernameHandler;
	UsernamePasswordHandlers				+= SubversionFileUsernamePasswordHandler;
	UsernamePasswordHandlers				+= SubversionWindowsUsernamePasswordHandler; // svn.exe adds this after the other handler
	SslServerTrustHandlers					+= SubversionFileSslServerTrustHandler;
	SslClientCertificateHandlers			+= SubversionFileSslClientCertificateHandler;
	SslClientCertificatePasswordHandlers	+= SubversionFileSslClientCertificatePasswordHandler;
}

void SvnAuthentication::AddDialogHandlers()
{
	UsernameHandlers						+= DialogUsernameHandler;
	UsernamePasswordHandlers				+= DialogUsernamePasswordHandler;
	SslServerTrustHandlers					+= DialogSslServerTrustHandler;
	SslClientCertificateHandlers			+= DialogSslClientCertificateHandler;
	SslClientCertificatePasswordHandlers	+= DialogSslClientCertificatePasswordHandler;
}

void SvnAuthentication::AddConsoleHandlers()
{
	UsernameHandlers						+= ConsoleUsernameHandler;
	UsernamePasswordHandlers				+= ConsoleUsernamePasswordHandler;
	SslServerTrustHandlers					+= ConsoleSslServerTrustHandler;
	SslClientCertificateHandlers			+= ConsoleSslClientCertificateHandler;
	SslClientCertificatePasswordHandlers	+= ConsoleSslClientCertificatePasswordHandler;
}

/// <summary>Retrieves an authorization baton allocated in the specified pool; containing the current authorization settings</summary>
svn_auth_baton_t *SvnAuthentication::GetAuthorizationBaton(AprPool ^pool, [Out] int% cookie)
{
	if(!pool)
		throw gcnew NullReferenceException("pool");

	AprArray<ISvnAuthWrapper^, SvnAuthProviderMarshaller^>^ authArray = gcnew AprArray<ISvnAuthWrapper^, SvnAuthProviderMarshaller^>(_handlers, pool);

	svn_auth_baton_t *rslt = nullptr;

	svn_auth_open(&rslt, authArray->Handle, pool->Handle);

	cookie = _cookie;
	return rslt;
}


struct AuthPromptWrappers
{
	static svn_error_t* svn_auth_username_prompt_func(svn_auth_cred_username_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* svn_auth_simple_prompt_func(svn_auth_cred_simple_t **cred, void *baton, const char *realm, const char *username, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* svn_auth_ssl_server_trust_prompt_func(svn_auth_cred_ssl_server_trust_t **cred, void *baton, const char *realm, apr_uint32_t failures, const svn_auth_ssl_server_cert_info_t *cert_info, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* svn_auth_ssl_client_cert_prompt_func(svn_auth_cred_ssl_client_cert_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* svn_auth_ssl_client_cert_pw_prompt_func(svn_auth_cred_ssl_client_cert_pw_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool);
};


////////////////////////////////////////////////////////////////
#pragma region // Username handler wrapper

svn_error_t* AuthPromptWrappers::svn_auth_username_prompt_func(svn_auth_cred_username_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool)
{
	SvnAuthWrapper<SvnUsernameArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnUsernameArgs^>^>::Get((IntPtr)baton);

	SvnUsernameArgs^ args = gcnew SvnUsernameArgs(SvnBase::Utf8_PtrToString(realm), may_save != 0);

	if(!wrapper->Raise(args) && !args->Cancel)
	{
		*cred = nullptr;
		return nullptr;
	}

	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");

	*cred = (svn_auth_cred_username_t *)AprPool::AllocCleared(sizeof(svn_auth_cred_username_t), pool);

	(*cred)->username = AprPool::AllocString(args->Username, pool);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnUsernameArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = NULL;

	if(_handler->Equals(SvnAuthentication::SubversionFileUsernameHandler))
	{
		svn_auth_get_username_provider(&provider, pool->Handle);
	}
	else
		svn_auth_get_username_prompt_provider(&provider, &AuthPromptWrappers::svn_auth_username_prompt_func, (void*)_baton->Handle, RetryLimit, pool->Handle);

	return provider;
}
#pragma endregion

////////////////////////////////////////////////////////////////
#pragma region // UsernamePassword handler wrapper

svn_error_t* AuthPromptWrappers::svn_auth_simple_prompt_func(svn_auth_cred_simple_t **cred, void *baton, const char *realm, const char *username, svn_boolean_t may_save, apr_pool_t *pool)
{
	SvnAuthWrapper<SvnUsernamePasswordArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnUsernamePasswordArgs^>^>::Get((IntPtr)baton);

	SvnUsernamePasswordArgs^ args = gcnew SvnUsernamePasswordArgs(SvnBase::Utf8_PtrToString(username), SvnBase::Utf8_PtrToString(realm), may_save != 0);

	if(!wrapper->Raise(args) && !args->Cancel)
	{
		*cred = nullptr;
		return nullptr;
	}

	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");

	*cred = (svn_auth_cred_simple_t *)AprPool::AllocCleared(sizeof(svn_auth_cred_simple_t), pool);

	(*cred)->username = AprPool::AllocString(args->Username, pool);
	(*cred)->password = AprPool::AllocString(args->Password, pool);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnUsernamePasswordArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = NULL;

	if(_handler->Equals(SvnAuthentication::SubversionFileUsernamePasswordHandler))
	{
		svn_auth_get_simple_provider(&provider, pool->Handle);
	}
	else if(_handler->Equals(SvnAuthentication::SubversionWindowsUsernamePasswordHandler))
	{
		svn_auth_get_windows_simple_provider(&provider, pool->Handle);
	}
	else
		svn_auth_get_simple_prompt_provider(&provider, &AuthPromptWrappers::svn_auth_simple_prompt_func, (void*)_baton->Handle, RetryLimit, pool->Handle);

	return provider;
}
#pragma endregion

////////////////////////////////////////////////////////////////
#pragma region	// SvnSslServerTrustArgs handler wrapper

svn_error_t* AuthPromptWrappers::svn_auth_ssl_server_trust_prompt_func(svn_auth_cred_ssl_server_trust_t **cred, void *baton, const char *realm, apr_uint32_t failures, const svn_auth_ssl_server_cert_info_t *cert_info, svn_boolean_t may_save, apr_pool_t *pool)
{
	SvnAuthWrapper<SvnSslServerTrustArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnSslServerTrustArgs^>^>::Get((IntPtr)baton);

	SvnSslServerTrustArgs^ args = gcnew SvnSslServerTrustArgs(
		(SvnCertificateTrustFailure)failures, 
		SvnBase::Utf8_PtrToString(cert_info->hostname),
		SvnBase::Utf8_PtrToString(cert_info->fingerprint),
		SvnBase::Utf8_PtrToString(cert_info->valid_from),
		SvnBase::Utf8_PtrToString(cert_info->valid_until),
		SvnBase::Utf8_PtrToString(cert_info->issuer_dname),
		SvnBase::Utf8_PtrToString(cert_info->ascii_cert),
		SvnBase::Utf8_PtrToString(realm), may_save != 0);

	if(!wrapper->Raise(args) && !args->Cancel)
	{
		*cred = nullptr;
		return nullptr;
	}

	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");

	*cred = (svn_auth_cred_ssl_server_trust_t*)AprPool::AllocCleared(sizeof(svn_auth_cred_ssl_server_trust_t), pool);

	(*cred)->accepted_failures = (apr_uint32_t)args->AcceptedFailures;
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnSslServerTrustArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = NULL;

	if(_handler->Equals(SvnAuthentication::SubversionFileSslServerTrustHandler))
	{
		svn_auth_get_ssl_server_trust_file_provider(&provider, pool->Handle);
	}
	else
		svn_auth_get_ssl_server_trust_prompt_provider(&provider, &AuthPromptWrappers::svn_auth_ssl_server_trust_prompt_func, (void*)_baton->Handle, pool->Handle);

	return provider;
}
#pragma endregion

////////////////////////////////////////////////////////////////
#pragma region // SslClientCertificate handler wrapper

svn_error_t* AuthPromptWrappers::svn_auth_ssl_client_cert_prompt_func(svn_auth_cred_ssl_client_cert_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool)
{
	SvnAuthWrapper<SvnSslClientCertificateArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnSslClientCertificateArgs^>^>::Get((IntPtr)baton);

	SvnSslClientCertificateArgs^ args = gcnew SvnSslClientCertificateArgs(SvnBase::Utf8_PtrToString(realm), may_save != 0);

	if(!wrapper->Raise(args) && !args->Cancel)
	{
		*cred = nullptr;
		return nullptr;
	}

	if(args->Cancel)
		return svn_error_create(SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");

	*cred = (svn_auth_cred_ssl_client_cert_t *)AprPool::AllocCleared(sizeof(svn_auth_cred_ssl_client_cert_t), pool);

	(*cred)->cert_file = AprPool::AllocString(args->CertificateFile, pool);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnSslClientCertificateArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = NULL;

	if(_handler->Equals(SvnAuthentication::SubversionFileSslClientCertificateHandler))
	{
		svn_auth_get_ssl_client_cert_file_provider(&provider, pool->Handle);
	}
	else
		svn_auth_get_ssl_client_cert_prompt_provider(&provider, &AuthPromptWrappers::svn_auth_ssl_client_cert_prompt_func, (void*)_baton->Handle, RetryLimit, pool->Handle);

	return provider;
}
#pragma endregion

////////////////////////////////////////////////////////////////
#pragma region // UsernamePassword handler wrapper

svn_error_t* AuthPromptWrappers::svn_auth_ssl_client_cert_pw_prompt_func(svn_auth_cred_ssl_client_cert_pw_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool)
{
	SvnAuthWrapper<SvnSslClientCertificatePasswordArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnSslClientCertificatePasswordArgs^>^>::Get((IntPtr)baton);

	SvnSslClientCertificatePasswordArgs^ args = gcnew SvnSslClientCertificatePasswordArgs(SvnBase::Utf8_PtrToString(realm), may_save != 0);

	if(!wrapper->Raise(args) && !args->Cancel)
	{
		*cred = nullptr;
		return nullptr;
	}

	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");

	*cred = (svn_auth_cred_ssl_client_cert_pw_t *)AprPool::AllocCleared(sizeof(svn_auth_cred_ssl_client_cert_pw_t), pool);

	(*cred)->password = AprPool::AllocString(args->Password, pool);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnSslClientCertificatePasswordArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = NULL;

	if(_handler->Equals(SvnAuthentication::SubversionFileSslClientCertificatePasswordHandler))
	{
		svn_auth_get_ssl_client_cert_pw_file_provider(&provider, pool->Handle);
	}
	else
		svn_auth_get_ssl_client_cert_pw_prompt_provider(&provider, &AuthPromptWrappers::svn_auth_ssl_client_cert_pw_prompt_func, (void*)_baton->Handle, RetryLimit, pool->Handle);

	return provider;
}
#pragma endregion

bool SvnAuthentication::ImpDialogUsernameHandler(Object ^sender, SvnUsernameArgs^ e)
{
	throw gcnew NotImplementedException();
}
bool SvnAuthentication::ImpDialogUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e)
{
	throw gcnew NotImplementedException();
}
bool SvnAuthentication::ImpDialogSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e)
{
	throw gcnew NotImplementedException();
}

bool SvnAuthentication::ImpDialogSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e)
{
	throw gcnew NotImplementedException();
}

bool SvnAuthentication::ImpDialogSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e)
{
	throw gcnew NotImplementedException();
}

///////////////////////////////
void SvnAuthentication::MaybePrintRealm(SvnAuthorizationArgs^ e)
{
	if(!e)
		throw gcnew ArgumentNullException("e");

	if(e->Realm)
		Console::WriteLine("Authentication realm: {0}", e->Realm);
}

String^ SvnAuthentication::ReadPassword()
{
	StringBuilder^ sb = gcnew StringBuilder();

	ConsoleKeyInfo^ key;
	do
	{
		key = Console::ReadKey(true);

		switch(key->Key)
		{
		case ConsoleKey::Backspace:
			if(sb->Length > 0)
			{
				Console::Write("\b \b");

				sb->Length--;
			}
			break;
		case ConsoleKey::Enter:
			break; // Don't add to buffer!
		default:
			Console::Write('*');
			sb->Append(key->KeyChar);
		}
	}
	while(key->Key != ConsoleKey::Enter);

	return sb->ToString();
}


bool SvnAuthentication::ImpConsoleUsernameHandler(Object ^sender, SvnUsernameArgs^ e)
{
	MaybePrintRealm(e);

	Console::Write("Username: ");
	e->Username = Console::ReadLine();

	return true;
}

bool SvnAuthentication::ImpConsoleUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e)
{
	MaybePrintRealm(e);

	if(!e->InitialUsername)
	{
		Console::Write("Username: ");
		e->Username = Console::ReadLine();
	}
	else
		e->Username = e->InitialUsername;

	Console::WriteLine();

	Console::Write("Password: ");
	e->Password = ReadPassword();
	Console::WriteLine();

	return true;
}

bool SvnAuthentication::ImpConsoleSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e)
{
	Console::WriteLine("Error validating server certificate for '{0}':", e->Realm);

	if(SvnCertificateTrustFailure::None != (e->Failures & SvnCertificateTrustFailure::UnknownCertificateAuthority))
	{
		Console::WriteLine(" - The certificate is not issued by a trusted authority. Use the\n"
			"   fingerprint to validate the certificate manually!");
	}
	if(SvnCertificateTrustFailure::None != (e->Failures & SvnCertificateTrustFailure::CommonNameMismatch))
	{
		Console::WriteLine(" - The certificate hostname does not match.");
	}
	if(SvnCertificateTrustFailure::None != (e->Failures & SvnCertificateTrustFailure::CertificateNotValidYet))
	{
		Console::WriteLine(" - The certificate is not yet valid.");
	}
	if(SvnCertificateTrustFailure::None != (e->Failures & SvnCertificateTrustFailure::CertificateExpired))
	{
		Console::WriteLine(" - The certificate is has expired.");
	}
	if(SvnCertificateTrustFailure::None != (e->Failures & SvnCertificateTrustFailure::UnknownSslProviderFailure))
	{
		Console::WriteLine(" - The certificate has an unknown error.");
	}

	Console::WriteLine("Certificcate information:\n"
		" - Hostname: {0}\n"
		" - Valid: from {1} until {2}\n"
		" - Issuer: {3}\n"
		" - Fingerprint: {4}",
		e->CommonName,
		e->ValidFrom,
		e->ValidUntil,
		e->Issuer,
		e->FingerPrint);

	try
	{
		while(true)
		{
			Console::Write("(R)eject, accept (t)emporarily or accept (p)ermanently? ");

			ConsoleKeyInfo^ki = Console::ReadKey();
			Console::WriteLine();

			switch(ki->Key)
			{
			case ConsoleKey::Escape:
			case ConsoleKey::R:
				return false;
			case ConsoleKey::P:
				e->AcceptedFailures = e->Failures;
				e->Save = e->MaySave;
				return true;
			case ConsoleKey::T:
				e->AcceptedFailures = e->Failures;
				return true;
			}
		}
	}
	finally
	{
		Console::WriteLine();
	}
}

bool SvnAuthentication::ImpConsoleSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e)
{
	MaybePrintRealm(e);

	Console::Write("Client certificate filename: ");

	e->CertificateFile = Console::ReadLine();
	Console::WriteLine();
	return true;
}

bool SvnAuthentication::ImpConsoleSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e)
{
	Console::Write("Passphrase for '{0}': ", e->Realm);

	e->Password = ReadPassword();
	Console::WriteLine();
	return true;
}