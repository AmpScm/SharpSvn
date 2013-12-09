#pragma once
#include "GitClientContext.h"

namespace SharpGit {
    namespace Plumbing {
        ref class GitReference;
        public ref class GitBranch : public Implementation::GitBase
        {
            initonly GitRepository ^_repository;
            initonly String ^_name;
            initonly int _type;
            GitReference^ _reference;

        internal:
            GitBranch(GitRepository^ repository, String ^name, git_branch_t type)
            {
                if (! repository)
                    throw gcnew ArgumentNullException("repository");
                else if (String::IsNullOrEmpty(name))
                    throw gcnew ArgumentNullException("name");

                _repository = repository;
                _name = name;
                _type = type;
            }

            GitBranch(GitRepository^ repository, String ^name, GitReference^ reference)
            {
                if (! repository)
                    throw gcnew ArgumentNullException("repository");
                else if (String::IsNullOrEmpty(name))
                    throw gcnew ArgumentNullException("name");
                else if (! reference)
                    throw gcnew ArgumentNullException("reference");

                _repository = repository;
                _name = name;
                _reference = reference;
                _type = GIT_BRANCH_LOCAL;
            }

            GitBranch(GitRepository^ repository, String ^name, GitReference^ reference, git_branch_t type)
            {
                if (! repository)
                    throw gcnew ArgumentNullException("repository");
                else if (String::IsNullOrEmpty(name))
                    throw gcnew ArgumentNullException("name");
                else if (! reference)
                    throw gcnew ArgumentNullException("reference");

                _repository = repository;
                _name = name;
                _reference = reference;
                _type = type;
            }

        public:
            property String^ Name
            {
                String^ get()
                {
                    return _name;
                }
            }

            property GitReference^ Reference
            {
                GitReference^ get();
            }

            virtual String^ ToString() override
            {
                return Name;
            }

            property bool IsLocal
            {
                bool get()
                {
                    return 0 != (_type & GIT_BRANCH_LOCAL);
                }
            }

            property bool IsRemote
            {
                bool get()
                {
                    return 0 != (_type & GIT_BRANCH_REMOTE);
                }
            }
        };


        public ref class GitBranchCollection sealed : public Implementation::GitBase,
                                                                                   public System::Collections::Generic::IEnumerable<GitBranch^>
        {
            initonly GitRepository ^_repository;
        internal:
            GitBranchCollection(GitRepository ^repository)
            {
                if (!repository)
                    throw gcnew ArgumentNullException("repository");

                _repository = repository;
            }

        public:
            virtual System::Collections::Generic::IEnumerator<GitBranch^>^ GetEnumerator();

        public:
            property IEnumerable<GitBranch^>^ Local
            {
                IEnumerable<GitBranch^>^ get();
            }

            property IEnumerable<GitBranch^>^ Remote
            {
                IEnumerable<GitBranch^>^ get();
            }

        public:
            bool Create(GitCommit^ commit, String^ name);
            bool Create(GitCommit^ commit, String^ name, GitArgs^ args);
            bool Create(GitCommit^ commit, String^ name, [Out] GitBranch^% branch);
            bool Create(GitCommit^ commit, String^ name, GitArgs^ args, [Out] GitBranch^% branch);

        private:
            IEnumerable<GitBranch^>^ GetEnumerable(git_branch_t flags);

        public:
            //bool Contains(GitReference ^reference);

        private:
            virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
            {
                return GetEnumerator();
            }
        };
    }
}
