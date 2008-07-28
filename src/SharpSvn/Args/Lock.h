// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Lock(String^, SvnLockArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLockArgs : public SvnClientArgs
	{
		String^ _comment;
		bool _stealLock;
		SvnException^ _result;

	public:
		SvnLockArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Lock;
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

	internal:
		void Reset()
		{
			_result = nullptr;
		}

		virtual void RaiseOnNotify(SvnNotifyEventArgs^ e) override
		{
			if(!_result)
				switch(e->Action)
			{
				case SvnNotifyAction::LockFailedLock:
					if(e->Error)
						_result = e->Error;
					else
						_result = gcnew SvnFileSystemLockException(String::Format("Locking failed on {0}", e->Path));
					break;
			}

			__super::RaiseOnNotify(e);
		}

		property SvnException^ LockResult
		{
			SvnException^ get()
			{
				return _result;
			}
		}
	};

}
