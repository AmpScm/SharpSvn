#pragma once

#include "LookArgs/SvnLookClientArgs.h"

namespace SharpSvn {

    /// <summary>Base Extended Parameter container of <see cref="SvnLookClient" /> commands</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnLookLockArgs : public SvnLookClientArgs
    {
        bool _ignorePathErrors;
    public:
        SvnLookLockArgs()
        {
        }

        /// <summary>If true, doesn't perform a check if path does really exist in the repository when no lock is found</summary>
        property bool IgnorePathErrors
        {
            bool get()
            {
                return _ignorePathErrors;
            }
            void set(bool value)
            {
                _ignorePathErrors = value;
            }
        }

        virtual property SvnCommandType CommandType
        {
          virtual SvnCommandType get() override sealed
          {
            return SvnCommandType::Unknown;
          }
        }
    };
}
