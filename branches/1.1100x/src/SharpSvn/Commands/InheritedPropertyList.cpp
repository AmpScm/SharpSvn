#include "stdafx.h"

#include "Args/InheritedPropertyList.h"

using namespace SharpSvn;

bool SvnClient::InheritedPropertyList(SvnTarget ^target,EventHandler<SvnInheritedPropertyListEventArgs^>^ listHandler)
{
    if (!target)
        throw gcnew ArgumentNullException("target");

    return InheritedPropertyList(target, gcnew SvnInheritedPropertyListArgs(), listHandler);
}

static svn_error_t *iprop_list(void *baton, const char *path, apr_hash_t *prop_hash, apr_array_header_t *inherited_props, apr_pool_t *scratch_pool)
{
    SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

    AprPool thePool(scratch_pool, false);

    SvnInheritedPropertyListArgs^ args = dynamic_cast<SvnInheritedPropertyListArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
    if (!args)
        return nullptr;

    SvnInheritedPropertyListEventArgs ^e_anchor = gcnew SvnInheritedPropertyListEventArgs(path, prop_hash, nullptr, client->CtxHandle, %thePool);
    try
    {

        if (prop_hash)
        {
            args->OnList(e_anchor);

            if (e_anchor->Cancel)
                return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
        }

        if (inherited_props)
        {
            // Properties on one or more ancestors
            for (int i = inherited_props->nelts-1; i >= 0; i--)
            {
                const svn_prop_inherited_item_t *pii = APR_ARRAY_IDX(inherited_props, i, const svn_prop_inherited_item_t *);

                SvnInheritedPropertyListEventArgs ^e = gcnew SvnInheritedPropertyListEventArgs(pii->path_or_url, pii->prop_hash, e_anchor, nullptr, %thePool);
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
    finally
    {
        e_anchor->Detach(false);
    }
}

bool SvnClient::InheritedPropertyList(SvnTarget ^target,SvnInheritedPropertyListArgs^ args, EventHandler<SvnInheritedPropertyListEventArgs^>^ listHandler)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    if (listHandler)
        args->List += listHandler;
    try
    {
        svn_opt_revision_t pegrev = target->Revision->ToSvnRevision();
        svn_opt_revision_t rev = args->Revision->Or(target->Revision)->ToSvnRevision();

        svn_error_t* r = svn_client_proplist4(
            target->AllocAsString(%pool, true),
            &pegrev,
            &rev,
            svn_depth_empty,
            nullptr /* changelist */,
            TRUE /* get_inherited_props */,
            iprop_list,
            (void*)_clientBaton->Handle,
            CtxHandle,
            pool.Handle);

        return args->HandleResult(this, r, target);
    }
    finally
    {
        if (listHandler)
            args->List -= listHandler;
    }
}

bool SvnClient::GetInheritedPropertyList(SvnTarget ^target,[Out] Collection<SvnInheritedPropertyListEventArgs^>^% list)
{
    if (!target)
        throw gcnew ArgumentNullException("target");

    return GetInheritedPropertyList(target, gcnew SvnInheritedPropertyListArgs(), list);
}

bool SvnClient::GetInheritedPropertyList(SvnTarget ^target,SvnInheritedPropertyListArgs^ args, [Out] Collection<SvnInheritedPropertyListEventArgs^>^% list)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    InfoItemCollection<SvnInheritedPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnInheritedPropertyListEventArgs^>();

    try
    {
        return InheritedPropertyList(target, args, results->Handler);
    }
    finally
    {
        list = results;
    }
}

private ref class TheIPropCollector
{
    initonly Dictionary<String^, SvnPropertyValue^> ^_d;

public:
    TheIPropCollector()
    {
        _d = gcnew Dictionary<String^, SvnPropertyValue^>();
    }

public:
    void Collect(Object ^sender, SvnInheritedPropertyListEventArgs^ e)
    {
        UNUSED_ALWAYS(sender);

        for each(SvnPropertyValue ^v in e->Properties)
        {
            if (! _d->ContainsKey(v->Key))
                _d[v->Key] = v;
        }
    }

    SvnPropertyCollection^ GetCollection()
    {
        SvnPropertyCollection^ pc = gcnew SvnPropertyCollection();
        for each(SvnPropertyValue ^v in _d->Values)
            pc->Add(v);

        return pc;
    }
};

// This can be optimized by using more unmanaged code, but we can always do that later
bool SvnClient::TryGetAllInheritedProperties(SvnTarget ^target,[Out] SvnPropertyCollection^% properties)
{
    TheIPropCollector ^pc = gcnew TheIPropCollector();

    SvnInheritedPropertyListArgs^ la = gcnew SvnInheritedPropertyListArgs();
    la->ThrowOnError = false;
    properties = nullptr;

    if (InheritedPropertyList(target, la, gcnew EventHandler<SvnInheritedPropertyListEventArgs^>(pc, &TheIPropCollector::Collect)))
    {
        properties = pc->GetCollection();
        return true;
    }

    return false;
}

// This can be optimized by using more unmanaged code, but we can always do that later
bool SvnClient::TryGetInheritedProperty(SvnTarget ^target, String^ propertyName, [Out] String^% value)
{
    SvnPropertyCollection^ props;
    value = nullptr;

    if (TryGetAllInheritedProperties(target, props))
    {
        if (props->Contains(propertyName))
        {
            SvnPropertyValue ^pv = props[propertyName];

            value = pv->StringValue;

            if (value)
                return true;
        }
    }

    return false;
}

bool SvnClient::InheritedPropertyList(String^ path, EventHandler<SvnInheritedPropertyListEventArgs^>^ listHandler)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return InheritedPropertyList(gcnew SvnPathTarget(path), listHandler);
}

bool SvnClient::InheritedPropertyList(String^ path, SvnInheritedPropertyListArgs^ args, EventHandler<SvnInheritedPropertyListEventArgs^>^ listHandler)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return InheritedPropertyList(gcnew SvnPathTarget(path), args, listHandler);
}

bool SvnClient::GetInheritedPropertyList(String^ path, [Out] Collection<SvnInheritedPropertyListEventArgs^>^% list)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return GetInheritedPropertyList(gcnew SvnPathTarget(path), list);
}

bool SvnClient::GetInheritedPropertyList(String^ path, SvnInheritedPropertyListArgs^ args, [Out] Collection<SvnInheritedPropertyListEventArgs^>^% list)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return GetInheritedPropertyList(gcnew SvnPathTarget(path), args, list);
}

bool SvnClient::TryGetAllInheritedProperties(String^ path, [Out] SvnPropertyCollection^% properties)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return TryGetAllInheritedProperties(gcnew SvnPathTarget(path), properties);
}

bool SvnClient::TryGetInheritedProperty(String^ path, String^ propertyName, [Out] String^% value)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return TryGetInheritedProperty(gcnew SvnPathTarget(path), propertyName, value);
}
