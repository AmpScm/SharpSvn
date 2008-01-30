// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

/// <summary>Extended Parameter container of <see cref="SvnClient::SetProperty(String^,String^,String^,SvnSetPropertyArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnSetPropertyArgs : public SvnClientArgsWithCommit
	{
		SvnDepth _depth;
		bool _skipChecks;
		__int64 _baseRevision;
		ICollection<String^>^ _changelists;
	public:
		SvnSetPropertyArgs()
		{
			_depth = SvnDepth::Empty;
			_baseRevision = SVN_INVALID_REVNUM;
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

		property bool SkipChecks
		{
			bool get()
			{
				return _skipChecks;
			}
			void set(bool value)
			{
				_skipChecks = value;
			}
		}

		property __int64 BaseRevision
		{
			__int64 get()
			{
				return _baseRevision;
			}
			void set(__int64 value)
			{
				if(value >= 0)
					_baseRevision = value;
				else
					_baseRevision = SVN_INVALID_REVNUM;
			}
		}

		/// <summary>Gets or sets the list of changelist-names</summary>
		property ICollection<String^>^ Changelists
		{
			ICollection<String^>^ get()
			{
				if(!_changelists)
					_changelists = gcnew List<String^>();
				return _changelists;
			}
			void set(ICollection<String^>^ value)
			{
				if(value)
					_changelists = gcnew List<String^>(value);
				else
					_changelists = nullptr;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetProperty</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetPropertyArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		ICollection<String^>^ _changelists;
	public:
		SvnGetPropertyArgs()
		{
			_depth = SvnDepth::Empty;
			_revision = SvnRevision::None;
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
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

		/// <summary>Gets or sets the list of changelist-names</summary>
		property ICollection<String^>^ Changelists
		{
			ICollection<String^>^ get()
			{
				if(!_changelists)
					_changelists = gcnew List<String^>();
				return _changelists;
			}
			void set(ICollection<String^>^ value)
			{
				if(value)
					_changelists = gcnew List<String^>(value);
				else
					_changelists = nullptr;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::PropertyList(SvnTarget^, SvnPropertyListArgs^, EventHandler{SvnPropertyListEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnPropertyListArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		ICollection<String^>^ _changelists;

	public:
		event EventHandler<SvnPropertyListEventArgs^>^ PropertyList;

	protected public:
		virtual void OnPropertyList(SvnPropertyListEventArgs^ e)
		{
			PropertyList(this, e);
		}

	public:
		SvnPropertyListArgs()
		{
			_depth = SvnDepth::Empty;
			_revision = SvnRevision::None;
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
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

		/// <summary>Gets or sets the list of changelist-names</summary>
		property ICollection<String^>^ Changelists
		{
			ICollection<String^>^ get()
			{
				if(!_changelists)
					_changelists = gcnew List<String^>();
				return _changelists;
			}
			void set(ICollection<String^>^ value)
			{
				if(value)
					_changelists = gcnew List<String^>(value);
				else
					_changelists = nullptr;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::SetRevisionProperty(SvnUriTarget^,String^,SvnSetRevisionPropertyArgs^,String^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnSetRevisionPropertyArgs : public SvnClientArgs
	{
		bool _force;
	public:
		SvnSetRevisionPropertyArgs()
		{
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
	};
			
	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetRevisionProperty</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetRevisionPropertyArgs : public SvnClientArgs
	{
	public:
		SvnGetRevisionPropertyArgs()
		{
		}		
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetRevisionPropertyList</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRevisionPropertyListArgs : public SvnClientArgs
	{
	public:
		SvnRevisionPropertyListArgs()
		{
		}
	};

};