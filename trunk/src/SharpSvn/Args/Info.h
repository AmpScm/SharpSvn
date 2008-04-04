// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of SvnClient.Info</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnInfoArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;

	public:
		event EventHandler<SvnInfoEventArgs^>^ Info;

	protected public:
		virtual void OnInfo(SvnInfoEventArgs^ e)
		{
			Info(this, e);
		}

	public:
		SvnInfoArgs()
		{
			_revision = SvnRevision::None;
			_depth = SvnDepth::Empty;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Info;
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
