// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Switch(String^, SvnUriTarget^, SvnSwitchArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnSwitchArgs : public SvnClientArgsWithConflict
	{
		SvnDepth _depth;
		SvnRevision^ _revision;
		bool _allowObstructions;
		bool _ignoreExternals;
		bool _keepDepth;

	public:
		SvnSwitchArgs()
		{
			_depth = SvnDepth::Infinity;
			_revision = SvnRevision::None;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Switch;
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

		property bool KeepDepth
		{
			bool get()
			{
				return _keepDepth;
			}
			void set(bool value)
			{
				_keepDepth = value;
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

		property bool AllowObstructions
		{
			bool get()
			{
				return _allowObstructions;
			}
			void set(bool value)
			{
				_allowObstructions = value;
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
	};

}
