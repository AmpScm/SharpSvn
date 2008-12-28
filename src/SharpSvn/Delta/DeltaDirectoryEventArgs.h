#pragma once

namespace SharpSvn {
	namespace Delta
	{
		public ref class SvnDeltaDeleteEntryEventArgs : SvnDeltaEventArgs
		{
			initonly SvnDeltaDirectoryNode^ _parentDirectory;
			initonly String^ _name;
			initonly __int64 _revision;
		internal:
			SvnDeltaDeleteEntryEventArgs(SvnDeltaDirectoryNode^ parentDirectory, String^ name, __int64 revision)
			{
				_parentDirectory = parentDirectory;
				_name = name;
				_revision = revision;
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
					return _name;
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

		public ref class SvnDeltaDirectoryEventArgs : SvnDeltaEventArgs
		{
		internal:
			initonly SvnDeltaDirectoryNode^ _directoryNode;
		private protected:
			SvnDeltaDirectoryEventArgs(SvnDeltaDirectoryNode^ directoryNode)
			{
				if (!directoryNode)
					throw gcnew ArgumentNullException("directoryNode");

				_directoryNode = directoryNode;
			}

		public:
			property SvnDeltaDirectoryNode^ DirectoryNode
			{
				SvnDeltaDirectoryNode^ get()
				{
					return _directoryNode;
				}
			}

			property String^ Name
			{
				String^ get()
				{
					return DirectoryNode->Name;
				}
			}

			property SvnDeltaDirectoryNode^ ParentDirectory
			{
				SvnDeltaDirectoryNode^ get()
				{
					return DirectoryNode->ParentDirectory;
				}
			}
		};

		public ref class SvnDeltaDirectoryAddEventArgs : SvnDeltaDirectoryEventArgs
		{
			initonly String^ _copyFromPath;
			initonly __int64 _copyFromRev;
		internal:
			SvnDeltaDirectoryAddEventArgs(SvnDeltaDirectoryNode^ directoryNode, String^ copy_from_path, __int64 copy_from_rev)
				: SvnDeltaDirectoryEventArgs(directoryNode)
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

		public ref class SvnDeltaDirectoryOpenEventArgs : SvnDeltaDirectoryEventArgs
		{
			initonly __int64 _baseRevision;
		internal:
			SvnDeltaDirectoryOpenEventArgs(SvnDeltaDirectoryNode^ directoryNode, __int64 baseRevision)
				: SvnDeltaDirectoryEventArgs(directoryNode)
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

		public ref class SvnDeltaDirectoryPropertyChangeEventArgs : SvnDeltaDirectoryEventArgs
		{
		internal:
			SvnDeltaDirectoryPropertyChangeEventArgs(SvnDeltaDirectoryNode^ directoryNode, const char* name, const svn_string_t* value)
				: SvnDeltaDirectoryEventArgs(directoryNode)
			{
				UNUSED_ALWAYS(name);
				UNUSED_ALWAYS(value);
			}
		};

		public ref class SvnDeltaDirectoryAbsentEventArgs : SvnDeltaEventArgs
		{
			initonly SvnDeltaDirectoryNode^ _parentDirectory;
			String^ _name;

			const char* _pName;
		internal:
			SvnDeltaDirectoryAbsentEventArgs(SvnDeltaDirectoryNode^ parentDirectory, const char* name)
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

		public ref class SvnDeltaDirectoryCloseEventArgs : SvnDeltaDirectoryEventArgs
		{
		internal:
			SvnDeltaDirectoryCloseEventArgs(SvnDeltaDirectoryNode^ directoryNode)
				: SvnDeltaDirectoryEventArgs(directoryNode)
			{
			}
		};

	};
};
