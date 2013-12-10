#include "stdafx.h"

#include "Args/InheritedPropertyList.h"

using namespace SharpSvn;

bool SvnClient::InheritedPropertyList(String^ path, EventHandler<SvnInheritedPropertyListEventArgs^>^ listHandler)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return InheritedPropertyList(path, gcnew SvnInheritedPropertyListArgs(), listHandler);
}

static svn_error_t *iprop_list(void *baton, const char *path, apr_hash_t *prop_hash, apr_array_header_t *inherited_props, apr_pool_t *scratch_pool)
{
    SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

    AprPool thePool(scratch_pool, false);

    SvnInheritedPropertyListArgs^ args = dynamic_cast<SvnInheritedPropertyListArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
    if (!args)
        return nullptr;
    try
    {
        if (prop_hash)
        {
            SvnInheritedPropertyListEventArgs ^e = gcnew SvnInheritedPropertyListEventArgs(path, prop_hash, 0, %thePool);
            try
            {
                args->OnList(e);

                if (e->Cancel)
                    return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
            }
            finally
            {
                e->Detach(false);
            }
        }

        if (inherited_props)
        {
            // Properties on one or more ancestors
            int n = 1;
            for (int i = inherited_props->nelts-1; i >= 0; i--)
            {
                const svn_prop_inherited_item_t *pii = APR_ARRAY_IDX(inherited_props, i, const svn_prop_inherited_item_t *);

                SvnInheritedPropertyListEventArgs ^e = gcnew SvnInheritedPropertyListEventArgs(pii->path_or_url, pii->prop_hash, n++, %thePool);
                try
                {
                    args->OnList(e);

                    if (e->Cancel)
                        return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
                }
                finally
                {
                    e->Detach(false);
                }
            }
        }

        return nullptr;
    }
    catch(Exception^ ex)
    {
        return SvnException::CreateExceptionSvnError("List receiver", ex);
    }
}

bool SvnClient::InheritedPropertyList(String^ path, SvnInheritedPropertyListArgs^ args, EventHandler<SvnInheritedPropertyListEventArgs^>^ listHandler)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    if (listHandler)
        args->List += listHandler;
    try
    {
        svn_opt_revision_t rev  = {svn_opt_revision_unspecified};

        svn_error_t* r = svn_client_proplist4(pool.AllocAbsoluteDirent(path),
                                              &rev,
                                              &rev,
                                              svn_depth_empty,
                                              nullptr /* changelist */,
                                              TRUE /* get_inherited_props */,
                                              iprop_list,
                                              (void*)_clientBaton->Handle,
                                              CtxHandle,
                                              pool.Handle);

        return args->HandleResult(this, r, path);
    }
    finally
    {
        if (listHandler)
            args->List -= listHandler;
    }
}

bool SvnClient::GetInheritedPropertyList(String^ path, [Out] Collection<SvnInheritedPropertyListEventArgs^>^% list)
{
    if (!path)
        throw gcnew ArgumentNullException("target");

    return GetInheritedPropertyList(path, gcnew SvnInheritedPropertyListArgs(), list);
}

bool SvnClient::GetInheritedPropertyList(String^ path, SvnInheritedPropertyListArgs^ args, [Out] Collection<SvnInheritedPropertyListEventArgs^>^% list)
{
    if (!path)
        throw gcnew ArgumentNullException("target");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    InfoItemCollection<SvnInheritedPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnInheritedPropertyListEventArgs^>();

    try
    {
        return InheritedPropertyList(path, args, results->Handler);
    }
    finally
    {
        list = results;
    }
}

bool SvnClient::TryGetInheritedProperty(String^ path, String^ propertyName, [Out] String^% value)
{
    throw gcnew NotImplementedException();
}
