// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {
	using System::Collections::Generic::List;

	/// <summary>Extended Parameter container of <see cref="SvnClient::Commit(String^, SvnCommitArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCommitArgs : public SvnClientArgsWithCommit
	{
		SvnDepth _depth;
		bool _keepLocks;
		bool _keepChangeList;
		SvnChangeListCollection^ _changelists;

	public:
		SvnCommitArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Commit;
			}
		}

	public:
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

		property bool KeepLocks
		{
			bool get()
			{
				return _keepLocks;
			}
			void set(bool value)
			{
				_keepLocks = value;
			}
		}

		/// <summary>Gets or sets the list of changelist-names to commit</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}

		property bool KeepChangeList
		{
			bool get()
			{
				return _keepChangeList;
			}
			void set(bool value)
			{
				_keepChangeList = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Import(String^,Uri^,SvnImportArgs^)" /> and <see cref="SvnClient::RemoteImport(String^,Uri^,SvnImportArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnImportArgs : public SvnClientArgsWithCommit
	{
		bool _noIgnore;
		bool _ignoreUnknownNodeTypes;
		SvnDepth _depth;
	public:
		SvnImportArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Import;
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

		property bool IgnoreUnknownNodeTypes
		{
			bool get()
			{
				return _ignoreUnknownNodeTypes;
			}
			void set(bool value)
			{
				_ignoreUnknownNodeTypes = value;
			}
		}

	internal:
		// Used by SvnImport to forward SvnCheckout errors
		void HandleOnSvnError(Object^ sender, SvnErrorEventArgs^ e)
		{
			UNUSED_ALWAYS(sender);
			OnSvnError(e);
		}
	};
};
