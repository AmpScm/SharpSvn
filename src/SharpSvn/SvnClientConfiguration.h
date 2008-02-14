// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	ref class SvnClient;

	public ref class SvnClientConfiguration sealed
	{
		SvnClient^ _client;
	internal:
		SvnClientConfiguration(SvnClient^ client)
		{
			if (!client)
				throw gcnew ArgumentNullException("client");

			_client = client;
		}

	public:
		/// <summary>Gets or sets a boolean indicating whether commits will fail if no log message is provided</summary>
		/// <remarks>The default value of this property is true</remarks>
		[System::ComponentModel::DefaultValue((System::Boolean)true)]
		property bool LogMessageRequired
		{
			bool get();
			void set(bool value);
		}
	};

}