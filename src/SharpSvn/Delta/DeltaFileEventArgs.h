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

#include "SvnDeltaTransform.h"

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
			initonly SvnPropertyKind _propKind;

		internal:
			SvnDeltaFilePropertyChangeEventArgs(SvnDeltaNode^ fileNode, const char* name, const svn_string_t* value)
				: SvnDeltaNodeEventArgs(fileNode)
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

		public ref class SvnDeltaFileChangeEventArgs : SvnDeltaNodeEventArgs
		{
			AprPool^ _pool;
			const char* _pBaseChecksum;
			String^ _baseMd5Checksum;
			SvnDeltaTarget^ _target;
		internal:
			SvnDeltaFileChangeEventArgs(SvnDeltaNode^ fileNode, const char* base_checksum, AprPool^ pool)
				: SvnDeltaNodeEventArgs(fileNode)
			{
				if (!pool)
					throw gcnew ArgumentNullException("pool");

				_pool = pool;
				_pBaseChecksum = base_checksum;
			}

		internal:
			property AprPool^ Pool
			{
				AprPool^ get()
				{
					return _pool;
				}
			}

		public:
			property String^ BaseMD5
			{
				String^ get()
				{
					if (!_baseMd5Checksum && _pBaseChecksum)
						_baseMd5Checksum = SvnBase::Utf8_PtrToString(_pBaseChecksum);

					return _baseMd5Checksum;
				}
			}

			/// <summary>Gets or sets the <see cref="SvnDeltaTarget" /> that receives the actual differences
			/// after this event returns</summary>
			property SvnDeltaTarget^ Target
			{
				SvnDeltaTarget^ get()
				{
					return _target;
				}
				void set(SvnDeltaTarget^ value)
				{
					if (value && value->IsDisposed)
						throw gcnew ObjectDisposedException("value");

					_target = value;
				}
			}

		public:
			/// <summary>Raised when <see cref="Target" /> is not null and completes its processing</summary>
			DECLARE_EVENT(SvnDeltaCompleteEventArgs^, DeltaComplete);

		protected:
			void OnDeltaComplete(SvnDeltaCompleteEventArgs^ e)
			{
				DeltaComplete(this, e);
			}

		internal:
			void InvokeDeltaComplete(System::Object ^sender,SharpSvn::Delta::SvnDeltaCompleteEventArgs^ e)
			{
				UNUSED_ALWAYS(sender);
				OnDeltaComplete(e);
			}

			void PrepareForDelta(SvnDeltaTarget^ target)
			{
				if (!target)
					throw gcnew ArgumentNullException("target");

				target->InvokeFileChange(this);

				if (event_DeltaComplete)
				{
					Detach();

					target->DeltaComplete += gcnew EventHandler<SvnDeltaCompleteEventArgs^>(
							this, &SvnDeltaFileChangeEventArgs::InvokeDeltaComplete);
				}
			}

		protected public:
			virtual void Detach(bool keepProperties) override
			{
				try
				{
					if (keepProperties)
					{
						GC::KeepAlive(BaseMD5);
					}
				}
				finally
				{
					if(!keepProperties)
						_pool = nullptr; // Don't lose the pool when a user asks for detach!

					_pBaseChecksum = nullptr;

					__super::Detach(keepProperties);
				}
			}
		};
	};
};