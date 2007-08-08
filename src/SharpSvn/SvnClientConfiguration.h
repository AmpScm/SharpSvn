#pragma once

namespace SharpSvn {

	ref class SvnClient;

	public ref class SvnClientConfiguration sealed
	{
		SvnClient^ _client;
	internal:
		SvnClientConfiguration(SvnClient^ client)
		{
			if(!client)
				throw gcnew ArgumentNullException("client");

			_client = client;
		}

	public:
		/// <summary>Gets or sets a boolean indicating whether commits will fail if no log message is provided</summary>
		property bool LogMessageRequired
		{
			bool get();
			void set(bool value);
		}
	};

}