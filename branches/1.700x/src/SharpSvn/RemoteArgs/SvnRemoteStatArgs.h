#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemoteStatArgs : public SvnRemoteSessionArgs
	{
		__int64 _revision;

	public:
		SvnRemoteStatArgs()
		{
			_revision = -1L;
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
			void set(__int64 value)
			{
				if (value < 0 || value == __int64::MaxValue)
					_revision = -1L;
				else
					_revision = value;
			}
		}
	};
}
}
