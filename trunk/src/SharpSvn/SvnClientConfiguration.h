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

    public ref class SvnClientConfiguration sealed : public SvnBase
    {
        initonly SvnClient^ _client;
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

        /// <summary>This property used to get a boolean indicating whether to enable the SharpPlink handler when no
        /// other valid handler for svn+ssh:// was registered. It is now mapped to the inverse of <see cref-"EnableSharpPlink" /></summary>
        [System::ComponentModel::DefaultValue((System::Boolean)true)]
        [Obsolete("Use .DisableBuiltinSsh and/or .FallbackSshClient as the direct SharpPlink support in SharpSvn for Subversion <= 1.8.x has been replaced with LibSSH2")]
        property bool EnableSharpPlink
        {
            bool get()  { return !DisableBuiltinSsh; }
            void set(bool value) { DisableBuiltinSsh = !value; }
        }

        /// <summary>
        property bool DisableBuiltinSsh
        {
            bool get();
            void set(bool value);
        }

        property String^ FallbackSshClient
        {
            String ^ get();
            void set(String ^value);
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

        /// <summary>Gets or sets a value indicating whether the 'preserved-conflict-file-exts' should be forced to '*'</summary>
        [System::ComponentModel::DefaultValue(SvnOverride::Never)]
        property SvnOverride KeepAllExtensionsOnConflict
        {
            SvnOverride get();
            void set(SvnOverride value);
        }

        /// <summary>Gets the subversion global ignore pattern as specified in the configuration</summary>
        property System::Collections::Generic::IEnumerable<String^>^ GlobalIgnorePattern
        {
            System::Collections::Generic::IEnumerable<String^>^ get();
        }

        /// <summary>While the configuration isn't used yet, allows overriding specific configuration options</summary>
        void SetOption(String^ file, String^ section, String^ option, String^ value);
    };

}
