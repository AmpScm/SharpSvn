#pragma once

namespace SharpSvn {
	namespace Delta
	{
		public ref class SvnDeltaDeleteEntryEventArgs : SvnDeltaDirectoryItemEventArgs
		{
			initonly String^ _path;
			initonly __int64 _revision;
		internal:
			SvnDeltaDeleteEntryEventArgs(SvnDeltaNode^ parentDirectory, String^ path, __int64 revision)
				: SvnDeltaDirectoryItemEventArgs(parentDirectory)
			{
				_path = path;
				_revision = revision;
			}

		public:

			property String^ Path
			{
				String^ get()
				{
					return _path;
				}
			}

			property String^ Name
			{
				String^ get()
				{
					return System::IO::Path::GetFileName(Path);
				}
			}
			
			property __int64 Revision
			{
				__int64 get()
				{
					return _revision;
				}
			}
		};

		public ref class SvnDeltaDirectoryAddEventArgs : SvnDeltaNodeEventArgs
		{
			initonly String^ _copyFromPath;
			initonly __int64 _copyFromRev;
		internal:
			SvnDeltaDirectoryAddEventArgs(SvnDeltaNode^ directoryNode, String^ copy_from_path, __int64 copy_from_rev)
				: SvnDeltaNodeEventArgs(directoryNode)
			{
				_copyFromPath = copy_from_path;
				_copyFromRev = copy_from_rev;
			}

		public:
			property String^ CopyFromPath
			{
				String^ get()
				{
					return _copyFromPath;
				}
			}

			property __int64 CopyFromRevision
			{
				__int64 get()
				{
					return _copyFromRev;
				}
			}
		};

		public ref class SvnDeltaDirectoryOpenEventArgs : SvnDeltaNodeEventArgs
		{
			initonly __int64 _baseRevision;
		internal:
			SvnDeltaDirectoryOpenEventArgs(SvnDeltaNode^ directoryNode, __int64 baseRevision)
				: SvnDeltaNodeEventArgs(directoryNode)
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

		public ref class SvnDeltaDirectoryPropertyChangeEventArgs : SvnDeltaNodeEventArgs
		{
		internal:
			SvnDeltaDirectoryPropertyChangeEventArgs(SvnDeltaNode^ directoryNode, const char* name, const svn_string_t* value)
				: SvnDeltaNodeEventArgs(directoryNode)
			{
				UNUSED_ALWAYS(name);
				UNUSED_ALWAYS(value);
			}
		};

		public ref class SvnDeltaDirectoryAbsentEventArgs : SvnDeltaNodeEventArgs
		{
		internal:
			SvnDeltaDirectoryAbsentEventArgs(SvnDeltaNode^ directoryNode)
				: SvnDeltaNodeEventArgs(directoryNode)
			{
			}
		};

		public ref class SvnDeltaDirectoryCloseEventArgs : SvnDeltaNodeEventArgs
		{
		internal:
			SvnDeltaDirectoryCloseEventArgs(SvnDeltaNode^ directoryNode)
				: SvnDeltaNodeEventArgs(directoryNode)
			{
			}
		};

	};
};
