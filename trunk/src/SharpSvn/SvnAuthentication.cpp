#include "stdafx.h"

#include "SvnAll.h"
#include "SvnAuthentication.h"
#include "Wincrypt.h"

using namespace SharpSvn;
using namespace SharpSvn::Apr;
using namespace SharpSvn::Security;
using System::Text::StringBuilder;

[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Security.SvnAuthProviderMarshaller")];

SvnAuthentication::SvnAuthentication()
{
	_wrappers = gcnew Dictionary<Delegate^, ISvnAuthWrapper^>();
	_handlers = gcnew List<ISvnAuthWrapper^>();

	AddSubversionFileHandlers(); // Add handlers which use no interaction by default
}

void SvnAuthentication::Clear()
{
	_wrappers->Clear();
	_handlers->Clear();
}

void SvnAuthentication::AddSubversionFileHandlers()
{
	UsernameHandlers						+= SubversionFileUsernameHandler;
	UsernamePasswordHandlers				+= SubversionWindowsUsernamePasswordHandler;
	UsernamePasswordHandlers				+= SubversionFileUsernamePasswordHandler;
	SslServerTrustHandlers					+= SubversionWindowsSslServerTrustHandler;
	SslServerTrustHandlers					+= SubversionFileSslServerTrustHandler;
	SslClientCertificateHandlers			+= SubversionFileSslClientCertificateHandler;
	SslClientCertificatePasswordHandlers	+= SubversionFileSslClientCertificatePasswordHandler;
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
		throw gcnew ArgumentNullException("pool");

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
	SvnAuthWrapper<SvnUsernameEventArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnUsernameEventArgs^>^>::Get((IntPtr)baton);

	SvnUsernameEventArgs^ args = gcnew SvnUsernameEventArgs(SvnBase::Utf8_PtrToString(realm), may_save != 0);

	AprPool tmpPool(pool, false);
	*cred = nullptr;
	try
	{
		wrapper->Raise(args);

	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Authorization handler", e);
	}
	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");
	else if(args->Break)
		return nullptr;


	*cred = (svn_auth_cred_username_t *)tmpPool.AllocCleared(sizeof(svn_auth_cred_username_t));

	(*cred)->username = tmpPool.AllocString(args->Username);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnUsernameEventArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = nullptr;

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
	SvnAuthWrapper<SvnUsernamePasswordEventArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnUsernamePasswordEventArgs^>^>::Get((IntPtr)baton);

	SvnUsernamePasswordEventArgs^ args = gcnew SvnUsernamePasswordEventArgs(SvnBase::Utf8_PtrToString(username), SvnBase::Utf8_PtrToString(realm), may_save != 0);

	AprPool tmpPool(pool, false);
	*cred = nullptr;
	try
	{
		wrapper->Raise(args);

	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Authorization handler", e);
	}
	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");
	else if(args->Break)
		return nullptr;

	*cred = (svn_auth_cred_simple_t *)tmpPool.AllocCleared(sizeof(svn_auth_cred_simple_t));

	(*cred)->username = tmpPool.AllocString(args->Username);
	(*cred)->password = tmpPool.AllocString(args->Password);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnUsernamePasswordEventArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = nullptr;

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
#pragma region	// SvnSslServerTrustEventArgs handler wrapper

svn_error_t* AuthPromptWrappers::svn_auth_ssl_server_trust_prompt_func(svn_auth_cred_ssl_server_trust_t **cred, void *baton, const char *realm, apr_uint32_t failures, const svn_auth_ssl_server_cert_info_t *cert_info, svn_boolean_t may_save, apr_pool_t *pool)
{
	SvnAuthWrapper<SvnSslServerTrustEventArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnSslServerTrustEventArgs^>^>::Get((IntPtr)baton);

	SvnSslServerTrustEventArgs^ args = gcnew SvnSslServerTrustEventArgs(
		(SvnCertificateTrustFailures)failures,
		SvnBase::Utf8_PtrToString(cert_info->hostname),
		SvnBase::Utf8_PtrToString(cert_info->fingerprint),
		SvnBase::Utf8_PtrToString(cert_info->valid_from),
		SvnBase::Utf8_PtrToString(cert_info->valid_until),
		SvnBase::Utf8_PtrToString(cert_info->issuer_dname),
		SvnBase::Utf8_PtrToString(cert_info->ascii_cert),
		SvnBase::Utf8_PtrToString(realm), may_save != 0);

	AprPool tmpPool(pool, false);
	*cred = nullptr;
	try
	{
		wrapper->Raise(args);

	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Authorization handler", e);
	}
	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");
	else if(args->Break)
		return nullptr;


	*cred = (svn_auth_cred_ssl_server_trust_t*)tmpPool.AllocCleared(sizeof(svn_auth_cred_ssl_server_trust_t));

	(*cred)->accepted_failures = (apr_uint32_t)args->AcceptedFailures;
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnSslServerTrustEventArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = nullptr;

	if(_handler->Equals(SvnAuthentication::SubversionFileSslServerTrustHandler))
	{
		svn_auth_get_ssl_server_trust_file_provider(&provider, pool->Handle);
	}
#if (SVN_VER_MAJOR > 1) || (SVN_VER_MINOR >= 5)
	else if(_handler->Equals(SvnAuthentication::SubversionWindowsSslServerTrustHandler))
	{
		svn_auth_get_windows_ssl_server_trust_provider(&provider, pool->Handle);
	}
#endif
	else
		svn_auth_get_ssl_server_trust_prompt_provider(&provider, &AuthPromptWrappers::svn_auth_ssl_server_trust_prompt_func, (void*)_baton->Handle, pool->Handle);

	return provider;
}
#pragma endregion

////////////////////////////////////////////////////////////////
#pragma region // SslClientCertificate handler wrapper

svn_error_t* AuthPromptWrappers::svn_auth_ssl_client_cert_prompt_func(svn_auth_cred_ssl_client_cert_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool)
{
	SvnAuthWrapper<SvnSslClientCertificateEventArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnSslClientCertificateEventArgs^>^>::Get((IntPtr)baton);

	SvnSslClientCertificateEventArgs^ args = gcnew SvnSslClientCertificateEventArgs(SvnBase::Utf8_PtrToString(realm), may_save != 0);

	AprPool tmpPool(pool, false);
	*cred = nullptr;
	try
	{
		wrapper->Raise(args);

	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Authorization handler", e);
	}
	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");
	else if(args->Break)
		return nullptr;


	*cred = (svn_auth_cred_ssl_client_cert_t *)tmpPool.AllocCleared(sizeof(svn_auth_cred_ssl_client_cert_t));

	(*cred)->cert_file = tmpPool.AllocString(args->CertificateFile);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnSslClientCertificateEventArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = nullptr;

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
	SvnAuthWrapper<SvnSslClientCertificatePasswordEventArgs^>^ wrapper = AprBaton<SvnAuthWrapper<SvnSslClientCertificatePasswordEventArgs^>^>::Get((IntPtr)baton);

	SvnSslClientCertificatePasswordEventArgs^ args = gcnew SvnSslClientCertificatePasswordEventArgs(SvnBase::Utf8_PtrToString(realm), may_save != 0);

	AprPool tmpPool(pool, false);
	*cred = nullptr;
	try
	{
		wrapper->Raise(args);

	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Authorization handler", e);
	}
	if(args->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, NULL, "Authorization canceled operation");
	else if(args->Break)
		return nullptr;

	*cred = (svn_auth_cred_ssl_client_cert_pw_t *)tmpPool.AllocCleared(sizeof(svn_auth_cred_ssl_client_cert_pw_t));

	(*cred)->password = tmpPool.AllocString(args->Password);
	(*cred)->may_save = args->Save;

	return nullptr;
}

svn_auth_provider_object_t *SvnSslClientCertificatePasswordEventArgs::Wrapper::GetProviderPtr(AprPool^ pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_auth_provider_object_t *provider = nullptr;

	if(_handler->Equals(SvnAuthentication::SubversionFileSslClientCertificatePasswordHandler))
	{
		svn_auth_get_ssl_client_cert_pw_file_provider(&provider, pool->Handle);
	}
	else
		svn_auth_get_ssl_client_cert_pw_prompt_provider(&provider, &AuthPromptWrappers::svn_auth_ssl_client_cert_pw_prompt_func, (void*)_baton->Handle, RetryLimit, pool->Handle);

	return provider;
}
#pragma endregion



///////////////////////////////
void SvnAuthentication::MaybePrintRealm(SvnAuthorizationEventArgs^ e)
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


void SvnAuthentication::ImpConsoleUsernameHandler(Object ^sender, SvnUsernameEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	MaybePrintRealm(e);

	Console::Write("Username: ");
	e->Username = Console::ReadLine();
}

void SvnAuthentication::ImpConsoleUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
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
}

void SvnAuthentication::ImpConsoleSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	Console::WriteLine("Error validating server certificate for '{0}':", e->Realm);

	if(SvnCertificateTrustFailures::None != (e->Failures & SvnCertificateTrustFailures::UnknownCertificateAuthority))
	{
		Console::WriteLine(" - The certificate is not issued by a trusted authority. Use the\n"
			"   fingerprint to validate the certificate manually!");
	}
	if(SvnCertificateTrustFailures::None != (e->Failures & SvnCertificateTrustFailures::CommonNameMismatch))
	{
		Console::WriteLine(" - The certificate hostname does not match.");
	}
	if(SvnCertificateTrustFailures::None != (e->Failures & SvnCertificateTrustFailures::CertificateNotValidYet))
	{
		Console::WriteLine(" - The certificate is not yet valid.");
	}
	if(SvnCertificateTrustFailures::None != (e->Failures & SvnCertificateTrustFailures::CertificateExpired))
	{
		Console::WriteLine(" - The certificate is has expired.");
	}
	if(SvnCertificateTrustFailures::None != (e->Failures & SvnCertificateTrustFailures::UnknownSslProviderFailure))
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
		bool breakOut = false;
		while(!breakOut)
		{
			Console::Write("(R)eject, accept (t)emporarily or accept (p)ermanently? ");

			ConsoleKeyInfo^ki = Console::ReadKey();
			Console::WriteLine();

			switch(ki->Key)
			{
			case ConsoleKey::Escape:
			case ConsoleKey::R:
				breakOut = true;
				break;
			case ConsoleKey::P:
				e->AcceptedFailures = e->Failures;
				e->Save = e->MaySave;
				return;
			case ConsoleKey::T:
				e->AcceptedFailures = e->Failures;
				return;
			}
		}
		e->Break = true;
	}
	finally
	{
		Console::WriteLine();
	}
}

void SvnAuthentication::ImpConsoleSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	MaybePrintRealm(e);

	Console::Write("Client certificate filename: ");

	e->CertificateFile = Console::ReadLine();
	Console::WriteLine();
}

void SvnAuthentication::ImpConsoleSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	Console::Write("Passphrase for '{0}': ", e->Realm);

	e->Password = ReadPassword();
	Console::WriteLine();
}

void SvnAuthentication::ImpSubversionFileUsernameHandler(Object ^sender, SvnUsernameEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	UNUSED_ALWAYS(e);
	throw gcnew NotImplementedException(SharpSvnStrings::SvnAuthManagedPlaceholder);
}

void SvnAuthentication::ImpSubversionFileUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	UNUSED_ALWAYS(e);
	throw gcnew NotImplementedException(SharpSvnStrings::SvnAuthManagedPlaceholder);
}

void SvnAuthentication::ImpSubversionWindowsFileUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	UNUSED_ALWAYS(e);
	throw gcnew NotImplementedException(SharpSvnStrings::SvnAuthManagedPlaceholder);
}

void SvnAuthentication::ImpSubversionFileSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	UNUSED_ALWAYS(e);
	throw gcnew NotImplementedException(SharpSvnStrings::SvnAuthManagedPlaceholder);
}

void SvnAuthentication::ImpSubversionFileSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	UNUSED_ALWAYS(e);
	throw gcnew NotImplementedException(SharpSvnStrings::SvnAuthManagedPlaceholder);
}

void SvnAuthentication::ImpSubversionFileSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	UNUSED_ALWAYS(e);
	throw gcnew NotImplementedException(SharpSvnStrings::SvnAuthManagedPlaceholder);
}

void SvnAuthentication::ImpSubversionWindowsSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e)
{
	UNUSED_ALWAYS(sender);
	UNUSED_ALWAYS(e);
	throw gcnew NotImplementedException(SharpSvnStrings::SvnAuthManagedPlaceholder);
}
