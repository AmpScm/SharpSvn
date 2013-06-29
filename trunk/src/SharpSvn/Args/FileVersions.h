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

	public enum class SvnKeywordExpansion
	{
		Default=0,
		SameValues,
		None
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::FileVersions(SvnTarget^, SvnFileVersionsArgs^, EventHandler{SvnFileVersionEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnFileVersionsArgs : public SvnClientArgs
	{
		SvnRevision ^_start;
		SvnRevision ^_end;
		String^ _toDirectory;
		bool _includeMergedRevisions;
        bool _retrieveProperties;
		bool _noRetrieveContents;

		SvnLineStyle _lineStyle;
		SvnKeywordExpansion _keywordExpansion;
		bool _repairEol;

	internal:
		// Only valid while processing
		AprPool ^_keepPool;
		AprPool ^_curPool;
		AprPool ^_prevPool;
		AprPool ^_curFilePool;
		AprPool ^_prevFilePool;
		const char* _lastFile;
		const char* _reposRoot;
        apr_hash_t* _properties;
		apr_hash_t* _curKwProps;
		SvnFileVersionEventArgs^ _fv;

	public:
		SvnFileVersionsArgs()
		{
			_start = SvnRevision::None;
			_end = SvnRevision::None;
		}

	public:
		DECLARE_EVENT(SvnFileVersionEventArgs^, FileVersion);

	protected public:
		virtual void OnFileVersion(SvnFileVersionEventArgs^ e)
		{
			FileVersion(this, e);
		}

	public:
		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::FileVersions;
			}
		}

	public:
		/// <summary>Gets or sets the blame range as <see cref="SvnRevisionRange" /></summary>
		property SvnRevisionRange^ Range
		{
			SvnRevisionRange^ get()
			{
				return gcnew SvnRevisionRange(Start, End);
			}

			void set(SvnRevisionRange^ value)
			{
				if (!value)
				{
					Start = nullptr;
					End = nullptr;
				}
				else
				{
					Start = value->StartRevision;
					End = value->EndRevision;
				}
			}
		}

		property SvnRevision^ Start
		{
			SvnRevision^ get()
			{
				return _start;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_start = value;
				else
					_start = SvnRevision::None;
			}
		}

		property SvnRevision^ End
		{
			SvnRevision^ get()
			{
				return _end;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_end = value;
				else
					_end = SvnRevision::None;
			}
		}

		property bool RetrieveMergedRevisions
		{
			bool get()
			{
				return _includeMergedRevisions;
			}
			void set(bool value)
			{
				_includeMergedRevisions = value;
			}
		}

        property bool RetrieveProperties
        {
            bool get()
            {
                return _retrieveProperties;
            }
            void set(bool value)
            {
                _retrieveProperties = value;
            }
        }

		property bool RetrieveContents
        {
            bool get()
            {
                return !_noRetrieveContents;
            }
            void set(bool value)
            {
                _noRetrieveContents = !value;
            }
        }

		/// <summary>Gets or sets a directory to receive all file versions</summary>
		/// <value>The directory to place the versions in or <c>null</c> to only delegate the files to the event handler</value>
		property String^ ToDirectory
		{
			String^ get()
			{
				return _toDirectory;
			}
			void set(String^ value)
			{
				_toDirectory = value;
			}
		}

	public:
		property SvnKeywordExpansion KeywordExpansion
		{
			SvnKeywordExpansion get()
			{
				return _keywordExpansion;
			}
			void set(SvnKeywordExpansion value)
			{
				_keywordExpansion = EnumVerifier::Verify(value);
			}
		}

		/// <summary>Gets or sets a value indicating how to translate line endings (<see cref="SharpSvn::SvnLineStyle::Default" /> = no translation</summary>
		property SvnLineStyle LineStyle
		{
			SvnLineStyle get()
			{
				return _lineStyle;
			}
			void set(SvnLineStyle value)
			{
				_lineStyle = EnumVerifier::Verify(value);
			}
		}

		property bool RepairLineEndings
		{
			bool get()
			{
				return _repairEol;
			}
			void set(bool value)
			{
				_repairEol = value;
			}
		}
	};

	public ref class SvnFileVersionWriteArgs : public SvnClientArgs
	{
		bool _retrieveUntranslated;
	public:
		property bool WriteInRepositoryFormat
		{
			bool get()
			{
				return _retrieveUntranslated;
			}
			void set(bool value)
			{
				_retrieveUntranslated = value;
			}
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
