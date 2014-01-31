#include "stdafx.h"
#include "GitClient.h"
#include "GitCheckOut.h"

#include "Plumbing/GitRepository.h"
#include "Plumbing/GitIndex.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

const git_checkout_opts * GitCheckOutArgs::MakeCheckOutOptions(GitPool ^pool)
{
    git_checkout_opts *opts = (git_checkout_opts *)pool->Alloc(sizeof(*opts));

    opts->version = GIT_CHECKOUT_OPTS_VERSION;

    if (! DryRun)
    {
        opts->checkout_strategy |= GIT_CHECKOUT_SAFE;

        if (Create)
            opts->checkout_strategy |= GIT_CHECKOUT_SAFE_CREATE;

        if (OverwriteExisting)
            opts->checkout_strategy |= GIT_CHECKOUT_FORCE;

        if (! FailOnConflicts)
            opts->checkout_strategy |= GIT_CHECKOUT_ALLOW_CONFLICTS;

        if (RemoveUnversioned)
            opts->checkout_strategy |= GIT_CHECKOUT_REMOVE_UNTRACKED;

        if (RemoveIgnored)
            opts->checkout_strategy |= GIT_CHECKOUT_REMOVE_IGNORED;

        if (UpdateOnly)
            opts->checkout_strategy |= GIT_CHECKOUT_UPDATE_ONLY;

        if (SkipIndexUpdate)
            opts->checkout_strategy |= GIT_CHECKOUT_DONT_UPDATE_INDEX;

        if (SkipInitialRefresh)
            opts->checkout_strategy |= GIT_CHECKOUT_NO_REFRESH;

        // GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH

        // Defined but unimplemented:
        // GIT_CHECKOUT_SKIP_UNMERGED
        // GIT_CHECKOUT_USE_OURS
        // GIT_CHECKOUT_USE_THEIRS
        // GIT_CHECKOUT_UPDATE_SUBMODULES
        // GIT_CHECKOUT_UPDATE_SUBMODULES_IF_CHANGED
    }

    return opts;
}
