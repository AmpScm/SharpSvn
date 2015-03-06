// Copyright 2007-2009 The SharpSvn Project
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
#include <svn_version.h>

#include "SvnLibraryAttribute.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;
using System::Diagnostics::CodeAnalysis::SuppressMessageAttribute;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetUuidFromUri(System.Uri,System.Guid&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetRepositoryIdFromUri(System.Uri,System.Guid&)", MessageId="1#")];

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#TryGetRepositoryId(System.String,System.Guid&)")];
[module: SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode", Scope="member", Target="SharpSvn.SvnClient.#set_AdministrativeDirectoryName(System.String)")];

SvnClient::SvnClient()
    : _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
    _clientBaton = gcnew AprBaton<SvnClient^>(this);
    Initialize();
}

SvnClient::~SvnClient()
{
    delete _clientBaton;
}

// Allow changing the AdministrativeDirectory for users willing to take the risks involved.
void SvnClient::AdministrativeDirectoryName::set(String^ value)
{
    if (String::IsNullOrEmpty(value))
        throw gcnew ArgumentNullException("value");

    AprPool pool(SmallThreadPool);

    SVN_THROW(svn_wc_set_adm_dir(pool.AllocString(value), pool.Handle));

    _admDir = svn_wc_get_adm_dir(pool.Handle);
    _administrativeDirName = Utf8_PtrToString(_admDir);
}

struct SvnClientCallBacks
{
    static void __cdecl svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool);

    static svn_error_t * __cdecl svn_wc_conflict_resolver_func(svn_wc_conflict_result_t **result, const svn_wc_conflict_description2_t *description, void *baton, apr_pool_t *result_pool, apr_pool_t *scratch_pool);
};

void SvnClient::Initialize()
{
    void* baton = (void*)_clientBaton->Handle;

    CtxHandle->notify_baton2 = baton;
    CtxHandle->notify_func2 = &SvnClientCallBacks::svn_wc_notify_func2;
    CtxHandle->conflict_baton2 = baton;
    CtxHandle->conflict_func2 = &SvnClientCallBacks::svn_wc_conflict_resolver_func;
}

System::Version^ SvnClient::Version::get()
{
    const svn_version_t* version = svn_client_version();

    return gcnew System::Version(version->major, version->minor, version->patch);
}

System::Version^ SvnClient::SharpSvnVersion::get()
{
    return (gcnew System::Reflection::AssemblyName(SvnClient::typeid->Assembly->FullName))->Version;
}

String^ SvnClient::VersionString::get()
{
    return SVN_VER_NUM SVN_VER_NUMTAG;
}

void SvnClient::HandleClientCancel(SvnCancelEventArgs^ e)
{
    __super::HandleClientCancel(e);

    if (!e->Cancel)
        OnCancel(e);
}

void SvnClient::OnCancel(SvnCancelEventArgs^ e)
{
    Cancel(this, e);
}

void SvnClient::HandleClientProgress(SvnProgressEventArgs^ e)
{
    __super::HandleClientProgress(e);

    OnProgress(e);
}

void SvnClient::OnProgress(SvnProgressEventArgs^ e)
{
    Progress(this, e);
}

void SvnClient::HandleClientCommitting(SvnCommittingEventArgs^ e)
{
    __super::HandleClientCommitting(e);

    if (! e->Cancel)
        OnCommitting(e);
}

void SvnClient::OnCommitting(SvnCommittingEventArgs^ e)
{
    Committing(this, e);
}

void SvnClient::HandleClientCommitted(SvnCommittedEventArgs^ e)
{
    __super::HandleClientCommitted(e);

    OnCommitted(e);
}

void SvnClient::OnCommitted(SvnCommittedEventArgs^ e)
{
    Committed(this, e);
}

void SvnClient::HandleClientNotify(SvnNotifyEventArgs^ e)
{
    __super::HandleClientNotify(e);

    OnNotify(e);
}

void SvnClient::OnNotify(SvnNotifyEventArgs^ e)
{
    Notify(this, e);
}

void SvnClient::HandleClientConflict(SvnConflictEventArgs^ e)
{
    if (e->Cancel)
        return;

    SvnClientArgsWithConflict^ conflictArgs = dynamic_cast<SvnClientArgsWithConflict^>(CurrentCommandArgs); // C#: _currentArgs as SvnClientArgsWithConflict

    if (conflictArgs)
    {
        conflictArgs->RaiseConflict(e);

        if (e->Cancel)
            return;
    }

    OnConflict(e);
}

void SvnClient::OnConflict(SvnConflictEventArgs^ e)
{
    Conflict(this, e);
}

void SvnClient::HandleClientError(SvnErrorEventArgs^ e)
{
    __super::HandleClientError(e);

    if (! e->Cancel)
        OnSvnError(e);
}

void SvnClient::OnSvnError(SvnErrorEventArgs^ e)
{
    SvnError(this, e);
}

void SvnClient::HandleProcessing(SvnProcessingEventArgs^ e)
{
    __super::HandleProcessing(e);

    OnProcessing(e);
}

void SvnClient::OnProcessing(SvnProcessingEventArgs^ e)
{
    Processing(this, e);
}


void SvnClientCallBacks::svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool)
{
    SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);
    AprPool aprPool(pool, false);

    SvnNotifyEventArgs^ ea = gcnew SvnNotifyEventArgs(notify, client->CurrentCommandArgs->CommandType, %aprPool);

    try
    {
        client->HandleClientNotify(ea);
    }
    finally
    {
        ea->Detach(false);
    }
}

svn_error_t* SvnClientCallBacks::svn_wc_conflict_resolver_func(svn_wc_conflict_result_t **result, const svn_wc_conflict_description2_t *description, void *baton, apr_pool_t *result_pool, apr_pool_t *scratch_pool)
{
    SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

    *result = svn_wc_create_conflict_result(svn_wc_conflict_choose_postpone, NULL, result_pool);

    AprPool resultPool(result_pool, false); // Connect to parent pool
    AprPool scratchPool(scratch_pool, false); // Connect to parent pool

    SvnConflictEventArgs^ ea = gcnew SvnConflictEventArgs(description, %scratchPool);

    try
    {
        client->HandleClientConflict(ea);

        if (ea->Cancel)
            return svn_error_create(SVN_ERR_CANCELLED, nullptr, "Operation canceled from OnConflict");

        if (ea->Choice != SvnAccept::Postpone && ea->MergedFile)
        {
            (*result)->choice = (svn_wc_conflict_choice_t)ea->Choice;

            if (ea->Choice == SvnAccept::Merged)
                (*result)->merged_file = resultPool.AllocDirent(ea->MergedFile);
        }

        return nullptr;
    }
    catch(Exception^ e)
    {
        return SvnException::CreateExceptionSvnError("Conflict resolver", e);
    }
    finally
    {
        ea->Detach(false);
    }
}

Uri^ SvnClient::GetUriFromWorkingCopy(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!SvnBase::IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    AprPool pool(%_pool);
    NoArgsStore store(this, %pool);

    const char* url = nullptr;

    svn_error_t* err = svn_client_url_from_path2(&url, pool.AllocAbsoluteDirent(path), CtxHandle, pool.Handle, pool.Handle);

    if (!err && url)
        return Utf8_PtrToUri(url, System::IO::Directory::Exists(path) ? SvnNodeKind::Directory : SvnNodeKind::File);
    else if (err)
        svn_error_clear(err);

    return nullptr;
}

bool SvnClient::TryGetRepositoryId(Uri^ uri, [Out] Guid% id)
{
    if (!uri)
        throw gcnew ArgumentNullException("uri");
    else if (!SvnBase::IsValidReposUri(uri))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

    id = Guid::Empty;

    EnsureState(SvnContextState::AuthorizationInitialized);

    AprPool pool(%_pool);
    NoArgsStore store(this, %pool);

    const char* uuidStr = nullptr;

    svn_error_t* err = svn_client_get_repos_root(NULL, &uuidStr,
        pool.AllocUri(uri),
        CtxHandle,
        pool.Handle, pool.Handle);

    if (err || !uuidStr || !*uuidStr)
    {
        if (err)
            svn_error_clear(err);
        return false;
    }
    else
    {
        id = Guid(Utf8_PtrToString(uuidStr));
        return true;
    }
}

bool SvnClient::TryGetRepositoryId(String^ path, [Out] Guid% id)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!SvnBase::IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    Uri^ uri;

    return TryGetRepository(path, uri, id);
}

Uri^ SvnClient::GetRepositoryRoot(Uri^ uri)
{
    if (!uri)
        throw gcnew ArgumentNullException("uri");
    else if (!SvnBase::IsValidReposUri(uri))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

    const char* resultUrl = nullptr;
    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);
    NoArgsStore store(this, %pool);

    svn_error_t* err = svn_client_get_repos_root(&resultUrl, nullptr,
        pool.AllocUri(uri),
        CtxHandle, pool.Handle, pool.Handle);

    if (!err && resultUrl)
        return Utf8_PtrToUri(resultUrl, SvnNodeKind::Directory);
    else if (err)
        svn_error_clear(err);

    return nullptr;
}

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.GetRepositoryRoot(System.String):System.Uri")];

Uri^ SvnClient::GetRepositoryRoot(String^ target)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (!IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

    Uri^ reposRoot;
    Guid id;

    if (TryGetRepository(target, reposRoot, id))
        return reposRoot;

    return nullptr;
}

bool SvnClient::TryGetRepository(Uri^ uri, [Out] Uri^% repositoryUrl, [Out] Guid% id)
{
    if (!uri)
        throw gcnew ArgumentNullException("uri");
    else if (!SvnBase::IsValidReposUri(uri))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

    id = Guid::Empty;
    repositoryUrl = nullptr;

    EnsureState(SvnContextState::AuthorizationInitialized);

    AprPool pool(%_pool);
    NoArgsStore store(this, %pool);

    const char* urlStr = nullptr;
    const char* uuidStr = nullptr;
    svn_error_t *err;

    err = svn_client_get_repos_root(&urlStr, &uuidStr,
                                    pool.AllocUri(uri),
                                    CtxHandle, pool.Handle, pool.Handle);

    if (err)
    {
        svn_error_clear(err);
        return false;
    }

    repositoryUrl = Utf8_PtrToUri(urlStr, SvnNodeKind::Directory);
    id = Guid(Utf8_PtrToString(uuidStr));
    return true;
}

bool SvnClient::TryGetRepository(String^ path, [Out] Uri^% repositoryUrl, [Out] Guid% id)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!SvnBase::IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    id = Guid::Empty;
    repositoryUrl = nullptr;

    EnsureState(SvnContextState::ConfigLoaded);

    AprPool pool(%_pool);
    NoArgsStore store(this, %pool);

    const char* urlStr = nullptr;
    const char* uuidStr = nullptr;
    svn_error_t *err;

    err = svn_client_get_repos_root(&urlStr, &uuidStr,
                                    pool.AllocAbsoluteDirent(path),
                                    CtxHandle, pool.Handle, pool.Handle);

    if (err || !urlStr || !uuidStr || !*urlStr || !*uuidStr)
    {
        svn_error_clear(err);
        return false;
    }

    repositoryUrl = Utf8_PtrToUri(urlStr, SvnNodeKind::Directory);
    id = Guid(Utf8_PtrToString(uuidStr));
    return true;
}

String^ SvnClient::GetWorkingCopyRoot(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    const char* wcroot_abspath = nullptr;
    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    NoArgsStore store(this, %pool);

    svn_error_t* err = svn_client_get_wc_root(&wcroot_abspath, pool.AllocAbsoluteDirent(path), CtxHandle, pool.Handle, pool.Handle);

    if (!err && wcroot_abspath)
        return Utf8_PathPtrToString(wcroot_abspath, %pool);
    else if (err)
        svn_error_clear(err);

    return nullptr;
}

void SvnClient::AddClientName(String^ name, System::Version^ version)
{
    if (String::IsNullOrEmpty(name))
        throw gcnew ArgumentNullException("name");
    else if (!version)
        throw gcnew ArgumentNullException("version");

    for (int i = 0; i < name->Length; i++)
    {
        if (!wchar_t::IsLetterOrDigit(name, i) && 0 > (((String^)"._ ")->IndexOf(name[i])))
            throw gcnew ArgumentException(SharpSvnStrings::InvalidCharacterInClientName, "name");
    }

    if (!_clientNames->Contains(name))
    {
        _clientNames->Add(name);

        _clientName += " " + name + "/" + version->ToString();
    }
}

const char* SvnClient::GetEolPtr(SvnLineStyle style)
{
    switch(style)
    {
    case SvnLineStyle::Default:
    case SvnLineStyle::Native:
        return nullptr;
    case SvnLineStyle::CarriageReturnLinefeed:
        return "CRLF";
    case SvnLineStyle::Linefeed:
        return "LF";
    case SvnLineStyle::CarriageReturn:
        return "CR";
    default:
        throw gcnew ArgumentOutOfRangeException("style");
    }
}

const char* SvnClient::GetEolValue(SvnLineStyle style)
{
    switch(style)
    {
    case SvnLineStyle::Default:
        return nullptr;
    case SvnLineStyle::Native:
        return APR_EOL_STR;
    case SvnLineStyle::CarriageReturnLinefeed:
        return "\r\n";
    case SvnLineStyle::Linefeed:
        return "\n";
    case SvnLineStyle::CarriageReturn:
        return "\r";
    default:
        throw gcnew ArgumentOutOfRangeException("style");
    }
}

using SharpSvn::Implementation::SvnLibrary;
using SharpSvn::Implementation::SvnLibraryAttribute;

static int CompareLibrary(SvnLibrary^ x, SvnLibrary^ y)
{
    return StringComparer::OrdinalIgnoreCase->Compare(x->Name, y->Name);
}

ICollection<SvnLibrary^>^ SvnClient::SvnLibraries::get()
{
    if (_svnLibraries)
        return _svnLibraries;

    System::Collections::Generic::List<SvnLibrary^>^ libs
        = gcnew System::Collections::Generic::List<SvnLibrary^>();

    for each(SvnLibraryAttribute^ i in SvnClient::typeid->Assembly->GetCustomAttributes(SvnLibraryAttribute::typeid, false))
    {
        libs->Add(gcnew SvnLibrary(i));
    }

    libs->Sort(gcnew Comparison<SvnLibrary^>(CompareLibrary));

    return _svnLibraries = libs->AsReadOnly();
}
