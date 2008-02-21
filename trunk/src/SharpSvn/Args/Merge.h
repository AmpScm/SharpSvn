// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Merge(String^,SvnTarget^, SvnRevisionRange^, SvnMergeArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnMergeArgs : public SvnMergeBaseArgs
	{
		SvnDepth _depth;
		bool _ignoreAncestry;
		bool _force;
		bool _recordOnly;
		bool _dryRun;
	public:
		SvnMergeArgs()
		{
			_depth = SvnDepth::Unknown;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Merge;
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

		property bool IgnoreAncestry
		{
			bool get()
			{
				return _ignoreAncestry;
			}
			void set(bool value)
			{
				_ignoreAncestry = value;
			}
		}

		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}

		property bool RecordOnly
		{
			bool get()
			{
				return _recordOnly;
			}
			void set(bool value)
			{
				_recordOnly = value;
			}
		}

		property bool DryRun
		{
			bool get()
			{
				return _dryRun;
			}
			void set(bool value)
			{
				_dryRun = value;
			}
		}
	};

}
