#pragma once

#include "SvnRemoteSessionArgs.h"


namespace SharpSvn {

	public ref class SvnRemoteListArgs : public SvnRemoteSessionArgs
	{
		SvnDirEntryItems _entryItems;

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