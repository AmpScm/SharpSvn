
#pragma once

namespace SharpSvn {
    namespace Remote {

    public ref class SvnRemoteSessionArgs abstract : public SvnClientArgs
    {
    protected:
        SvnRemoteSessionArgs()
        {
        }

    public:
        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Unknown;
            }
        }
    };
}
}
