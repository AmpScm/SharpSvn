// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {
	ref class SvnRevision;

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDumpRepositoryArgs : public SvnClientArgs
	{
		SvnRevision^ _start;
		SvnRevision^ _end;
		bool _deltas;
		bool _incremental;
	public:
		SvnDumpRepositoryArgs()
		{
			_start = SvnRevision::None;
			_end = SvnRevision::None;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
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
					_start = SvnRevision::Zero;
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
					_end = SvnRevision::Head;
			}
		}

		property bool Deltas
		{
			bool get()
			{
				return _deltas;
			}
			void set(bool value)
			{
				_deltas = value;
			}
		}

		property bool Incremental
		{
			bool get()
			{
				return _incremental;
			}
			void set(bool value)
			{
				_incremental = value;
			}
		}
	};
}