// $Id: SvnTools.h 631 2008-07-28 21:40:00Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once


namespace SharpSvn {
	using namespace SharpSvn::Implementation;

	public ref class SvnHookArguments
	{
	public:
		static bool ParsePostCommitArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePostLockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePostRevPropChangeArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePostUnlockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreCommitArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreLockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreRevPropChangeArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreUnlockArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParseStartCommitArguments(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);

	protected:
		static String^ ReadStdInText();

	private:
		String^ _repositoryPath;
		String^ _txnName;
		__int64 _revision;
		String^ _user;
		String^ _path;
		String^ _propertyName;
		String^ _propertyValue;
		String^ _action;
		ICollection<String^>^ _capabilities;
		String^ _newValue;
		String^ _previousValue;

	protected:
		SvnHookArguments()
		{
			_revision = -1L;
		}

	public:
		property String^ RepositoryPath
		{
			String^ get()
			{
				return _repositoryPath;
			}
		protected:
			void set(String^ value)
			{
				_repositoryPath = value;
			}
		}

		property String^ TransactionName
		{
			String^ get()
			{
				return _txnName;
			}
		protected:
			void set(String^ value)
			{
				_txnName = value;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		protected:
			void set(__int64 value)
			{
				_revision = value;
			}
		}

		property String^ User
		{
			String^ get()
			{
				return _user;
			}
		protected:
			void set(String^ value)
			{
				_user = value;
			}
		}

		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		protected:
			void set(String^ value)
			{
				_path = value;
			}
		}

		property String^ PropertyName
		{
			String^ get()
			{
				return _propertyName;
			}
		protected:
			void set(String^ value)
			{
				_propertyName = value;
			}
		}

		property String^ Action
		{
			String^ get()
			{
				return _action;
			}
		protected:
			void set(String^ value)
			{
				_propertyName = value;
			}
		}

		property ICollection<String^>^ Capabilities
		{
			ICollection<String^>^ get()
			{
				return _capabilities;
			}
		protected:
			void set(ICollection<String^>^ value)
			{
				_capabilities = value;
			}
		}

		property String^ NewValue
		{
			String^ get()
			{
				return _newValue;
			}
		protected:
			void set(String^ value)
			{
				_newValue = value;
			}
		}

		property String^ PreviousValue
		{
			String^ get()
			{
				return _previousValue;
			}
		protected:
			void set(String^ value)
			{
				_previousValue = value;
			}
		}
	};
}