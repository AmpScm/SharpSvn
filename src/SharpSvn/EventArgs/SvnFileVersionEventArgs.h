// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {


	public ref class SvnFileVersionEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
		initonly bool _resultOfMerge;
		String^ _path;

		initonly DateTime _date;
		String^ _message;

		SvnPropertyCollection^ _properties;
		AprPool^ _pool;

		String^ _author;
		String^ _versionFile;

		const char* _pPath;
		const char* _pcAuthor;
		const char* _pcMessage;
		apr_hash_t* _revProps;

	internal:
		SvnFileVersionEventArgs(String^ versionFile, const char *path, svn_revnum_t rev, apr_hash_t *rev_props, bool result_of_merge, AprPool^ pool)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_pool = pool;
			_revision = rev;
			_resultOfMerge = result_of_merge;

			_versionFile = versionFile;
			_pPath = path;

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

	public:
		property String^ Path
		{
			String^ get()
			{
				if (!_path && _pPath)
					_path = SvnBase::Utf8_PtrToString(_pPath);

				return _path;
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

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
				}

			}
			finally
			{
				_pool = nullptr;
				_pPath = nullptr;
				_pcAuthor = nullptr;
				_pcMessage = nullptr;
				_revProps = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};
}