#include "stdafx.h"

#include "SvnAll.h"
#include "SvnObjBaton.h"
#include "SvnAuthentication.h"

using namespace TurtleSvn;
using namespace TurtleSvn::Apr;
using namespace TurtleSvn::Security;

void SvnAuthentication::Clear()
{
	_wrappers->Clear();
	_handlers->Clear();
}

void SvnAuthentication::AddSubversionFileHandlers()
{
	UsernameHandlers						+= SubversionFileUsernameHandler;
	UsernamePasswordHandlers				+= SubversionFileUsernamePasswordHandler;
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


void SvnAuthentication::ImpDialogUsernameHandler(Object ^sender, SvnUsernameArgs^ e)
{
	throw gcnew NotImplementedException();
}
void SvnAuthentication::ImpDialogUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e)
{
	throw gcnew NotImplementedException();
}
void SvnAuthentication::ImpDialogSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e)
{
	throw gcnew NotImplementedException();
}

void SvnAuthentication::ImpDialogSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e)
{
	throw gcnew NotImplementedException();
}

void SvnAuthentication::ImpDialogSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e)
{
	throw gcnew NotImplementedException();
}

void SvnAuthentication::ImpConsoleUsernameHandler(Object ^sender, SvnUsernameArgs^ e)
{
	throw gcnew NotImplementedException();
}

void SvnAuthentication::ImpConsoleUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e)
{
	throw gcnew NotImplementedException();
}

void SvnAuthentication::ImpConsoleSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e)
{
	throw gcnew NotImplementedException();
}

void SvnAuthentication::ImpConsoleSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e)
{
	throw gcnew NotImplementedException();
}

void SvnAuthentication::ImpConsoleSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e)
{
	throw gcnew NotImplementedException();
}