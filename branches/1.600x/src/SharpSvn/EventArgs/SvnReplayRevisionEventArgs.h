// $Id: SvnConflictEventArgs.h 944 2008-12-12 10:20:50Z rhuijben $
//
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

namespace SharpSvn {
	ref class Delta::SvnDeltaEditor;

	public ref class SvnReplayRevisionStartEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
		Delta::SvnDeltaEditor^ _editor;
		apr_hash_t* _revProps;
		AprPool^ _pool;
		SvnPropertyCollection^ _revisionProperties;

	internal:
		SvnReplayRevisionStartEventArgs(__int64 revision, Delta::SvnDeltaEditor^ editor, apr_hash_t* revProps, AprPool^ pool)
		{
			_revision = revision;
			_editor = editor;
			_revProps = revProps;
			_pool = pool;
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}
		property Delta::SvnDeltaEditor^ Editor
		{
			Delta::SvnDeltaEditor^ get()
			{
				return _editor;
			}
			void set(Delta::SvnDeltaEditor^ value)
			{
				_editor = value;
			}
		}

		property SvnPropertyCollection^ RevisionProperties
        {
            SvnPropertyCollection^ get()
            {
                if (!_revisionProperties && _revProps && _pool)
                    _revisionProperties = SvnBase::CreatePropertyDictionary(_revProps, _pool);

                return _revisionProperties;
            }
        }

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(RevisionProperties);
				}
			}
			finally
			{
				_revProps = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnReplayRevisionEndEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
		apr_hash_t* _revProps;
		AprPool^ _pool;
		SvnPropertyCollection^ _revisionProperties;
	internal:
		SvnReplayRevisionEndEventArgs(__int64 revision, apr_hash_t *revProps, AprPool^ pool)
		{
			_revision = revision;
			_revProps = revProps;
			_pool = pool;
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property SvnPropertyCollection^ RevisionProperties
        {
            SvnPropertyCollection^ get()
            {
                if (!_revisionProperties && _revProps && _pool)
                    _revisionProperties = SvnBase::CreatePropertyDictionary(_revProps, _pool);

                return _revisionProperties;
            }
        }

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(RevisionProperties);
				}
			}
			finally
			{
				_revProps = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

}