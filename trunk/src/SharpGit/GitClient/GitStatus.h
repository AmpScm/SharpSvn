#pragma once

namespace SharpGit {
    namespace Implementation {
        ref class GitPool;
    };

    public ref class GitStatusArgs : public GitClientArgs
    {
        bool _noWorkDir;
        bool _noIndex;
        bool _noUnversioned;
        bool _showIgnored;
        bool _showUnmodified;
        bool _noSubmodules;
        bool _recurseUnversioned;
        bool _recurseIgnored;

    public:
        event EventHandler<GitStatusEventArgs^>^ Status;

        property bool WorkingDirectoryStatus
        {
            bool get()
            {
                return !_noWorkDir;
            }
            void set(bool value)
            {
                _noWorkDir = !value;
            }
        }

        property bool IndexStatus
        {
            bool get()
            {
                return !_noIndex;
            }
            void set(bool value)
            {
                _noIndex = !value;
            }
        }

        property bool IncludeUnversioned
        {
            bool get()
            {
                return !_noUnversioned;
            }
            void set(bool value)
            {
                if (!value)
                {
                    _noUnversioned = true;
                    _recurseUnversioned = false;
                }
                else
                    _noUnversioned = false;
            }
        }

        property bool IncludeIgnored
        {
            bool get()
            {
                return _showIgnored;
            }
            void set(bool value)
            {
                _showIgnored = value;
            }
        }

        property bool IncludeUnmodified
        {
            bool get()
            {
                return _showUnmodified;
            }
            void set(bool value)
            {
                _showUnmodified = value;
            }
        }

        property bool IncludeSubmodules
        {
            bool get()
            {
                return !_noSubmodules;
            }
            void set(bool value)
            {
                _noSubmodules = !value;
            }
        }

        property bool IncludeUnversionedRecursive
        {
            bool get()
            {
                return _recurseUnversioned;
            }
            void set(bool value)
            {
                if (value)
                {
                    _recurseUnversioned = true;
                    _noUnversioned = false;
                }
                else
                    _recurseUnversioned = false;
            }
        }

        property bool IncludeIgnoredRecursive
        {
            bool get()
            {
                return _recurseIgnored;
            }
            void set(bool value)
            {
                if (value)
                {
                    _recurseIgnored = true;
                    _showIgnored = true;
                }
                else
                    _recurseIgnored = false;
            }
        }

    protected public:
        virtual void OnStatus(GitStatusEventArgs ^e)
        {
            Status(this, e);
        }

    internal:
        const git_status_options *MakeOptions(String^ path, Implementation::GitPool ^pool);
    };
}
