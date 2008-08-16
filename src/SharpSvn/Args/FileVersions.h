// $Id: Export.h 500 2008-03-26 17:47:22Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::FileVersions(SvnTarget^, SvnFileVersionsArgs^, EventHandler{SvnFileVersionEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnFileVersionsArgs : public SvnClientArgs
	{
		SvnRevision ^_start;
		SvnRevision ^_end;
		String^ _toDirectory;
		bool _includeMergedRevisions;

	internal:
		// Only valid while processing
		AprPool ^_curPool;
		AprPool ^_prevPool;
		AprPool ^_curFilePool;
		AprPool ^_prevFilePool;
		const char* _lastFile;
		const char* _tempPath;

	public:
		SvnFileVersionsArgs()
		{
			_start = SvnRevision::None;
			_end = SvnRevision::None;
		}

	public:
		event EventHandler<SvnFileVersionEventArgs^>^ FileVersion;

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
	};

}
