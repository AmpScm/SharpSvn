#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::CreateDirectory(String^, SvnCreateDirectoryArgs^)" /> and <see cref="SvnClient::RemoteCreateDirectory(Uri^, SvnCreateDirectoryArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCropWorkingCopyArgs : public SvnClientArgs
	{
		SvnDepth _depth; // Default value = Empty

	public:
		SvnCropWorkingCopyArgs()
		{}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::CropWorkingCopy;
			}
		}

	public:
	};
}

