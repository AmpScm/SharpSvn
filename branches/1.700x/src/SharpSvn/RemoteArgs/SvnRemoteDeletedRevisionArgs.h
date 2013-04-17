#pragma once

#include "SvnRemoteSessionArgs.h"


namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemoteDeletedRevisionArgs : public SvnRemoteSessionArgs
	{
        SvnRevision^ _endRevision;
    public:
        SvnRemoteDeletedRevisionArgs()
        {
            _endRevision = SvnRevision::Head;
        }

        property SvnRevision^ EndRevision
        {
            SvnRevision^ get()
            {
                return _endRevision;
            }

            void set(SvnRevision^ value)
            {
                if (value)
                    _endRevision = value;
                else
                    _endRevision = SvnRevision::Head;
            }
        }

	};
}
}