// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#pragma once

namespace SharpSvn {
	using namespace SharpSvn::Implementation;

	public enum class SvnHookType
	{
		None,
		PostCommit,
		PostLock,
		PostRevPropChange,
		PostUnlock,
		PreCommit,
		PreLock,
		PreRevPropChange,
		PreUnlock,
		StartCommit,
	};

	public ref class SvnHookArguments
	{
	public:
		static bool ParseHookArguments(array<String^>^ args, SvnHookType hookType, bool useConsole, [Out] SvnHookArguments^% data);
	internal:
		static bool ParsePostCommit(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePostLock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePostRevPropChange(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePostUnlock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreCommit(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreLock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreRevPropChange(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParsePreUnlock(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);
		static bool ParseStartCommit(array<String^>^ args, bool useConsole, [Out] SvnHookArguments^% data);

	public:
		static String^ GetHookFileName(String^ path, SvnHookType hookType);

	protected:
		static String^ ReadStdInText();

	private:
		String^ _repositoryPath;
		String^ _txnName;
		__int64 _revision;
		String^ _user;
		String^ _path;
		String^ _propertyName;
		String^ _action;
		Collection<String^>^ _capabilities;
		String^ _newValue;
		String^ _previousValue;
		SvnLookOrigin^ _lookOrigin;

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

		/// <summary>Gets a <see cref="SvnLookOrigin" /> instance for the current hook request</summary>
		property SvnLookOrigin^ LookOrigin
		{
			SvnLookOrigin^ get()
			{
				if (!_lookOrigin)
				{
					if (TransactionName)
						_lookOrigin = gcnew SvnLookOrigin(RepositoryPath, TransactionName);
					else if(Revision >= 0)
						_lookOrigin = gcnew SvnLookOrigin(RepositoryPath, Revision);
					else
						throw gcnew InvalidOperationException();
				}
				return _lookOrigin;
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
				_action = value;
			}
		}

		property Collection<String^>^ Capabilities
		{
			Collection<String^>^ get()
			{
				return _capabilities;
			}
		internal:
			void set(Collection<String^>^ value)
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