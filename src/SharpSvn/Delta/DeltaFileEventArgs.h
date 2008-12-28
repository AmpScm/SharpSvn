#pragma once

namespace SharpSvn {
	namespace Delta
	{
		public ref class SvnDeltaFileEventArgs abstract : SvnDeltaEventArgs
		{
			initonly SvnDeltaFileNode^ _fileNode;
		private protected:
			SvnDeltaFileEventArgs(SvnDeltaFileNode^ fileNode)
			{
				if (!fileNode)
					throw gcnew ArgumentNullException("fileNode");

				_fileNode = fileNode;
			}

		public:
			property SvnDeltaFileNode^ FileNode
			{
				SvnDeltaFileNode^ get()
				{
					return _fileNode;
				}
			}

			property String^ Name
			{
				String^ get()
				{
					return FileNode->Name;
				}
			}

			property SvnDeltaDirectoryNode^ ParentDirectory
			{
				SvnDeltaDirectoryNode^ get()
				{
					return FileNode->ParentDirectory;
				}
			}
		};

		public ref class SvnDeltaFileAddEventArgs : SvnDeltaFileEventArgs
		{
			initonly String^ _copyFromPath;
			initonly __int64 _copyFromRev;
		internal:
			SvnDeltaFileAddEventArgs(SvnDeltaFileNode^ fileNode, String^ copy_from_path, __int64 copy_from_rev)
				: SvnDeltaFileEventArgs(fileNode)
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

		public ref class SvnDeltaFileOpenEventArgs : SvnDeltaFileEventArgs
		{
			initonly __int64 _baseRevision;
		internal:
			SvnDeltaFileOpenEventArgs(SvnDeltaFileNode^ fileNode, __int64 baseRevision)
				: SvnDeltaFileEventArgs(fileNode)
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

		public ref class SvnDeltaFilePropertyChangeEventArgs : SvnDeltaFileEventArgs
		{
		internal:
			SvnDeltaFilePropertyChangeEventArgs(SvnDeltaFileNode^ fileNode, const char* name, const svn_string_t* value)
				: SvnDeltaFileEventArgs(fileNode)
			{
				UNUSED_ALWAYS(name);
				UNUSED_ALWAYS(value);
			}
		};

		public ref class SvnDeltaFileAbsentEventArgs : SvnDeltaEventArgs
		{
			initonly SvnDeltaDirectoryNode^ _parentDirectory;
			String^ _name;

			const char* _pName;
		internal:
			SvnDeltaFileAbsentEventArgs(SvnDeltaDirectoryNode^ parentDirectory, const char* name)
			{
				_parentDirectory = parentDirectory;
				_pName = name;
			}

		public:
			property SvnDeltaDirectoryNode^ ParentDirectory
			{
				SvnDeltaDirectoryNode^ get()
				{
					return _parentDirectory;
				}
			}

			property String^ Name
			{
				String^ get()
				{
					if (!_name && _pName)
						_name = SvnBase::Utf8_PtrToString(_pName);

					return _name;
				}
			}

		internal:
			virtual void Detach(bool keepProperties) override
			{
				try
				{
					if (keepProperties)
					{
						GC::KeepAlive(Name);
					}
				}
				finally
				{
					_pName = nullptr;
					__super::Detach(keepProperties);
				}
			}	
		};

		public ref class SvnDeltaFileCloseEventArgs : SvnDeltaFileEventArgs
		{
		internal:
			SvnDeltaFileCloseEventArgs(SvnDeltaFileNode^ fileNode, const char* checksum)
				: SvnDeltaFileEventArgs(fileNode)
			{
				UNUSED_ALWAYS(checksum);
			}
		};

		public ref class SvnDeltaBeforeFileDeltaEventArgs : SvnDeltaFileEventArgs
		{
		internal:
			SvnDeltaBeforeFileDeltaEventArgs(SvnDeltaFileNode^ fileNode, const char* base_checksum)
				: SvnDeltaFileEventArgs(fileNode)
			{
				UNUSED_ALWAYS(base_checksum);
			}
		};
	};
};