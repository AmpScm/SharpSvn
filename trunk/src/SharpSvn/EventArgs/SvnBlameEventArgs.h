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
		initonly bool _localChange;
		initonly __int64 _startRevnum;
		initonly __int64 _endRevnum;

	internal:
		SvnBlameEventArgs(__int64 revision, __int64 lineNr, apr_hash_t *rev_props,
			svn_revnum_t merged_revision, apr_hash_t *merged_rev_props,
			const char *merged_path, const char* line, bool localChange,
			__int64 start_revnum, __int64 end_revnum, AprPool^ pool)
		{
			if (!pool)
				throw gcnew ArgumentNullException("pool");
			else if (!line)
				throw gcnew ArgumentNullException("line");

			const char *date = NULL;
			const char *merged_date = NULL;

			_revision = revision;
			_lineNr = lineNr;
			_pcLine = line;

			if (rev_props != NULL)
			{
				_pcAuthor = svn_prop_get_value(rev_props, SVN_PROP_REVISION_AUTHOR);
				date = svn_prop_get_value(rev_props, SVN_PROP_REVISION_DATE);
			}
			if (merged_rev_props != NULL)
			{
				_pcMergedAuthor = svn_prop_get_value(merged_rev_props, SVN_PROP_REVISION_AUTHOR);
				merged_date = svn_prop_get_value(merged_rev_props, SVN_PROP_REVISION_DATE);
			}

			apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()

			svn_error_t *err;

			if (date)
			{
				err = svn_time_from_cstring(&when, date, pool->Handle); // pool is not used at this time (might be for errors in future versions)

				if (!err)
					_date = SvnBase::DateTimeFromAprTime(when);
				else
				{
					svn_error_clear(err);
					_date = DateTime::MinValue;
				}
			}
			else
				_date = DateTime::MinValue;

			_mergedRevision = merged_revision;
			_pcMergedPath = merged_path;

			if (merged_date)
			{
				err = svn_time_from_cstring(&when, merged_date, pool->Handle);

				if (!err)
					_mergedDate = SvnBase::DateTimeFromAprTime(when);
				else
				{
					svn_error_clear(err);
					_mergedDate = DateTime::MinValue;
				}
			}
			else
				_mergedDate = DateTime::MinValue;

			_localChange = localChange;
			_startRevnum = start_revnum;
			_endRevnum = end_revnum;
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property __int64 StartRevision
		{
			__int64 get()
			{
				return _startRevnum;
			}
		}

		property __int64 EndRevision
		{
			__int64 get()
			{
				return _endRevnum;
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

		property bool LocalChange
		{
			bool get()
			{
				return _localChange;
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