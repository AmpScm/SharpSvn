// Copyright 2007-2008 The SharpSvn Project
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

// Included from SvnClientEventArgs.h
#include "SvnConflictData.h"

namespace SharpSvn {

	public ref class SvnConflictEventArgs sealed : public SvnCancelEventArgs
	{
		const svn_wc_conflict_description2_t* _description;
		AprPool^ _pool;
		SvnAccept _result;

		SvnConflictData^ _data;
		String^ _mergeResult;

	internal:
		// BH: Note svn_wc_conflict_description_t is also mapped by SvnConflictData
		// for the non-event case
		SvnConflictEventArgs(const svn_wc_conflict_description2_t *description, AprPool^ pool)
		{
			if (!description)
				throw gcnew ArgumentNullException("description");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_description = description;
			_pool = pool;
			_result = SvnAccept::Postpone;
		}

	public:
		property SvnConflictData^ Conflict
		{
			SvnConflictData^ get()
			{
				if (!_data && _description && _pool)
					_data = gcnew SvnConflictData(_description, _pool);

				return _data;
			}
		}

		property SvnAccept Choice
		{
			SvnAccept get()
			{
				return _result;
			}
			void set(SvnAccept value)
			{
				_result = EnumVerifier::Verify(value);
			}
		}

	public:
		/// <summary><see cref="SvnConflictData::Name" /></summary>
		property String^ Path
		{
			String^ get()
			{
				return Conflict ? Conflict->Name : nullptr;
			}
		}

		/// <summary><see cref="SvnConflictData::PropertyName" /></summary>
		property String^ PropertyName
		{
			String^ get()
			{
				return Conflict ? Conflict->PropertyName : nullptr;
			}
		}

		/// <summary><see cref="SvnConflictData::MimeType" /></summary>
		property String^ MimeType
		{
			String^ get()
			{
				return Conflict ? Conflict->MimeType : nullptr;
			}
		}

		property String^ BaseFile
		{
			String^ get()
			{
				return Conflict ? Conflict->BaseFile : nullptr;
			}
		}

		property String^ TheirFile
		{
			String^ get()
			{
				return Conflict ? Conflict->TheirFile : nullptr;
			}
		}

		property String^ MyFile
		{
			String^ get()
			{
				return Conflict ? Conflict->MyFile : nullptr;
			}
		}

		property String^ MergedFile
		{
			String^ get()
			{
				if (_mergeResult)
					return _mergeResult;

				return Conflict ? Conflict->MergedFile : nullptr;
			}
			void set(String^ value)
			{
				if (String::IsNullOrEmpty(value) && (value != MergedFile))
					throw gcnew InvalidOperationException("Only settable with valid filename");

				_mergeResult = value;
			}
		}

		property bool IsBinary
		{
			bool get()
			{
				return Conflict && Conflict->IsBinary;
			}
		}

		property SvnConflictAction ConflictAction
		{
			SvnConflictAction get()
			{
				return Conflict ? Conflict->ConflictAction : (SvnConflictAction)0;
			}
		}

		property SvnConflictReason ConflictReason
		{
			SvnConflictReason get()
			{
				return Conflict ? Conflict->ConflictReason : (SvnConflictReason)0;
			}
		}

		property SvnConflictType ConflictType
		{
			SvnConflictType get()
			{
				return Conflict ? Conflict->ConflictType : (SvnConflictType)0;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return Conflict ? Conflict->NodeKind : SvnNodeKind::None;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Conflict);
				}

				if (_data)
					_data->Detach(keepProperties);
			}
			finally
			{
				_pool = nullptr;
				_description = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
}

