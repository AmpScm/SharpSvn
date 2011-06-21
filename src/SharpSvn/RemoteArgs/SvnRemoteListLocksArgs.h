#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemoteListLocksArgs : public SvnRemoteSessionArgs
	{
		SvnDepth _depth;
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
			_depth = SvnDepth::Infinity;
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
	};
}
}