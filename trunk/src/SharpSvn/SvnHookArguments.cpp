// $Id: SvnTools.cpp 631 2008-07-28 21:40:00Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"
#include "SvnHookArguments.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using System::Text::StringBuilder;
using namespace System::IO;

bool SvnHookArguments::ParsePostCommitArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 2)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->Revision = __int64::Parse(args[1]);
	return true;
}

bool SvnHookArguments::ParsePostLockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 2)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->User = args[1];
	return true;
}

bool SvnHookArguments::ParsePostRevPropChangeArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 5)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->Revision = __int64::Parse(args[1]);
	data->User = args[2];
	data->PropertyName = args[3];
	data->Action = args[4];

	if (useConsole)
		data->PreviousValue = ReadStdInText();
	return true;
}

bool SvnHookArguments::ParsePostUnlockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 2)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->User = args[1];
	return true;
}

bool SvnHookArguments::ParsePreCommitArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 2)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->TransactionName = args[1];
	return true;
}

bool SvnHookArguments::ParsePreLockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 3)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->Path = args[1];
	data->User = args[2];
	return true;
}

bool SvnHookArguments::ParsePreRevPropChangeArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 5)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->Revision = __int64::Parse(args[1]);
	data->User = args[2];
	data->PropertyName = args[3];
	data->Action = args[4];

	if (useConsole)
		data->NewValue = ReadStdInText();
	return true;
}

bool SvnHookArguments::ParsePreUnlockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 3)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->Path = args[1];
	data->User = args[2];
	return true;
}

bool SvnHookArguments::ParseStartCommitArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	UNUSED_ALWAYS(useConsole);

	data = nullptr;
	if (args->Length < 3)
		return false;

	data = gcnew SvnHookArguments();
	data->RepositoryPath = args[0];
	data->User = args[1];
	data->Capabilities = array<String^>::AsReadOnly(args[2]->Split(','));
	return true;
}

String^ SvnHookArguments::ReadStdInText()
{
	return "";
}