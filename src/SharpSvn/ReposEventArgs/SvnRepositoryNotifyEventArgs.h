// $Id: SvnMergesEligibleEventArgs.h 944 2008-12-12 10:20:50Z rhuijben $
//
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

#include "EventArgs\SvnEventArgs.h"

namespace SharpSvn {

	public ref class SvnRepositoryNotifyEventArgs sealed : public SvnEventArgs
	{
	private:
		const svn_repos_notify_t *_notify;
		AprPool^ _pool;
		initonly SvnRepositoryNotifyAction _action;

	internal:
		SvnRepositoryNotifyEventArgs(const svn_repos_notify_t *notify, AprPool^ pool)
		{
			_notify = notify;
			_pool = pool;
			_action = (SvnRepositoryNotifyAction)notify->action;
		}

	public:
		property SvnRepositoryNotifyAction Action
		{
			SvnRepositoryNotifyAction get()
			{
				return _action;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					// GC::KeepAlive()
				}
			}
			finally
			{
				_notify = nullptr;
				_pool = nullptr;
			}
		}
	};
}