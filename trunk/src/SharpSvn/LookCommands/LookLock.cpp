#include "stdafx.h"

#include "LookArgs/SvnLookLockArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;


bool SvnLookClient::GetLock(SvnLookOrigin^ lookOrigin, String^ path, [Out] SvnLockInfo^% lockInfo)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!path)
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path) && path->Length)
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    return GetLock(lookOrigin, path, gcnew SvnLookLockArgs(), lockInfo);
}

bool SvnLookClient::GetLock(SvnLookOrigin^ lookOrigin, String^ path, SvnLookLockArgs^ args, [Out] SvnLockInfo^% lockInfo)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!path)
        throw gcnew ArgumentNullException("path");
    else if (!IsNotUri(path) && path->Length)
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    svn_fs_root_t *root = nullptr;
    svn_fs_t* fs = nullptr;
    svn_error_t* r = open_origin(lookOrigin, &root, &fs, nullptr, %pool);

    if (r)
        return args->HandleResult(this, r);

    svn_lock_t *lock;
    const char *pPath = pool.AllocRelpath(path);
    r = svn_fs_get_lock(&lock, fs, pPath, pool.Handle);

    if (r)
        return args->HandleResult(this, r);

    if (!lock && !args->IgnorePathErrors)
    {
        svn_node_kind_t kind;
        r = svn_fs_check_path(&kind, root, pPath, pool.Handle);

        if (r)
            return args->HandleResult(this, r);

        if (kind != svn_node_file)
        {
            if (kind == svn_node_dir || kind == svn_node_symlink)
            {
                return args->HandleResult(this,
                          svn_error_createf(SVN_ERR_FS_NOT_FILE, NULL,
                                            "'%s' is not a file", pPath));
            }
            else
            {
                return args->HandleResult(this,
                          svn_error_createf(SVN_ERR_FS_NOT_FOUND, NULL,
                                            "'%s' is not a file", pPath));
            }
        }
    }

    if (lock)
        lockInfo = gcnew SvnLockInfo(lock, false);
    else
        lockInfo = nullptr;

    return args->HandleResult(this, r);
}