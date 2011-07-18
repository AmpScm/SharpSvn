#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemoteLocationSegmentsArgs : public SvnRemoteSessionArgs
	{
        __int64 _pegRevision;
        __int64 _startRevision;
        __int64 _endRevision;
	public:
		DECLARE_EVENT(SvnRemoteLocationSegmentEventArgs^, LocationSegment)

	protected public:
		virtual void OnLocationSegment(SvnRemoteLocationSegmentEventArgs^ e)
		{
			LocationSegment(this, e);
		}

	public:
		SvnRemoteLocationSegmentsArgs()
		{
            _pegRevision = -1L;
            _startRevision = -1L;
            _endRevision = -1L;
		}

		/// <summary>Gets the revision in which the Url's are evaluated (Aka peg revision)</summary>
		property __int64 OperationalRevision
		{
			__int64 get()
			{
				return _pegRevision;
			}
			void set(__int64 value)
			{
				if (value >= 0 )
					_pegRevision = value;
				else
					_pegRevision = -1;
			}
		}

		property __int64 StartRevision
		{
			__int64 get()
			{
				return _startRevision;
			}
			void set(__int64 value)
			{
				if (value >= 0 )
					_startRevision = value;
				else
					_startRevision = -1;
			}
		}

		property __int64 EndRevision
		{
			__int64 get()
			{
				return _endRevision;
			}
			void set(__int64 value)
			{
				if (value >= 0 )
					_endRevision = value;
				else
					_endRevision = -1;
			}
		}

    internal:
        // Storage for while handling events
        Uri^ _reposUri;
        int _nOffset;
    };
}
}