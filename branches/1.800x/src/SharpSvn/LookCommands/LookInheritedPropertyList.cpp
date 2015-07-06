#include "stdafx.h"
#include "LookArgs/SvnLookInheritedPropertyListArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::InheritedPropertyList(SvnLookOrigin^ lookOrigin, String^ path, EventHandler<SvnLookInheritedPropertyListEventArgs^> ^propertyHandler)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!path)
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path) && path->Length)
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
    else if (!propertyHandler)
        throw gcnew ArgumentNullException("propertyHandler");

    return InheritedPropertyList(lookOrigin, path, gcnew SvnLookInheritedPropertyListArgs(), propertyHandler);
}

bool SvnLookClient::InheritedPropertyList(SvnLookOrigin^ lookOrigin, String^ path, SvnLookInheritedPropertyListArgs ^args, EventHandler<SvnLookInheritedPropertyListEventArgs^> ^propertyHandler)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!path)
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path) && path->Length)
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    // We allow a null listHandler; the args object might just handle it itself

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    if (propertyHandler)
        args->InheritedPropertyList += propertyHandler;
    try
    {
        svn_fs_root_t* root = nullptr;
        svn_error_t* r;
        apr_hash_t *props;
        apr_array_header_t *inherited_props;
        const char *c_path = apr_pstrcat(pool.Handle, "/", pool.AllocRelpath(path), (void*)nullptr);

        if (r = open_origin(lookOrigin, &root, nullptr, nullptr, %pool))
            return args->HandleResult(this, r);

        /* This function accepts both relpaths and fspaths */
        if (r = svn_fs_node_proplist(&props, root, c_path, pool.Handle))
            return args->HandleResult(this, r);

        /* This function only accept fspaths */
        if (r = svn_repos_fs_get_inherited_props(&inherited_props, root, c_path,
                                                 NULL /* propname */,
                                                 NULL, NULL /* authz */,
                                                 pool.Handle, pool.Handle))
            return args->HandleResult(this, r);

        AprPool iterpool(%pool);
        SvnLookInheritedPropertyListEventArgs ^ea;

        /* TODO: Walk entries */
        for (int i = 0; i < inherited_props->nelts; i++)
        {
            const svn_prop_inherited_item_t *item = APR_ARRAY_IDX(inherited_props, i, svn_prop_inherited_item_t *);
            SvnLookInheritedPropertyListEventArgs ^ea;

            iterpool.Clear();
            ea = gcnew SvnLookInheritedPropertyListEventArgs(item->path_or_url, item->prop_hash, %iterpool);
            try
            {
                args->OnInheritedPropertyList(ea);
            }
            finally
            {
                ea->Detach(false);
            }

        }

        ea = gcnew SvnLookInheritedPropertyListEventArgs(c_path+1, props, %iterpool);
        try
        {
            args->OnInheritedPropertyList(ea);
        }
        finally
        {
            ea->Detach(false);
        }

        return args->HandleResult(this, r);
    }
    finally
    {
        if (propertyHandler)
            args->InheritedPropertyList -= propertyHandler;
    }
}

bool SvnLookClient::GetInheritedPropertyList(SvnLookOrigin^ lookOrigin, String^ path, [Out] Collection<SvnLookInheritedPropertyListEventArgs^> ^list)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!path)
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path) && path->Length)
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    InfoItemCollection<SvnLookInheritedPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnLookInheritedPropertyListEventArgs^>();

    try
    {
        return InheritedPropertyList(lookOrigin, path, gcnew SvnLookInheritedPropertyListArgs(), results->Handler);
    }
    finally
    {
        list = results;
    }
}

bool SvnLookClient::GetInheritedPropertyList(SvnLookOrigin^ lookOrigin, String^ path, SvnLookInheritedPropertyListArgs ^args, [Out] Collection<SvnLookInheritedPropertyListEventArgs^> ^list)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!path)
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path) && path->Length)
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    InfoItemCollection<SvnLookInheritedPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnLookInheritedPropertyListEventArgs^>();

    try
    {
        return InheritedPropertyList(lookOrigin, path, args, results->Handler);
    }
    finally
    {
        list = results;
    }
}
