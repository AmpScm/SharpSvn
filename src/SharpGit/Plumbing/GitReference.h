#pragma once
#include "GitClientContext.h"

namespace SharpGit {
    namespace Plumbing {


        public ref class GitReference : public Implementation::GitBase
        {
        private:
            initonly GitRepository ^_repository;
            git_reference *_reference;
            String^ _name;

            !GitReference()
            {
                if (_reference)
                {
                    try
                    {
                        git_reference_free(_reference);
                    }
                    finally
                    {
                        _reference = nullptr;
                        _name = nullptr;
                    }
                }
            }

            ~GitReference()
            {
                try
                    {
                        git_reference_free(_reference);
                    }
                    finally
                    {
                        _reference = nullptr;
                        _name = nullptr;
                    }
            }

        internal:
            GitReference(GitRepository ^repository, git_reference *handle)
            {
                if (! repository)
                    throw gcnew ArgumentNullException("repository");
                if (! handle)
                    throw gcnew ArgumentNullException("handle");

                _repository = repository;
                _reference = handle;
            }

            GitReference(GitRepository ^repository, String^ name)
            {
                if (! repository)
                    throw gcnew ArgumentNullException("repository");
                else if (String::IsNullOrEmpty(name))
                    throw gcnew ArgumentNullException("name");

                _repository = repository;
                _name = name;
            }

            property git_reference* Handle
            {
                git_reference* get();
            }

        private:


        public:
            property bool IsDisposed
            {
                bool get();
            }

            property String^ Name
            {
                String^ get();
            }

            bool Delete();
            bool Delete(GitArgs ^args);
        };

        public ref class GitReferenceCollection sealed : public Implementation::GitBase,
                                                                                   public System::Collections::Generic::IEnumerable<GitReference^>
        {
            initonly GitRepository ^_repository;
        internal:
            GitReferenceCollection(GitRepository ^repository)
            {
                if (!repository)
                    throw gcnew ArgumentNullException("repository");

                _repository = repository;
            }

        public:
            virtual System::Collections::Generic::IEnumerator<GitReference^>^ GetEnumerator();

        private:
            IEnumerable<GitReference^>^ GetEnumerable();

        private:
            virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
            {
                return GetEnumerator();
            }
        };
    }
}
