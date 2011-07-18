#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemotePropertiesArgs : public SvnRemoteSessionArgs
	{
		__int64 _revision;
		SvnNodeKind _nodeKind;

	public:
		SvnRemotePropertiesArgs()
		{
			_revision = -1L;
			_nodeKind = SvnNodeKind::Unknown;
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
			void set(__int64 value)
			{
				_revision = (value >= 0) ? value : -1;
			}
		}

		/// <summary>Only retrieve the properties if the node is of the specified kind. Unknown for retrieving the kind</summary>
		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}

			void set(SvnNodeKind value)
			{
				_nodeKind = EnumVerifier::Verify(value);
			}
		}
	};
}
}