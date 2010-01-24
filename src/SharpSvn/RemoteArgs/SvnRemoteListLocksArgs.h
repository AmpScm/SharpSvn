#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemoteListLocksArgs : public SvnRemoteSessionArgs
	{
	public:
		DECLARE_EVENT(SvnRemoteListLockEventArgs^, List)

	protected public:
		virtual void OnList(SvnRemoteListLockEventArgs^ e)
		{
			List(this, e);
		}

	public:
		SvnRemoteListLocksArgs()
		{
		}
	};
}
}