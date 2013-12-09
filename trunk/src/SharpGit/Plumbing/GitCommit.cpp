#include "stdafx.h"

#include "GitCommit.h"
#include "GitRepository.h"
#include "GitTree.h"
#include "../GitClient/GitCommitCmd.h"

using namespace SharpGit::Plumbing;
using namespace SharpGit;

struct git_commit {};

GitSignature^ GitCommit::Author::get()
{
    if (!_author && !IsDisposed)
    {
        const git_signature *s = git_commit_author(Handle);
        _author = gcnew GitSignature(s);
    }
    return _author;
}

GitSignature^ GitCommit::Committer::get()
{
    if (!_committer && !IsDisposed)
    {
        const git_signature *s = git_commit_committer(Handle);
        _committer = gcnew GitSignature(s);
    }
    return _committer;
}

String^ GitCommit::LogMessage::get()
{
    if (!_logMessage && !IsDisposed)
    {
        const char *encoding = git_commit_message_encoding(Handle);
        const char *msg = git_commit_message(Handle);

        if (!msg || !msg[0])
            return _logMessage = "";

        if (!encoding || !_stricmp(encoding, "UTF-8"))
        {
            // The simple case
            return _logMessage = Utf8_PtrToString(msg);
        }

        try
        {
            System::Text::Encoding ^enc = System::Text::Encoding::GetEncoding(gcnew String(encoding));

            _logMessage = gcnew String(msg, 0, strlen(msg), enc);
        }
        catch (ArgumentException^)
        {
            const char *c = msg;

            while (*c > 0 && *c < 128)
                c++;

            if (*c == 0)
                return _logMessage = gcnew String(msg); // Plain ASCII

            throw;
        }
    }

    return _logMessage;
}

private ref class GitCommitAncestorWalker sealed : public IEnumerable<GitCommit^>, public IEnumerator<GitCommit^>
{
    initonly GitCommit^ _root;
    initonly bool _startSelf;
    GitCommit^ _current;
    bool _started;

public:
    GitCommitAncestorWalker(GitCommit^ root, bool startSelf)
    {
        if (! root)
            throw gcnew ArgumentNullException("root");

        _root = root;
        _startSelf = startSelf;
        _current = _root;
    }

private:
    ~GitCommitAncestorWalker()
    {
    }

private:
    virtual IEnumerator<GitCommit^>^ GetCommitEnumerator() sealed = IEnumerable<GitCommit^>::GetEnumerator
    {
        return gcnew GitCommitAncestorWalker(_root, _startSelf);
    }

    virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
    {
        return GetCommitEnumerator();
    }

    property Object^ ObjectCurrent
    {
        virtual Object^ get() sealed = System::Collections::IEnumerator::Current::get
        {
            return Current;
        }
    }

public:
    property GitCommit^ Current
    {
        virtual GitCommit^ get()
        {
            if (!_started)
                return nullptr;

            return _current;
        }
    }

    virtual bool MoveNext()
    {
        if (!_started)
        {
            if (_startSelf)
                _current = _root;
            else
                _current = _root->Ancestor;

            _started = true;
        }
        else if ((Object^)_current)
        {
            _current = _current->Ancestor;
        }

        return nullptr != (Object^)_current;
    }

    virtual void Reset()
    {
        _started = false;
        _current = nullptr;
    }
};

IEnumerable<GitCommit^>^ GitCommit::Ancestors::get()
{
    return gcnew GitCommitAncestorWalker(this, false);
}

IEnumerable<GitCommit^>^ GitCommit::AncestorsAndSelf::get()
{
    return gcnew GitCommitAncestorWalker(this, true);
}

GitTree^ GitCommit::Tree::get()
{
    if (!_tree && !IsDisposed)
    {
        git_tree *tree;

        int r = git_commit_tree(&tree, Handle);

        if (r == 0)
            _tree = gcnew GitTree(Repository, tree);
    }

    return _tree;
}
