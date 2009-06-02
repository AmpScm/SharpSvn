#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s Changed method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLookGetPropertyArgs : public SvnClientArgs
	{
	public:
		SvnLookGetPropertyArgs()
		{
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
