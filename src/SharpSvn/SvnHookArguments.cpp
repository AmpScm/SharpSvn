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
#include "SvnHookArguments.h"

[module: SuppressMessage("Microsoft.Usage", "CA1801:ReviewUnusedParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParsePreLock(System.String[],System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="useConsole")];
[module: SuppressMessage("Microsoft.Usage", "CA1801:ReviewUnusedParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParseStartCommit(System.String[],System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="useConsole")];
[module: SuppressMessage("Microsoft.Usage", "CA1801:ReviewUnusedParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParsePreUnlock(System.String[],System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="useConsole")];
[module: SuppressMessage("Microsoft.Usage", "CA1801:ReviewUnusedParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParsePostLock(System.String[],System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="useConsole")];
[module: SuppressMessage("Microsoft.Usage", "CA1801:ReviewUnusedParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParsePostCommit(System.String[],System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="useConsole")];
[module: SuppressMessage("Microsoft.Usage", "CA1801:ReviewUnusedParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParsePreCommit(System.String[],System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="useConsole")];
[module: SuppressMessage("Microsoft.Usage", "CA1801:ReviewUnusedParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParsePostUnlock(System.String[],System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="useConsole")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnHookArguments.#ParseHookArguments(System.String[],SharpSvn.SvnHookType,System.Boolean,SharpSvn.SvnHookArguments&)", MessageId="3#")];

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using System::Text::StringBuilder;
using System::Globalization::CultureInfo;
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

	return SvnTools::PathCombine(path, "hooks\\" + name);
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
	data->Revision = __int64::Parse(args[1], CultureInfo::InvariantCulture);
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
	data->Revision = __int64::Parse(args[1], CultureInfo::InvariantCulture);
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
	data->Revision = __int64::Parse(args[1], CultureInfo::InvariantCulture);
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
	data->Capabilities = gcnew Collection<String^>(array<String^>::AsReadOnly(args[2]->Split(',')));
	return true;
}

String^ SvnHookArguments::ReadStdInText()
{
	return System::Console::In->ReadToEnd();
}