#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s Changed method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLookPropertyListArgs : public SvnClientArgs
	{
	public:
		SvnLookPropertyListArgs()
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
