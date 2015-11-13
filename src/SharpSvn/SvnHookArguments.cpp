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

    return ParseHookArguments(args, hookType, useConsole ? System::Console::In : nullptr, data);
}

bool SvnHookArguments::ParseHookArguments(array<String^>^ args, SvnHookType hookType, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    switch(hookType)
    {
    case SvnHookType::PostCommit:
        return ParsePostCommit(args, consoleData, data);
    case SvnHookType::PostLock:
        return ParsePostLock(args, consoleData, data);
    case SvnHookType::PostRevPropChange:
        return ParsePostRevPropChange(args, consoleData, data);
    case SvnHookType::PostUnlock:
        return ParsePostUnlock(args, consoleData, data);
    case SvnHookType::PreCommit:
        return ParsePreCommit(args, consoleData, data);
    case SvnHookType::PreLock:
        return ParsePreLock(args, consoleData, data);
    case SvnHookType::PreRevPropChange:
        return ParsePreRevPropChange(args, consoleData, data);
    case SvnHookType::PreUnlock:
        return ParsePreUnlock(args, consoleData, data);
    case SvnHookType::StartCommit:
        return ParseStartCommit(args, consoleData, data);
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

bool SvnHookArguments::ParsePostCommit(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    UNUSED_ALWAYS(consoleData);

    data = nullptr;
    if (args->Length < 2)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PostCommit);
    data->RepositoryPath = args[0];
    data->Revision = __int64::Parse(args[1], CultureInfo::InvariantCulture);
    return true;
}

bool SvnHookArguments::ParsePostLock(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    data = nullptr;
    if (args->Length < 2)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PostLock);
    data->RepositoryPath = args[0];
    data->User = args[1];

    if (consoleData)
        data->ReadPathsFromStdIn(consoleData);

    return true;
}

bool SvnHookArguments::ParsePostRevPropChange(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    UNUSED_ALWAYS(consoleData);

    data = nullptr;
    if (args->Length < 5)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PostCommit);
    data->RepositoryPath = args[0];
    data->Revision = __int64::Parse(args[1], CultureInfo::InvariantCulture);
    data->User = args[2];
    data->PropertyName = args[3];
    data->Action = args[4];

    if (consoleData)
        data->PreviousValue = ReadStdInText(consoleData);
    return true;
}

bool SvnHookArguments::ParsePostUnlock(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    data = nullptr;
    if (args->Length < 2)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PostUnlock);
    data->RepositoryPath = args[0];
    data->User = args[1];

    if (consoleData)
        data->ReadPathsFromStdIn(consoleData);

    return true;
}

bool SvnHookArguments::ParsePreCommit(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    UNUSED_ALWAYS(consoleData);

    data = nullptr;
    if (args->Length < 2)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PreCommit);
    data->RepositoryPath = args[0];
    data->TransactionName = args[1];
    return true;
}

bool SvnHookArguments::ParsePreLock(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    UNUSED_ALWAYS(consoleData);

    data = nullptr;
    if (args->Length < 3)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PreLock);
    data->RepositoryPath = args[0];
    data->Path = args[1];
    data->User = args[2];
    return true;
}

bool SvnHookArguments::ParsePreRevPropChange(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    UNUSED_ALWAYS(consoleData);

    data = nullptr;
    if (args->Length < 5)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PreRevPropChange);
    data->RepositoryPath = args[0];
    data->Revision = __int64::Parse(args[1], CultureInfo::InvariantCulture);
    data->User = args[2];
    data->PropertyName = args[3];
    data->Action = args[4];

    if (consoleData)
        data->NewValue = ReadStdInText(consoleData);
    return true;
}

bool SvnHookArguments::ParsePreUnlock(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    UNUSED_ALWAYS(consoleData);

    data = nullptr;
    if (args->Length < 3)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::PreUnlock);
    data->RepositoryPath = args[0];
    data->Path = args[1];
    data->User = args[2];
    return true;
}

bool SvnHookArguments::ParseStartCommit(array<String^>^ args, TextReader ^consoleData, [Out] SvnHookArguments^% data)
{
    if (!args)
        throw gcnew ArgumentNullException("args");

    UNUSED_ALWAYS(consoleData);

    data = nullptr;
    if (args->Length < 3)
        return false;

    data = gcnew SvnHookArguments(SvnHookType::StartCommit);
    data->RepositoryPath = args[0];
    data->User = args[1];
    data->Capabilities = gcnew Collection<String^>(array<String^>::AsReadOnly(args[2]->Split(',')));
    return true;
}

String^ SvnHookArguments::ReadStdInText(TextReader^ consoleData)
{
    return consoleData->ReadToEnd();
}

void SvnHookArguments::ReadPathsFromStdIn(TextReader^ consoleData)
{
    Collection<String^> ^paths = gcnew Collection<String^>();
    String ^line;

    while ((line = consoleData->ReadLine()))
    {
        paths->Add(line);
    }

    Paths = paths;
}
