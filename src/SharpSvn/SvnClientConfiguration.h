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

		/// <summary>Gets or sets a boolean indicating whether to load the svn mimetypes file when calling Add or Import the first time</summary>
		/// <remarks>The default value of this property is true; this matches the behaviour of the svn commandline client</remarks>
		/// <value>true if loading the mimetypes file on the initial import, otherwise false</value>
		[System::ComponentModel::DefaultValue((System::Boolean)true)]
		property bool LoadSvnMimeTypes
		{
			bool get();
			void set(bool value);
		}

		/// <summary>Gets or sets a boolean indicating whether to enable the SharpPlink handler when no other valid
		/// handler for svn+ssh:// is registered</summary>
		/// <remarks>The default value of this property is true; this matches the behaviour of TortoiseSvn</remarks>
		/// <value>true if enabling SharpPlink on the first repository request, otherwise false</value>
		property bool EnableSharpPlink
		{
			bool get();
			void set(bool value);
		}
	};

}