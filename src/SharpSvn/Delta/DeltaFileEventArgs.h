#pragma once

namespace SharpSvn {
	namespace Delta
	{
		public ref class SvnDeltaFileAddEventArgs : SvnDeltaNodeEventArgs
		{
			initonly String^ _copyFromPath;
			initonly __int64 _copyFromRev;
		internal:
			SvnDeltaFileAddEventArgs(SvnDeltaNode^ fileNode, String^ copy_from_path, __int64 copy_from_rev)
				: SvnDeltaNodeEventArgs(fileNode)
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

		public ref class SvnDeltaFileOpenEventArgs : SvnDeltaNodeEventArgs
		{
			initonly __int64 _baseRevision;
		internal:
			SvnDeltaFileOpenEventArgs(SvnDeltaNode^ fileNode, __int64 baseRevision)
				: SvnDeltaNodeEventArgs(fileNode)
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

		public ref class SvnDeltaFilePropertyChangeEventArgs : SvnDeltaNodeEventArgs
		{
			const char* _pName;
			const svn_string_t *_pValue;
			String^ _name;
			SvnPropertyValue^ _value;

		internal:
			SvnDeltaFilePropertyChangeEventArgs(SvnDeltaNode^ fileNode, const char* name, const svn_string_t* value)
				: SvnDeltaNodeEventArgs(fileNode)
			{
				if (!name)
					throw gcnew ArgumentNullException("name");
				// value = null  -> Property delete
				_pName = name;
				_pValue = value;
			}

		public:
			property SvnPropertyValue^ Value
			{
				SvnPropertyValue^ get()
				{
					if (!_value && _pValue && _pName && PropertyName)
						_value = SvnPropertyValue::Create(_pName, _pValue, nullptr, PropertyName);

					return _value;
				}
			}

			property String^ PropertyName
			{
				String^ get()
				{
					if (!_name && _pName)
						_name = SvnBase::Utf8_PtrToString(_pName);

					return _name;
				}
			}

		protected public:
			virtual void Detach(bool keepProperties) override
			{
				try
				{
					if (keepProperties)
					{
						GC::KeepAlive(Name);
						GC::KeepAlive(Value);
					}
				}
				finally
				{
					_name = nullptr;
					_value = nullptr;

					__super::Detach(keepProperties);
				}
			}
		};

		public ref class SvnDeltaFileAbsentEventArgs : SvnDeltaNodeEventArgs
		{
		internal:
			SvnDeltaFileAbsentEventArgs(SvnDeltaNode^ fileNode)
				: SvnDeltaNodeEventArgs(fileNode)
			{
			}
		};

		public ref class SvnDeltaFileCloseEventArgs : SvnDeltaNodeEventArgs
		{
		internal:
			SvnDeltaFileCloseEventArgs(SvnDeltaNode^ fileNode, const char* checksum)
				: SvnDeltaNodeEventArgs(fileNode)
			{
				UNUSED_ALWAYS(checksum);
			}
		};

		public ref class SvnDeltaBeforeFileDeltaEventArgs : SvnDeltaNodeEventArgs
		{
		internal:
			SvnDeltaBeforeFileDeltaEventArgs(SvnDeltaNode^ fileNode, const char* base_checksum)
				: SvnDeltaNodeEventArgs(fileNode)
			{
				UNUSED_ALWAYS(base_checksum);
			}
		};
	};
};