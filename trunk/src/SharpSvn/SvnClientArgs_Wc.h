// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended parameter container for <see cref="SvnClient" />Cleanup</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCleanUpArgs : public SvnClientArgs
	{
	public:
		SvnCleanUpArgs()
		{
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Add(String^, SvnAddArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnAddArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _noIgnore;
		bool _force;
		bool _addParents;

	public:
		SvnAddArgs()
		{
			_depth = SvnDepth::Infinity;
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

		property bool NoIgnore
		{
			bool get()
			{
				return _noIgnore;
			}
			void set(bool value)
			{
				_noIgnore = value;
			}
		}

		/// <summary>If force is not set and path is already under version control, return the error
		/// SVN_ERR_ENTRY_EXISTS. If force is set, do not error on already-versioned items. When used
		/// on a directory in conjunction with the recursive flag, this has the effect of scheduling
		/// for addition unversioned files and directories scattered deep within a versioned tree.</summary>
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

		/// <summary>
		///
		/// </summary>
		property bool AddParents
		{
			bool get()
			{
				return _addParents;
			}
			void set(bool value)
			{
				_addParents = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Relocate(String^, Uri^, Uri^, SvnRelocateArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRelocateArgs : public SvnClientArgs
	{
		bool _nonRecursive;
	public:
		SvnRelocateArgs()
		{
		}

		property bool NonRecursive
		{
			bool get()
			{
				return _nonRecursive;
			}
			void set(bool value)
			{
				_nonRecursive = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Revert(String^, SvnRevertArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRevertArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		ICollection<String^>^ _changelists;

	public:
		SvnRevertArgs()
		{
			_depth = SvnDepth::Empty;
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

	/// <summary>Extended Parameter container of <see cref="SvnClient::Resolved(String^, SvnResolvedArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnResolvedArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		SvnConflictChoice _choice;

	public:
		SvnResolvedArgs()
		{
			_depth = SvnDepth::Empty;
			_choice = SvnConflictChoice::Merged;
		}

		SvnResolvedArgs(SvnConflictChoice choice)
		{
			_depth = SvnDepth::Empty;
			_choice = EnumVerifier::Verify(choice);
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

		property SvnConflictChoice Choice
		{
			SvnConflictChoice get()
			{
				return _choice;
			}
			void set(SvnConflictChoice value)
			{
				_choice = EnumVerifier::Verify(value);
			}
		}
	};

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
			_revision = SvnRevision::Head;
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
				if(value)
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

	/// <summary>Extended Parameter container of <see cref="SvnClient::AddToChangelist(String^,String^, SvnAddToChangelistArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnAddToChangelistArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		ICollection<String^>^ _changelists;
	public:
		SvnAddToChangelistArgs()
		{
			_depth = SvnDepth::Infinity;
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

	/// <summary>Extended Parameter container of <see cref="SvnClient::RemoveFromChangelist(String^,SvnRemoveFromChangelistArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRemoveFromChangelistArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		ICollection<String^>^ _changelists;
	public:
		SvnRemoveFromChangelistArgs()
		{
			_depth = SvnDepth::Infinity;
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


	/// <summary>Extended Parameter container of <see cref="SvnClient::ListChangelist(String^,SvnListChangelistArgs^,EventHandler{SvnListChangelistEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnListChangelistArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		ICollection<String^>^ _changelists;
	public:
		SvnListChangelistArgs()
		{
			_depth = SvnDepth::Infinity;
		}

	public:
		event EventHandler<SvnListChangelistEventArgs^>^ ListChangelist;

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

	protected public:
		virtual void OnListChangelist(SvnListChangelistEventArgs^ e)
		{
			ListChangelist(this, e);
		}
	};
};
