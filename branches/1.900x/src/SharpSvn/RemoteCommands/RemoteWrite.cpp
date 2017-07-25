#include "stdafx.h"

#include "RemoteArgs/SvnRemoteWriteArgs.h"
#include "SvnStreamWrapper.h"
#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;


bool SvnRemoteSession::Write(String^ relPath, Stream^ output)
{
    Uri^ uri;
    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");
    else if (!output)
        throw gcnew ArgumentNullException("output");

    return Write(relPath, output, gcnew SvnRemoteWriteArgs());
}

bool SvnRemoteSession::Write(String ^relPath, Stream^ output, SvnRemoteWriteArgs^ args)
{
    Uri^ uri;
    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");
    else if (!output)
        throw gcnew ArgumentNullException("output");
    else if (!args)
        throw gcnew ObjectDisposedException("args");

    AprPool pool(%_pool);

    return InternalWrite(relPath, output, args, nullptr, %pool);
}

bool SvnRemoteSession::Write(String ^relPath, Stream^ output, [Out] SvnPropertyCollection ^%properties)
{
    Uri^ uri;
    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");
    else if (!output)
        throw gcnew ArgumentNullException("output");

    return Write(relPath, output, gcnew SvnRemoteWriteArgs(), properties);
}

bool SvnRemoteSession::Write(String ^relPath, Stream^ output, SvnRemoteWriteArgs^ args, [Out] SvnPropertyCollection ^%properties)
{
    Uri^ uri;
    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");
    else if (!output)
        throw gcnew ArgumentNullException("output");
    else if (!args)
        throw gcnew ObjectDisposedException("args");

    AprPool pool(%_pool);
    apr_hash_t *props = nullptr;

    properties = nullptr;

    if (InternalWrite(relPath, output, args, &props, %pool))
    {
        properties = CreatePropertyDictionary(props, %pool);
        return true;
    }

    return false;
}


bool SvnRemoteSession::InternalWrite(String ^relPath, Stream^ output, SvnRemoteWriteArgs^ args, apr_hash_t **props, AprPool ^resultPool)
{
    Uri^ uri;
    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");
    else if (!output)
        throw gcnew ArgumentNullException("output");
    else if (!args)
        throw gcnew ObjectDisposedException("args");

    Ensure();
    AprPool scratchPool(resultPool);
    ArgsStore store(this, args, %scratchPool);

    SvnStreamWrapper wrapper(output, false, true, %scratchPool);

    svn_error_t *r = svn_ra_get_file(_session,
                                     scratchPool.AllocRelpath(relPath),
                                     (svn_revnum_t)args->Revision,
                                     wrapper.Handle,
                                     NULL,
                                     props,
                                     resultPool->Handle);

    return args->HandleResult(this, r, relPath);
}

