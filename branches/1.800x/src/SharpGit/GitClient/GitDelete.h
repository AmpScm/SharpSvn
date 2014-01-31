#pragma once

namespace SharpGit {

    public ref class GitDeleteArgs : public GitClientArgs
    {
        bool _keepLocal;

    public:
        property bool KeepLocal
        {
            bool get()
            {
                return _keepLocal;
            }
            void set(bool value)
            {
                _keepLocal = value;
            }
        }
    };
}
