// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnBlameEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
		initonly __int64 _lineNr;
		const char* _pcAuthor;
		const char* _pcLine;
		initonly DateTime _date;
		String^ _author;
		String^ _line;
		initonly __int64 _mergedRevision;
		const char* _pcMergedAuthor;
		const char* _pcMergedPath;
		initonly DateTime _mergedDate;
		String^ _mergedAuthor;
		String^ _mergedPath;

	internal:
		SvnBlameEventArgs(__int64 revision, __int64 lineNr, const char* author, const char* date,
			svn_revnum_t merged_revision, const char *merged_author, const char *merged_date,
			const char *merged_path, const char* line, AprPool^ pool)
		{
			if (!pool)
				throw gcnew ArgumentNullException("pool");
			else if(!line)
				throw gcnew ArgumentNullException("line");

			_revision = revision;
			_lineNr = lineNr;
			_pcAuthor = author;
			_pcLine = line;

			apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()

			svn_error_t *err;

			if(date)
			{
				err = svn_time_from_cstring(&when, date, pool->Handle); // pool is not used at this time (might be for errors in future versions)

				if (!err)
					_date = SvnBase::DateTimeFromAprTime(when);
				else
					svn_error_clear(err);
			}
			else
				_date = DateTime::MinValue;

			_mergedRevision = merged_revision;
			_pcMergedAuthor = merged_author;
			_pcMergedPath = merged_path;

			if (merged_date)
			{
				err = svn_time_from_cstring(&when, merged_date, pool->Handle);

				if (!err)
					_mergedDate = SvnBase::DateTimeFromAprTime(when);
				else
					svn_error_clear(err);
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

		property __int64 LineNumber
		{
			__int64 get()
			{
				return _lineNr;
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

		property String^ Line
		{
			String^ get()
			{
				if (!_line && _pcLine)
				{
					try
					{
						_line = SvnBase::Utf8_PtrToString(_pcLine);
					}
					catch(ArgumentException^)
					{
						_line = SharpSvnStrings::NonUtf8ConvertableLine;
					}
				}

				return _line;
			}
		}

		property String^ MergedAuthor
		{
			String^ get()
			{
				if (!_mergedAuthor && _pcMergedAuthor)
					_mergedAuthor = SvnBase::Utf8_PtrToString(_pcMergedAuthor);

				return _mergedAuthor;
			}
		}

		property String^ MergedPath
		{
			String^ get()
			{
				if (!_mergedPath && _pcMergedPath)
					_mergedPath = SvnBase::Utf8_PtrToString(_pcMergedPath);

				return _mergedPath;
			}
		}

		property DateTime MergedTime
		{
			DateTime get()
			{
				return _mergedDate;
			}
		}

		property __int64 MergedRevision
		{
			__int64 get()
			{
				return _mergedRevision;
			}
		}

	public:
		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return _revision.GetHashCode();
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Author);
					GC::KeepAlive(Line);
					GC::KeepAlive(MergedAuthor);
					GC::KeepAlive(MergedPath);
				}
			}
			finally
			{
				_pcAuthor = nullptr;
				_pcLine = nullptr;
				_pcMergedAuthor = nullptr;
				_pcMergedPath = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}