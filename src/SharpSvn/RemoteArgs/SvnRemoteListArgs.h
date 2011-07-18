#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemoteListArgs : public SvnRemoteSessionArgs
	{
		SvnDirEntryItems _entryItems;
		__int64 _revision;

	public:
		DECLARE_EVENT(SvnRemoteListEventArgs^, List)

	protected public:
		virtual void OnList(SvnRemoteListEventArgs^ e)
		{
			List(this, e);
		}

	public:
		SvnRemoteListArgs()
		{
			_entryItems = SvnDirEntryItems::SvnListDefault;
			_revision = -1L;
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

		property SvnDirEntryItems RetrieveEntries
		{
			SvnDirEntryItems get()
			{
				return _entryItems;
			}

			void set(SvnDirEntryItems value)
			{
				_entryItems = value;
			}
		}

	};
}
}