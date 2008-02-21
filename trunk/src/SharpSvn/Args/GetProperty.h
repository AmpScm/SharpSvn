// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetProperty</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetPropertyArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;
	public:
		SvnGetPropertyArgs()
		{
			_depth = SvnDepth::Empty;
			_revision = SvnRevision::None;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::GetProperty;
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

		/// <summary>Gets the list of changelist-names</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if (!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};

}
