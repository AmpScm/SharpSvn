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
