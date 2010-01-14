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

	ref class SvnFileVersionWriteArgs;
	ref class SvnFileVersionsArgs;

	public ref class SvnFileVersionEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
		initonly bool _hasContent;
		initonly bool _hasProperties;
		initonly bool _resultOfMerge;
		String^ _path;
		String^ _versionFile;

		initonly DateTime _date;
		String^ _author;
		String^ _message;
		System::Uri^ _uri;
		System::Uri^ _repositoryRoot;

        SvnPropertyCollection^ _revisionProperties;
		SvnPropertyCollection^ _properties;
		AprPool^ _pool;

		const char* _pReposRoot;
		const char* _pPath;
		const char* _pUrl;
		const char* _pcAuthor;
		const char* _pcMessage;
		apr_hash_t* _revProps;
        apr_hash_t* _fileProps;
		SvnClient^ _client;

	internal:
		SvnFileVersionEventArgs(
			const char *repos_root,
            const char *path,
            svn_revnum_t rev,
            apr_hash_t *rev_props,
            apr_hash_t *file_props,
			bool has_content,
			bool has_properties,
            bool result_of_merge,
			SvnClient^ client,
            AprPool^ pool)
		{
			if (!repos_root)
				throw gcnew ArgumentNullException("repos_root");
			else if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");
			else if (!client)
				throw gcnew ArgumentNullException("client");

			_pool = pool;
			_revision = rev;
			_resultOfMerge = result_of_merge;
			_hasContent = has_content;
			_hasProperties = has_properties;

			_pReposRoot = repos_root;
			_pPath = path;

            _revProps = rev_props;
            _fileProps = file_props;
			_client = client;

			const char* pcAuthor = nullptr;
			const char* pcDate = nullptr;
			const char* pcMessage = nullptr;

			if (rev_props)
				svn_compat_log_revprops_out(&pcAuthor, &pcDate, &pcMessage, rev_props);

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

			_pcAuthor = pcAuthor;
			_pcMessage = pcMessage;
		}

		apr_hash_t* GetKeywords(SvnFileVersionsArgs^ args);

		void SetPool(AprPool^ pool)
		{
			if (!pool)
				throw gcnew ArgumentNullException("pool");

			_pool = pool;
		}

		property const char* ItemUrl
		{
			const char* get()
			{
				if (!_pUrl && _pReposRoot && _pPath && _pool)
					_pUrl = svn_path_url_add_component2(_pReposRoot, _pPath+1, _pool->Handle);

				return _pUrl;
			}
		}

	public:
        property __int64 Revision
        {
            __int64 get()
            {
                return _revision;
            }
        }

		property DateTime Time
		{
			DateTime get()
			{
				return _date;
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

		property System::Uri^ RepositoryRoot
		{
			System::Uri^ get()
			{
				if (!_repositoryRoot && _pReposRoot)
					_repositoryRoot = SvnBase::Utf8_PtrToUri(_pReposRoot, SvnNodeKind::Directory);

				return _repositoryRoot;
			}
		}

		property System::Uri^ Uri
		{
			System::Uri^ get()
			{
				if (!_uri && ItemUrl)
					_uri = SvnBase::Utf8_PtrToUri(ItemUrl, SvnNodeKind::File);

				return _uri;
			}
		}

		property String^ Path
		{
			String^ get()
			{
				if (!_path && _pPath)
					_path = SvnBase::Utf8_PtrToString(_pPath);

				return _path;
			}
		}

		property bool HasContentChanges
		{
			bool get()
			{
				return _hasContent;
			}
		}

		property bool HasPropertyChanges
		{
			bool get()
			{
				return _hasProperties;
			}
		}

		property bool ResultOfMerge
		{
			bool get()
			{
				return _resultOfMerge;
			}
		}

		property String^ VersionFile
		{
			String^ get()
			{
				return _versionFile;
			}
			void set(String^ value)
			{
				if(!_pPath)
					throw gcnew InvalidOperationException("Can't change file path after the event completed");

				_versionFile = value;
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

        property SvnPropertyCollection^ Properties
        {
            SvnPropertyCollection^ get()
            {
                if (!_properties && _fileProps && _pool)
                    _properties = SvnBase::CreatePropertyDictionary(_fileProps, _pool);

                return _properties;
            }
        }

		/// <overloads>Write the file contents to the specified file/stream (Only valid in EventHandler)</overloads>
		/// <summary>Write the file contents to the specified file/stream (Only valid in EventHandler)</summary>
		/// <exception cref="InvalidOperationException">Not in the EventHandler</exception>
		void WriteTo(String^ outputFileName);
		/// <summary>Write the file contents to the specified file/stream (Only valid in EventHandler)</summary>
		/// <exception cref="InvalidOperationException">Not in the EventHandler</exception>
		void WriteTo(System::IO::Stream^ output);
		/// <summary>Write the file contents to the specified file/stream (Only valid in EventHandler)</summary>
		/// <exception cref="InvalidOperationException">Not in the EventHandler</exception>
		void WriteTo(String^ outputFileName, SvnFileVersionWriteArgs^ args);
		/// <summary>Write the file contents to the specified file/stream (Only valid in EventHandler)</summary>
		/// <exception cref="InvalidOperationException">Not in the EventHandler</exception>
		void WriteTo(System::IO::Stream^ output, SvnFileVersionWriteArgs^ args);

		/// <summary>Gets the file contents of the current version as readable stream (Only valid in EventHandler)</summary>
		/// <exception cref="InvalidOperationException">Not in the EventHandler</exception>
		/// <remarks>The stream will be closed when the eventhandler returns. It is not seekabel but you can reset the stream to position 0</remarks>
		System::IO::Stream^ GetContentStream();

		/// <summary>Gets the file contents of the current version as readable stream (Only valid in EventHandler)</summary>
		/// <exception cref="InvalidOperationException">Not in the EventHandler</exception>
		/// <remarks>The stream will be closed when the eventhandler returns. It is not seekabel but you can reset the stream to position 0</remarks>
		System::IO::Stream^ GetContentStream(SvnFileVersionWriteArgs^ args);

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Author);
					GC::KeepAlive(LogMessage);
					GC::KeepAlive(Path);
					GC::KeepAlive(Uri);
					GC::KeepAlive(RepositoryRoot);
                    GC::KeepAlive(RevisionProperties);
                    GC::KeepAlive(Properties);
				}
			}
			finally
			{
				_pool = nullptr;
				_pReposRoot = nullptr;
				_pPath = nullptr;
				_pUrl = nullptr;
				_pcAuthor = nullptr;
				_pcMessage = nullptr;
				_revProps = nullptr;
                _fileProps = nullptr;

				__super::Detach(keepProperties);
			}
		}

	internal:
		void DetachBeforeFileData(AprPool^ toPool)
		{
			_pool = toPool;

			apr_pool_t* h = toPool->Handle;

			if (_pReposRoot)
				_pReposRoot = apr_pstrdup(h, _pReposRoot);
			if (_pPath)
				_pPath = apr_pstrdup(h, _pPath);
			if (_pUrl)
				_pUrl = apr_pstrdup(h, _pUrl);
			if (_pcAuthor)
				_pcAuthor = apr_pstrdup(h, _pcAuthor);
			if (_pcMessage)
				_pcMessage = apr_pstrdup(h, _pcMessage);

			// Keep revProps
			// Keep fileProps
		}
	};
}