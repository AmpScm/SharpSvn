#pragma once

namespace SharpGit {

    public ref class GitEventArgs abstract : public EventArgs
    {

    protected:
        virtual void Detach(bool keepValues)
        {
            UNUSED(keepValues);
        }
    public:
        void Detach()
        {
            Detach(true);
        }
    };

    public ref class GitClientEventArgs abstract : public GitEventArgs
    {
    };
}

#include "GitStatusEventArgs.h""