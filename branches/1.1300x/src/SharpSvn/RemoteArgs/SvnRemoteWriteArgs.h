#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
    namespace Remote {

        public ref class SvnRemoteWriteArgs : public SvnRemoteSessionArgs
        {
            __int64 _revision;

        public:
            SvnRemoteWriteArgs()
            {
                _revision = -1L;
            }

            property __int64 Revision
            {
                __int64 get()
                {
                    return _revision;
                }
                void set(__int64 value)
                {
                    _revision = (value >= 0) ? value : -1;
                }
            }
        };
    }
}
