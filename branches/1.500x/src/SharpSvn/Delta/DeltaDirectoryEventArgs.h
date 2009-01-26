// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
			const char* _pName;
			const svn_string_t *_pValue;
			String^ _name;
			SvnPropertyValue^ _value;
			initonly SvnPropertyKind _propKind;

		internal:
			SvnDeltaDirectoryPropertyChangeEventArgs(SvnDeltaNode^ directoryNode, const char* name, const svn_string_t* value)
				: SvnDeltaNodeEventArgs(directoryNode)
			{
				if (!name)
					throw gcnew ArgumentNullException("name");
				// value = null  -> Property delete
				_pName = name;
				_pValue = value;
				if(name && *name)
					_propKind = (SvnPropertyKind)svn_property_kind(nullptr, name);
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

			property SvnPropertyKind PropertyKind
			{
				SvnPropertyKind get()
				{
					return _propKind;
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
