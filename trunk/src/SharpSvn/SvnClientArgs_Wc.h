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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::CleanUp;
			}
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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Add;
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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Relocate;
			}
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
		SvnChangeListCollection^ _changelists;

	public:
		SvnRevertArgs()
		{
			_depth = SvnDepth::Empty;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Revert;
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
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Resolved;
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

	/// <summary>Extended Parameter container of <see cref="SvnClient::AddToChangeList(String^,String^, SvnAddToChangeListArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnAddToChangeListArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;
	public:
		SvnAddToChangeListArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::AddToChangeList;
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
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::RemoveFromChangeList(String^,SvnRemoveFromChangeListArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRemoveFromChangeListArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;
	public:
		SvnRemoveFromChangeListArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::RemoveFromChangeList;
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
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};


	/// <summary>Extended Parameter container of <see cref="SvnClient::ListChangeList(String^,SvnListChangeListArgs^,EventHandler{SvnListChangeListEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnListChangeListArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;
	public:
		SvnListChangeListArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::ListChangeList;
			}
		}

	public:
		event EventHandler<SvnListChangeListEventArgs^>^ ListChangeList;

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
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}

	protected public:
		virtual void OnListChangeList(SvnListChangeListEventArgs^ e)
		{
			ListChangeList(this, e);
		}
	};
};
