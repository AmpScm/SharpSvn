#pragma once
#include "GitClientContext.h"
#include "GitCommitCmd.h"

namespace SharpGit {

    public ref class GitTagArgs : public GitClientArgs
    {
        initonly GitSignature^ _signature;
        String ^_logMessage;
        bool _overwrite;
        bool _noNormalize;
        bool _stripComments;
        bool _lightWeight;

    public:
        GitTagArgs()
        {
            _signature = gcnew GitSignature();
        }

        property GitSignature ^Tagger
        {
            GitSignature^ get()
            {
                return _signature;
            }
        }

        property String ^ LogMessage
        {
            String ^get()
            {
                return _logMessage;
            }
            void set(String ^value)
            {
                _logMessage = value;
            }
        }

        property bool OverwriteExisting
        {
            bool get()
            {
                return _overwrite;
            }
            void set(bool value)
            {
                _overwrite = value;
            }
        }

        property bool NormalizeLogMessage
        {
            bool get()
            {
                return !_noNormalize;
            }
            void set(bool value)
            {
                _noNormalize = !value;
            }
        }

        property bool StripLogMessageComments
        {
            bool get()
            {
                return _stripComments;
            }
            void set(bool value)
            {
                _stripComments = value;
            }
        }

        property bool LightWeight
        {
            bool get()
            {
                return _lightWeight;
            }
            void set(bool value)
            {
                _lightWeight = value;
            }
        }
    };
}}
