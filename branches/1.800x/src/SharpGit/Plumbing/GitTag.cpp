#include "stdafx.h"

#include "GitRepository.h"
#include "GitObject.h"
#include "GitBlob.h"
#include "GitCommit.h"
#include "GitReference.h"
#include "GitTag.h"
#include "GitTree.h"
#include "GitBranch.h"

#include "../GitClient/GitCommitCmd.h"
#include "../GitClient/GitTagCmd.h"

#include "UnmanagedStructs.h"

using namespace SharpGit;
using namespace SharpGit::Implementation;
using namespace SharpGit::Plumbing;
using System::Collections::Generic::List;

GitSignature^ GitTag::Tagger::get()
{
    if (!_tagger && !IsDisposed)
        _tagger = gcnew GitSignature(git_tag_tagger(Handle));

    return _tagger;
}

GitObject^ GitTag::Target::get()
{
    if (!_target && !IsDisposed)
    {
        git_object *ob;

        if (0 == git_tag_target(&ob, Handle))
            _target = GitObject::Create(Repository, ob);
    }
    return _target;
}

GitId^ GitTag::TargetId::get()
{
    if (!_targetId && !IsDisposed)
        _targetId = gcnew GitId(git_tag_target_id(Handle));

    return _targetId;
}

bool GitObject::Tag(String ^tagName, GitTagArgs ^args)
{
    GitId ^ignored;
    return Tag(tagName, args, ignored);
}

bool GitObject::Tag(String ^tagName, GitTagArgs ^args, [Out] GitId ^%id)
{
    if (String::IsNullOrEmpty(tagName))
        throw gcnew ArgumentNullException("tagName");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    GitPool pool;
    git_oid result;
    int r;
    if (!args->LightWeight)
    {
        bool normalize = args->NormalizeLogMessage;
        bool strip = args->StripLogMessageComments;
        const char *msg;
        if (normalize || strip)
        {
            String^ msgString = args->LogMessage ? args->LogMessage : "";
            msgString = msgString->Replace("\r", "");
            msg = pool.AllocString(msgString);
            size_t sz = strlen(msg);
            sz += sz/4 + 4;
            char *result = (char*)pool.Alloc(sz+1);

            int r = git_message_prettify(result, sz, msg, strip);

            if (r < 0)
            {
                int len = git_message_prettify(NULL, 0, msg, strip);

                if (len >= 0)
                {
                    result = (char*)pool.Alloc(sz+1);

                    r = git_message_prettify(result, sz, msg, strip);
                }
            }

            if (r < 0)
                return args->HandleGitError(this, r);

            msg = result;
        }
        else
            msg = args->LogMessage ? pool.AllocString(args->LogMessage) : "";

        r = git_tag_create(&result, Repository->Handle,
            pool.AllocString(tagName),
            Handle,
            args->Tagger->Alloc(Repository, %pool),
            msg,
            args->OverwriteExisting);
    }
    else
    {
        r = git_tag_create_lightweight(&result, Repository->Handle,
            pool.AllocString(tagName),
            Handle,
            args->OverwriteExisting);
    }

    if (r == 0)
        id = gcnew GitId(result);
    else
        id = nullptr;

    return args->HandleGitError(this, r);
}

GitBranchCollection^ GitRepository::Branches::get()
{
    if (! _branches)
        _branches = gcnew GitBranchCollection(this);

    return _branches;
}

GitReference^ GitBranch::Reference::get()
{
    if (!_reference && !_repository->IsDisposed)
        _reference = gcnew GitReference(_repository, Name);

    return _reference;
}

IEnumerator<GitBranch^>^ GitBranchCollection::GetEnumerator()
{
    IEnumerable<GitBranch^>^ walker = GetEnumerable((git_branch_t)(GIT_BRANCH_LOCAL | GIT_BRANCH_REMOTE));

    return walker->GetEnumerator();
}

IEnumerable<GitBranch^>^ GitBranchCollection::Local::get()
{
    return GetEnumerable(GIT_BRANCH_LOCAL);
}

IEnumerable<GitBranch^>^ GitBranchCollection::Remote::get()
{
    return GetEnumerable(GIT_BRANCH_REMOTE);
}

IEnumerable<GitBranch^>^ GitBranchCollection::GetEnumerable(git_branch_t flags)
{
    List<GitBranch^> ^branches = gcnew List<GitBranch^>();
    if (_repository->IsDisposed)
        return branches->AsReadOnly();

    git_branch_iterator *iter;
    int r = git_branch_iterator_new(&iter, _repository->Handle, flags);
    if (r != 0)
    {
    (gcnew GitNoArgs())->HandleGitError(this, r);
    return branches;
    }

    git_reference *ref;
    git_branch_t branch_type;
    while (! git_branch_next(&ref, &branch_type, iter))
    {
    const char *name;
    String^ nm;

    if (0 == git_branch_name(&name, ref))
        nm = GitBase::Utf8_PtrToString(name);
    else
        nm = "";

    GitReference ^gr = gcnew GitReference(_repository, ref);


    branches->Add(gcnew GitBranch(_repository, nm, gr, branch_type));
    }
    git_branch_iterator_free(iter);

    return branches->AsReadOnly();
}

bool GitBranchCollection::Create(GitCommit^ commit, String^ name)
{
    GitBranch ^ignored;
    return Create(commit, name, gcnew GitNoArgs(), ignored);
}

bool GitBranchCollection::Create(GitCommit^ commit, String^ name, GitArgs^ args)
{
    GitBranch ^ignored;
    return Create(commit, name, args, ignored);
}

bool GitBranchCollection::Create(GitCommit^ commit, String^ name, [Out] GitBranch^% branch)
{
    return Create(commit, name, gcnew GitNoArgs(), branch);
}

bool GitBranchCollection::Create(GitCommit^ commit, String^ name, GitArgs^ args, [Out] GitBranch^% branch)
{
    if (_repository->IsDisposed)
        throw gcnew ObjectDisposedException("repository");
    else if (!commit)
        throw gcnew ArgumentNullException("commit");
    else if (String::IsNullOrEmpty(name))
        throw gcnew ArgumentNullException("name");
    else if(commit->IsDisposed || commit->Repository != _repository)
        throw gcnew ArgumentOutOfRangeException("commit");

    GitPool pool(_repository->Pool);
    git_reference *result;
    int r = git_branch_create(&result, _repository->Handle, pool.AllocString(name), commit->Handle, FALSE /* force */);

    if (!r)
        branch = gcnew GitBranch(_repository, name, gcnew GitReference(_repository, result));
    else
        branch = nullptr;

    return args->HandleGitError(this, r);
}

/*bool GitBranchCollection::Contains(GitReference ^reference)
{
if (_repository->IsDisposed)
return false;

int r = git_branch_lookup(
}*/
