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

#include "Args/Log.h"

#include <private/svn_sorts_private.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.String,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.Uri,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.String,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.Uri,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")]
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")]
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#Log(System.String,SharpSvn.SvnLogArgs,System.EventHandler`1<SharpSvn.SvnLogEventArgs>)")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.String,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)")];
[module: SuppressMessage("Microsoft.Usage", "CA2234:PassSystemUriObjectsInsteadOfStrings", Scope="member", Target="SharpSvn.SvnClient.#Log(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnLogArgs,System.EventHandler`1<SharpSvn.SvnLogEventArgs>)")];

SvnChangeItemCollection^ SvnLoggingEventArgs::ChangedPaths::get()
{
    if (!_changedPaths && _entry && _entry->changed_paths2 && _pool)
    {
        apr_array_header_t *sorted_paths;
        _changedPaths = gcnew SvnChangeItemCollection();

        /* Get an array of sorted hash keys. */
        sorted_paths = svn_sort__hash(_entry->changed_paths2,
            svn_sort_compare_items_as_paths, _pool->Handle);

        for (int i = 0; i < sorted_paths->nelts; i++)
        {
            const svn_sort__item_t *item = &(APR_ARRAY_IDX(sorted_paths, i,
                svn_sort__item_t));
            const char *path = (const char *)item->key;
            const svn_log_changed_path2_t *pChangeInfo = (const svn_log_changed_path2_t *)item->value;

            SvnChangeItem^ ci = gcnew SvnChangeItem(SvnBase::Utf8_PtrToString(path), pChangeInfo);

            _changedPaths->Add(ci);
        }

        if (_changedPaths->Count)
        {
            _changeItemsToDetach = gcnew array<SvnChangeItem^>(_changedPaths->Count);
            _changedPaths->CopyTo(_changeItemsToDetach, 0);
        }
    }

    return _changedPaths;
}



static String^ UriToStringCanonical(Uri^ value)
{
    if (!value)
        return nullptr;

    String^ name = SvnBase::UriToString(value);

    if (name && name->Length && (name[name->Length-1] == '/'))
        return name->TrimEnd('/'); // "svn://host:port" is canoncialized to "svn://host:port/" by the .Net Uri class
    else
        return name;
}


bool SvnClient::Log(String^ targetPath, EventHandler<SvnLogEventArgs^>^ logHandler)
{
    if (String::IsNullOrEmpty(targetPath))
        throw gcnew ArgumentNullException("targetPath");
    return Log(targetPath, gcnew SvnLogArgs(), logHandler);
}

bool SvnClient::Log(Uri^ target, EventHandler<SvnLogEventArgs^>^ logHandler)
{
    if (!target)
        throw gcnew ArgumentNullException("target");

    return Log(target, gcnew SvnLogArgs(), logHandler);
}


bool SvnClient::Log(Uri^ target, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    return InternalLog(NewSingleItemCollection(UriToStringCanonical(target)), nullptr, SvnRevision::Head, args, logHandler);
}

bool SvnClient::Log(String^ targetPath, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
    if (String::IsNullOrEmpty(targetPath))
        throw gcnew ArgumentNullException("targetPath");
    else if (!IsNotUri(targetPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPath");

    return InternalLog(NewSingleItemCollection(
        SvnPathTarget::GetTargetPath(targetPath)->Replace(System::IO::Path::DirectorySeparatorChar, '/')),
        nullptr, SvnRevision::Base, args, logHandler);
}

bool SvnClient::Log(ICollection<Uri^>^ targets, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
    if (!targets)
        throw gcnew ArgumentNullException("targets");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!targets->Count)
        throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "targets");

    Uri^ first = nullptr;
    String^ root = nullptr;
    bool moreThanOne = false;
    for each (Uri^ uri in targets)
    {
        if (!uri)
            throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");

        if (!uri->IsAbsoluteUri)
        {
            if (args->BaseUri)
                uri = gcnew Uri(args->BaseUri, uri);

            if (!uri->IsAbsoluteUri)
                throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "targets");
        }

        if (!first)
        {
            first = uri;
            root = uri->AbsolutePath->TrimEnd('/');
            continue;
        }
        else
            moreThanOne = true;

        if (Uri::Compare(uri, first, UriComponents::HostAndPort | UriComponents::Scheme | UriComponents::StrongAuthority, UriFormat::UriEscaped, StringComparison::Ordinal))
            throw gcnew ArgumentException(SharpSvnStrings::AllUrisMustBeOnTheSameServer, "targets");

        String^ itemPath = uri->AbsolutePath->TrimEnd('/');

        int nEnd = Math::Min(root->Length, itemPath->Length);

        while (nEnd >= 0 && String::Compare(root, 0, itemPath, 0, nEnd, StringComparison::Ordinal))
        {
            nEnd = root->LastIndexOf('/', nEnd-1);
        }

        if (nEnd == root->Length && itemPath->Length > root->Length && itemPath[nEnd] != '/')
        {               // root can't end in a '/', as that would have been trimmed !

            nEnd = root->LastIndexOf('/', nEnd-1);
        }

        if (nEnd >= root->Length - 1)
        {}
        else
        {
            while (nEnd > 1 && nEnd < root->Length && root[nEnd-1] == '/')
                nEnd--;

            if (nEnd > 1)
                root = root->Substring(0, nEnd);
            else
                root = "/";
        }
    }

    if (moreThanOne && !root->EndsWith("/", StringComparison::Ordinal))
        root += "/";

    System::Collections::Generic::List<String^>^ rawTargets = gcnew System::Collections::Generic::List<String^>();
    Uri^ rootUri = gcnew Uri(first, Uri::UnescapeDataString(root)); // URI .ctor escapes the 2nd parameter
    if (moreThanOne)
    {
        // Invoke with primary url followed by relative subpaths
        AprPool tmpPool(%_pool);
        AprArray<System::Uri^, AprUriMarshaller^> uriArray(targets, %tmpPool);
        const char *pCommon;
        apr_array_header_t *pCondensed;

        SVN_THROW(svn_uri_condense_targets(&pCommon, &pCondensed, uriArray.Handle, true, tmpPool.Handle, tmpPool.Handle));

        rawTargets->Add(SvnBase::Utf8_PtrToString(pCommon));;

        for (int i = 0; i < pCondensed->nelts; i++)
            rawTargets->Add(SvnBase::Utf8_PtrToString(APR_ARRAY_IDX(pCondensed, i, const char *)));
    }
    else
    {
        rootUri = nullptr;
        rawTargets->Add(UriToStringCanonical(first));
    }

    return InternalLog(static_cast<ICollection<String^>^>(rawTargets), rootUri, SvnRevision::Head, args, logHandler);
}

bool SvnClient::Log(ICollection<String^>^ targetPaths, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
    if (!targetPaths)
        throw gcnew ArgumentNullException("targetPaths");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!targetPaths->Count)
        throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "targetPaths");

    // This overload is not supported by the Subversion runtime at this time. We fake it useing the client api
    Uri^ first = nullptr;

    System::Collections::Generic::List<Uri^>^ targetsUris = gcnew System::Collections::Generic::List<Uri^>();

    for each (String^ path in targetPaths)
    {
        if (!path)
            throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targetPaths");
        else if (!IsNotUri(path))
            throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPaths");

        Uri^ uri = GetUriFromWorkingCopy(path);

        if (!uri)
        {
            ArgsStore store(this, args, nullptr);

            // We must provide some kind of svn error, to eventually replace this method with real svn client code
            return args->HandleResult(this, svn_error_create(SVN_ERR_WC_NOT_DIRECTORY, nullptr, nullptr), targetPaths);
        }

        if (!first)
            first = uri;
        else if (Uri::Compare(uri, first, UriComponents::HostAndPort | UriComponents::Scheme | UriComponents::StrongAuthority, UriFormat::UriEscaped, StringComparison::Ordinal))
        {
            ArgsStore store(this, args, nullptr);

            // We must provide some kind of svn error, to eventually replace this method with real svn client code
            return args->HandleResult(this, svn_error_create(SVN_ERR_WC_BAD_PATH, nullptr, "Working copy paths must be in the same repository"), targetPaths);

            // TODO: Give some kind of meaningfull error. We just ignore other repository paths in logging now
            continue;
        }

        targetsUris->Add(uri);
    }

    return Log(targetsUris, args, logHandler);
}

static svn_error_t *svnclient_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
    SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

    SvnLogArgs^ args = dynamic_cast<SvnLogArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnLogArgs
    AprPool aprPool(pool, false);
    if (!args)
        return nullptr;

    if (log_entry->revision == SVN_INVALID_REVNUM)
    {
        // This marks the end of logs at this level,
        args->_mergeLogLevel--;
        return nullptr;
    }

    SvnLogEventArgs^ e = gcnew SvnLogEventArgs(log_entry, args->_mergeLogLevel, args->_searchRoot, %aprPool);

    if (log_entry->has_children)
        args->_mergeLogLevel++;

    try
    {
        args->OnLog(e);

        if (e->Cancel)
            return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Log receiver canceled operation");
        else
            return nullptr;
    }
    catch(Exception^ ex)
    {
        return SvnException::CreateExceptionSvnError("Log receiver", ex);
    }
    finally
    {
        e->Detach(false);
    }
}

bool SvnClient::InternalLog(ICollection<String^>^ targets, Uri^ searchRoot, SvnRevision^ altPegRev, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
    if (!targets)
        throw gcnew ArgumentNullException("targets");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    args->_mergeLogLevel = 0; // Clear log level
    args->_searchRoot = searchRoot;
    if (logHandler)
        args->Log += logHandler;
    try
    {
        apr_array_header_t* retrieveProperties;

        if (args->RetrieveAllProperties)
            retrieveProperties = nullptr;
        else if (args->RetrievePropertiesUsed)
            retrieveProperties = AllocArray(args->RetrieveProperties, %pool);
        else
            retrieveProperties = svn_compat_log_revprops_in(pool.Handle);

        svn_opt_revision_t pegRev = args->OperationalRevision->Or(altPegRev)->ToSvnRevision();

        apr_array_header_t *revision_ranges;
        {
            int count = args->RangesUsed ? args->Ranges->Count : 1;
            revision_ranges = apr_array_make(pool.Handle, count, sizeof(svn_opt_revision_range_t *));

            if (args->RangesUsed)
                for each(SvnRevisionRange ^ r in args->Ranges)
                {
                    svn_opt_revision_range_t *range =
                        (svn_opt_revision_range_t*)pool.Alloc(sizeof(svn_opt_revision_range_t));

                    range->start = r->StartRevision->Or(SvnRevision::Head)->ToSvnRevision();
                    range->end = r->EndRevision->Or(SvnRevision::Zero)->ToSvnRevision();

                    APR_ARRAY_PUSH(revision_ranges, svn_opt_revision_range_t *) = range;
                }
            else
            {
                svn_opt_revision_range_t *range =
                    (svn_opt_revision_range_t*)pool.Alloc(sizeof(svn_opt_revision_range_t));

                range->start = args->Start->Or(args->OperationalRevision)->Or(SvnRevision::Head)->ToSvnRevision();
                range->end = args->End->Or(SvnRevision::Zero)->ToSvnRevision();

                APR_ARRAY_PUSH(revision_ranges, svn_opt_revision_range_t *) = range;
            }
        }

        svn_error_t *r = svn_client_log5(
            AllocArray(targets, %pool),
            &pegRev,
            revision_ranges,
            args->Limit,
            args->RetrieveChangedPaths,
            args->StrictNodeHistory,
            args->RetrieveMergedRevisions,
            retrieveProperties,
            svnclient_log_handler,
            (void*)_clientBaton->Handle,
            CtxHandle,
            pool.Handle);

        return args->HandleResult(this, r, targets);
    }
    finally
    {
        if (logHandler)
            args->Log -= logHandler;

        args->_searchRoot = nullptr;
        args->_mergeLogLevel = 0;
    }
}

bool SvnClient::GetLog(String^ targetPath, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
    if (String::IsNullOrEmpty(targetPath))
        throw gcnew ArgumentNullException("targetPath");

    InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

    try
    {
        return Log(targetPath, results->Handler);
    }
    finally
    {
        logItems = results;
    }
}

bool SvnClient::GetLog(Uri^ target, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
    if (!target)
        throw gcnew ArgumentNullException("target");

    InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

    try
    {
        return Log(target, results->Handler);
    }
    finally
    {
        logItems = results;
    }
}

bool SvnClient::GetLog(Uri^ target, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

    try
    {
        return Log(target, args, results->Handler);
    }
    finally
    {
        logItems = results;
    }
}

bool SvnClient::GetLog(String^ targetPath, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
    if (!targetPath)
        throw gcnew ArgumentNullException("targetPath");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

    try
    {
        return Log(targetPath, args, results->Handler);
    }
    finally
    {
        logItems = results;
    }
}


bool SvnClient::GetLog(ICollection<Uri^>^ targets, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
    if (!targets)
        throw gcnew ArgumentNullException("targets");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

    try
    {
        return Log(targets, args, results->Handler);
    }
    finally
    {
        logItems = results;
    }
}

bool SvnClient::GetLog(ICollection<String^>^ targetPaths, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
    if (!targetPaths)
        throw gcnew ArgumentNullException("targetPaths");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

    try
    {
        return Log(targetPaths, args, results->Handler);
    }
    finally
    {
        logItems = results;
    }
}

SvnRevisionPropertyNameCollection::SvnRevisionPropertyNameCollection(bool initialEmpty)
: KeyedCollection<String^,String^>(StringComparer::Ordinal, 10) // Start using hashtable at 10 items
{
    if (!initialEmpty)
    {
        Add(SVN_PROP_REVISION_AUTHOR);
        Add(SVN_PROP_REVISION_DATE);
        Add(SVN_PROP_REVISION_LOG);
    }
}

void SvnRevisionPropertyNameCollection::AddDefaultProperties()
{
    if (!Contains(SVN_PROP_REVISION_AUTHOR))
        Add(SVN_PROP_REVISION_AUTHOR);

    if (!Contains(SVN_PROP_REVISION_DATE))
        Add(SVN_PROP_REVISION_DATE);

    if (!Contains(SVN_PROP_REVISION_LOG))
        Add(SVN_PROP_REVISION_LOG);
}

SvnRevisionPropertyNameCollection^ SvnLogArgs::RetrieveProperties::get()
{
    if (!_retrieveProperties)
    {
        _retrieveProperties = gcnew SvnRevisionPropertyNameCollection(false);
    }

    return _retrieveProperties;
}
