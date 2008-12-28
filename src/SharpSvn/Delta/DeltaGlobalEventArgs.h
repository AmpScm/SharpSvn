#pragma once

namespace SharpSvn {
	namespace Delta
	{
		public ref class SvnDeltaSetTargetEventArgs : SvnDeltaEventArgs
		{
			initonly __int64 _revision;
		internal:
			SvnDeltaSetTargetEventArgs(__int64 targetRevision)
			{
				_revision = targetRevision;
			}

		public:
			property __int64 TargetRevision
			{
				__int64 get()
				{
					return _revision;
				}
			}
		};

		public ref class SvnDeltaOpenEventArgs : SvnDeltaEventArgs
		{
			initonly __int64 _baseRevision;
		internal:
			SvnDeltaOpenEventArgs(__int64 baseRevision)
			{
				_baseRevision = baseRevision;
			}

		public:
			property __int64 BaseRevision
			{
				__int64 get()
				{
					return _baseRevision;
				}
			}
		};

		public ref class SvnDeltaCloseEventArgs : SvnDeltaEventArgs
		{
		internal:
			SvnDeltaCloseEventArgs()
			{
			}
		};

		public ref class SvnDeltaAbortEventArgs : SvnDeltaEventArgs
		{
		internal:
			SvnDeltaAbortEventArgs()
			{
			}
		};
	};
};
