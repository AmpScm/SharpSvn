#include "stdafx.h"
#include "GitClient.h"
#include "GitDelete.h"

#include "Plumbing/GitRepository.h"
#include "Plumbing/GitIndex.h"

using namespace System;
using namespace SharpGit;
using namespace SharpGit::Plumbing;

bool GitClient::Delete(String ^path)
{
    return Delete(path, gcnew GitDeleteArgs());
}

bool GitClient::Delete(String ^path, GitDeleteArgs ^args)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    GitRepository repo;

    if (!repo.Locate(path, args))
        return false;

    AssertNotBare(repo);

    GitPool pool;

    // TODO: Only delete versioned and ignored nodes, like subversion does

    if (repo.Index->Remove(repo.MakeRelativePath(path), true)
        && repo.Index->Write(args))
    {
        if (!args->KeepLocal)
        {
            if (System::IO::File::Exists(path))
                System::IO::File::Delete(path);
            else if (System::IO::Directory::Exists(path))
                System::IO::Directory::Delete(path, true);
        }

        return true;
    }
    else
        return false;
}
