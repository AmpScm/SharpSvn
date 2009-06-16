
#pragma once

namespace SharpSvn {

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