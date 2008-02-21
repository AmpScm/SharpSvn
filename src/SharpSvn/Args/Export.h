// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Export(SvnTarget^, String^, SvnExportArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnExportArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreExternals;
		bool _overwrite;
		SvnRevision^ _revision;
		SvnLineStyle _lineStyle;
	public:
		SvnExportArgs()
		{
			_depth = SvnDepth::Infinity;
			_revision = SvnRevision::None;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Export;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}

		property bool Overwrite
		{
			bool get()
			{
				return _overwrite;
			}
			void set(bool value)
			{
				_overwrite = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

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
	};

}
