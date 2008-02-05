// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Lock(String^, SvnLockArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLockArgs : public SvnClientArgs
	{
		String^ _comment;
		bool _stealLock;

	public:
		SvnLockArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Lock;
			}
		}

		property String^ Comment
		{
			String^ get()
			{
				return _comment ? _comment : "";
			}

			void set(String^ value)
			{
				_comment = value;
			}
		}

		property bool StealLock
		{
			bool get()
			{
				return _stealLock;
			}
			void set(bool value)
			{
				_stealLock = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Unlock(String^, SvnUnlockArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnUnlockArgs : public SvnClientArgs
	{
		bool _breakLock;

	public:
		SvnUnlockArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Unlock;
			}
		}

		property bool BreakLock
		{
			bool get()
			{
				return _breakLock;
			}
			void set(bool value)
			{
				_breakLock = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Copy(SvnTarget^,String^,SvnCopyArgs^)" /> and 
	/// <see cref="SvnClient::RemoteCopy(SvnUriTarget^,Uri^,SvnCopyArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCopyArgs : public SvnClientArgsWithCommit
	{
		bool _makeParents;
		bool _alwaysCopyBelow;

	public:
		SvnCopyArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Copy;
			}
		}

		property bool MakeParents
		{
			bool get()
			{
				return _makeParents;
			}
			void set(bool value)
			{
				_makeParents = value;
			}
		}

		/// <summary>Always copies the result to below the target (this behaviour is always used if multiple targets are provided)</summary>
		property bool AlwaysCopyAsChild
		{
			bool get()
			{
				return _alwaysCopyBelow;
			}
			void set(bool value)
			{
				_alwaysCopyBelow = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Move(String^,String^,SvnMoveArgs^)" /> and 
	/// <see cref="SvnClient::RemoteMove(Uri^,Uri^,SvnMoveArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnMoveArgs : public SvnClientArgs
	{
		bool _force;
		bool _moveAsChild;
		bool _makeParents;
	public:
		SvnMoveArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Move;
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

		property bool AlwaysMoveAsChild
		{
			bool get()
			{
				return _moveAsChild;
			}
			void set(bool value)
			{
				_moveAsChild = value;
			}
		}

		property bool MakeParents
		{
			bool get()
			{
				return _makeParents;
			}
			void set(bool value)
			{
				_makeParents = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::CreateDirectory(String^, SvnCreateDirectoryArgs^)" /> and <see cref="SvnClient::RemoteCreateDirectory(Uri^, SvnCreateDirectoryArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCreateDirectoryArgs : public SvnClientArgsWithCommit
	{
		bool _makeParents;
	public:
		SvnCreateDirectoryArgs()
		{}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::CreateDirectory;
			}
		}

		property bool MakeParents
		{
			bool get()
			{
				return _makeParents;
			}

			void set(bool value)
			{
				_makeParents = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Delete(String^,SvnDeleteArgs^)" /> and <see cref="SvnClient::RemoteDelete(Uri^,SvnDeleteArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDeleteArgs : public SvnClientArgsWithCommit
	{
		bool _force;
		bool _keepLocal;
	public:
		SvnDeleteArgs()
		{}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Delete;
			}
		}

		/// <summary>If Force is not set then this operation will fail if any path contains locally modified
		/// and/or unversioned items. If Force is set such items will be deleted.</summary>
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

		property bool KeepLocal
		{
			bool get()
			{
				return _keepLocal;
			}
			void set(bool value)
			{
				_keepLocal = value;
			}
		}
	};
};
