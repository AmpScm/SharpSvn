// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
        [System::ComponentModel::DefaultValue((System::Boolean)true)]
		property bool EnableSharpPlink
		{
			bool get();
			void set(bool value);
		}

        /// <summary>Gets or sets a boolean indicating whether to always use the subversion integrated diff library
        /// instead of the user configured diff tools</summary>
        /// <remarks>The default value of this property is true; to allow parsing the output of the diff library</remarks>
        /// <value>true if subversions internal diff must be used, otherwise false</value>
        [System::ComponentModel::DefaultValue((System::Boolean)true)]
        property bool UseSubversionDiff
        {
            bool get();
            void set(bool value);
        }

		/// <summary>Gets or sets a boolean indicating whether to use automerging of changes on text files. Disabling
		/// this makes subversion handle all files as binary when handling content merging</summary>
		/// <remarks>The default value of this property is true; Set it to false to disable automerging</remarks>
		/// <value>true if subversion must do an attempt to automerge text files, otherwise false</value>
		[System::ComponentModel::DefaultValue((System::Boolean)true)]
		property bool UseSubversionAutomaticMerge
		{
			bool get();
			void set(bool value);
		}
	};

}