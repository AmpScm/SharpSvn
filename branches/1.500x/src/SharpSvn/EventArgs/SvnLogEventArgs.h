// $Id$
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

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnChangeItem sealed
	{
		initonly String^ _path;
		initonly SvnChangeAction _action;
		initonly String^ _copyFromPath;
		initonly __int64 _copyFromRevision;
	internal:
		SvnChangeItem(String^ path, SvnChangeAction action, String^ copyFromPath, __int64 copyFromRevision)
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_action = action;
			_copyFromPath = copyFromPath;
			_copyFromRevision = copyFromPath ? copyFromRevision : -1;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property SvnChangeAction Action
		{
			SvnChangeAction get()
			{
				return _action;
			}
		}

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
				return _copyFromRevision;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return CopyFromRevision.GetHashCode() ^ Path->GetHashCode();
		}
	};

	namespace Implementation {
		public ref class SvnChangeItemCollection sealed : System::Collections::ObjectModel::KeyedCollection<String^, SvnChangeItem^>
		{
		internal:
			SvnChangeItemCollection()
			{}
		protected:
			virtual String^ GetKeyForItem(SvnChangeItem^ item) override
			{
				if (!item)
					throw gcnew ArgumentNullException("item");

				return item->Path;
			}
		};
	}

	public ref class SvnLoggingEventArgs abstract : public SvnCancelEventArgs
	{
		svn_log_entry_t* _entry;
		AprPool^ _pool;

		String^ _author;
		initonly DateTime _date;
		String^ _message;
		initonly __int64 _revision;

		const char* _pcAuthor;
		const char* _pcMessage;
		SvnPropertyCollection^ _customProperties;

	internal:
		SvnLoggingEventArgs(svn_log_entry_t *entry, AprPool ^pool)
		{
			if (!entry)
				throw gcnew ArgumentNullException("entry");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_entry = entry;
			_pool = pool;

			const char* pcAuthor = nullptr;
			const char* pcDate = nullptr;
			const char* pcMessage = nullptr;

			if (entry->revprops)
				svn_compat_log_revprops_out(&pcAuthor, &pcDate, &pcMessage, entry->revprops);

			if (pcDate)
			{
				apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()
				svn_error_t *err = pcDate ? svn_time_from_cstring(&when, pcDate, pool->Handle) : nullptr;

				if (!err)
					_date = SvnBase::DateTimeFromAprTime(when);
				else
					svn_error_clear(err);
			}
			else
				_date = DateTime::MinValue;

			_revision = entry->revision;
			_pcAuthor = pcAuthor;
			_pcMessage = pcMessage;
		}

	private:
		SvnChangeItemCollection^ _changedPaths;
	public:

		property SvnChangeItemCollection^ ChangedPaths
		{
			SvnChangeItemCollection^  get()
			{
				if (!_changedPaths && _entry && _entry->changed_paths && _pool)
				{
					_changedPaths = gcnew SvnChangeItemCollection();

					for (apr_hash_index_t* hi = apr_hash_first(_pool->Handle, _entry->changed_paths); hi; hi = apr_hash_next(hi))
					{
						const char* pKey;
						apr_ssize_t keyLen;
						svn_log_changed_path_t *pChangeInfo;

						apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&pChangeInfo);

						SvnChangeItem^ ci = gcnew SvnChangeItem(SvnBase::Utf8_PtrToString(pKey, (int)keyLen),
							(SvnChangeAction)pChangeInfo->action,
							SvnBase::Utf8_PtrToString(pChangeInfo->copyfrom_path),
							pChangeInfo->copyfrom_rev);

						_changedPaths->Add(ci);
					}
				}

				return _changedPaths;
			}
		}

		/// <summary>Gets the list of custom properties retrieved with the log</summary>
		/// <remarks>Properties must be listed in SvnLogArgs.RetrieveProperties to be available here</remarks>
		property SvnPropertyCollection^ RevisionProperties
		{
			SvnPropertyCollection^ get()
			{
				if (!_customProperties && _entry && _entry->revprops && _pool)
				{
					_customProperties = SvnBase::CreatePropertyDictionary(_entry->revprops, _pool);
				}
				return _customProperties;
			}
		}

		[Obsolete("Use .RevisionProperties")]
		property SvnPropertyCollection^ CustomProperties
		{
			SvnPropertyCollection^ get()
			{
				return RevisionProperties;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property String^ Author
		{
			String^ get()
			{
				if (!_author && _pcAuthor)
					_author = SvnBase::Utf8_PtrToString(_pcAuthor);

				return _author;
			}
		}

		property DateTime Time
		{
			DateTime get()
			{

				return _date;
			}
		}

		property String^ LogMessage
		{
			String^ get()
			{
				if (!_message && _pcMessage)
				{
					_message = SvnBase::Utf8_PtrToString(_pcMessage);

					if (_message)
					{
						// Subversion log messages always use \n newlines
						_message = _message->Replace("\n", Environment::NewLine);
					}
				}

				return _message;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Revision.GetHashCode();
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(ChangedPaths);
					GC::KeepAlive(Author);
					GC::KeepAlive(LogMessage);
					GC::KeepAlive(RevisionProperties);
				}
			}
			finally
			{
				_entry = nullptr;
				_pool = nullptr;
				_pcMessage = nullptr;
				_pcAuthor = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};


	public ref class SvnLogEventArgs : public SvnLoggingEventArgs
	{
		initonly Uri^ _logOrigin;
		initonly bool _hasChildren;
		initonly int _mergeLevel;

	internal:
		SvnLogEventArgs(svn_log_entry_t *entry, int mergeLevel, Uri^ logOrigin, AprPool ^pool)
			: SvnLoggingEventArgs(entry, pool)
		{
			_hasChildren = entry->has_children ? true : false;
			_mergeLevel = mergeLevel;
			_logOrigin = logOrigin;
		}

	public:
				/// <summary>Gets the log origin SharpSvn used for retrieving the logfile</summary>
		property Uri^ LogOrigin
		{
			Uri^ get()
			{
				return _logOrigin;
			}
		}

		/// <summary>Set to true when the following items are merged-child items of this item.</summary>
		property bool HasChildren
		{
			bool get()
			{
				return _hasChildren;
			}
		}

		/// <summary>Gets the nesting level of the logs via merges</summary>
		property int MergeLogNestingLevel
		{
			int get()
			{
				return _mergeLevel;
			}
		}

	};

}