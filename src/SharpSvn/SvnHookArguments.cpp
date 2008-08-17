// $Id$
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



bool SvnHookArguments::ParseHookArguments(array<String^>^ args, SvnHookType hookType, bool useConsole, [Out] SvnHookArguments^% data)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	switch(hookType)
	{
	case SvnHookType::PostCommit:
		return ParsePostCommit(args, useConsole, data);
	case SvnHookType::PostLock:
		return ParsePostLock(args, useConsole, data);
	case SvnHookType::PostRevPropChange:
		return ParsePostRevPropChange(args, useConsole, data);
	case SvnHookType::PostUnlock:
		return ParsePostUnlock(args, useConsole, data);
	case SvnHookType::PreCommit:
		return ParsePreCommit(args, useConsole, data);
	case SvnHookType::PreLock:
		return ParsePreLock(args, useConsole, data);
	case SvnHookType::PreRevPropChange:
		return ParsePreRevPropChange(args, useConsole, data);
	case SvnHookType::PreUnlock:
		return ParsePreUnlock(args, useConsole, data);	
	case SvnHookType::StartCommit:
		return ParseStartCommit(args, useConsole, data);
	default:
		throw gcnew ArgumentOutOfRangeException("hookType", hookType, "Invalid hooktype passed");
	}
}

String^ SvnHookArguments::GetHookFileName(String^ path, SvnHookType hookType)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	String^ name;

	switch(hookType)
	{
	case SvnHookType::PostCommit:
		name = "post-commit";
		break;
	case SvnHookType::PostLock:
		name = "post-lock";
		break;
	case SvnHookType::PostRevPropChange:
		name = "post-revprop-change";
		break;
	case SvnHookType::PostUnlock:
		name = "post-unlock";
		break;
	case SvnHookType::PreCommit:
		name = "pre-commit";
		break;
	case SvnHookType::PreLock:
		name = "pre-lock";
		break;
	case SvnHookType::PreRevPropChange:
		name = "pre-revprop-change";
		break;
	case SvnHookType::PreUnlock:
		name = "pre-unlock";
		break;
	case SvnHookType::StartCommit:
		name = "start-commit";
		break;
	default:
		throw gcnew ArgumentOutOfRangeException("hookType", hookType, "Invalid hooktype passed");
	}

	return System::IO::Path::Combine(path, "hooks\\" + name);
}

bool SvnHookArguments::ParsePostCommit(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParsePostLock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParsePostRevPropChange(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParsePostUnlock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParsePreCommit(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParsePreLock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParsePreRevPropChange(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParsePreUnlock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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

bool SvnHookArguments::ParseStartCommit(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data)
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
	return System::Console::In->ReadToEnd();
}