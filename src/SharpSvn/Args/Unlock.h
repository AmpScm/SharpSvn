// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Unlock(String^, SvnUnlockArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnUnlockArgs : public SvnClientArgs
	{
		bool _breakLock;
		SvnException^ _result;

	public:
		SvnUnlockArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unlock;
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
				case SvnNotifyAction::LockFailedUnlock:
					if(e->Error)
						_result = e->Error;
					else
						_result = gcnew SvnFileSystemLockException(String::Format("Locking failed on {0}", e->Path));
					break;
			}
			__super::RaiseOnNotify(e);
		}

		property SvnException^ UnlockResult
		{
			SvnException^ get()
			{
				return _result;
			}
		}
	};

}
