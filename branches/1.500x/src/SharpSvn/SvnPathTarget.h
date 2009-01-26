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

#include "SvnTarget.h"

namespace SharpSvn {
	using namespace System;

	public ref class SvnPathTarget sealed : public SvnTarget
	{
		initonly String^ _path;
		initonly String^ _fullPath;

	private:
		static String^ GetFullTarget(String^ path);
	public:
		static String^ GetTargetPath(String^ path);

	public:
		SvnPathTarget(String^ path, SvnRevision^ revision)
			: SvnTarget(revision)
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if (!SvnBase::IsNotUri(path))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

			_path = GetTargetPath(path);
			_fullPath = GetFullTarget(_path);
		}

		SvnPathTarget(String^ path)
			: SvnTarget(SvnRevision::None)
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if (!SvnBase::IsNotUri(path))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

			_path = GetTargetPath(path);
			_fullPath = GetFullTarget(_path);
		}

		SvnPathTarget(String^ path, __int64 revision)
			: SvnTarget(gcnew SvnRevision(revision))
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if (!SvnBase::IsNotUri(path))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

			_path = GetTargetPath(path);
			_fullPath = GetFullTarget(_path);
		}

		SvnPathTarget(String^ path, DateTime date)
			: SvnTarget(gcnew SvnRevision(date))
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if (!SvnBase::IsNotUri(path))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

			_path = GetTargetPath(path);
			_fullPath = GetFullTarget(_path);
		}

		SvnPathTarget(String^ path, SvnRevisionType type)
			: SvnTarget(gcnew SvnRevision(type))
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if (!SvnBase::IsNotUri(path))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

			_path = GetTargetPath(path);
			_fullPath = GetFullTarget(_path);
		}

		property String^ TargetName
		{
			virtual String^ get() override
			{
				return _path;
			}
		}

		property String^ FileName
		{
			virtual String^ get() override
			{
				return System::IO::Path::GetFileName(_path);
			}
		}
	internal:
		property String^ SvnTargetName
		{
			virtual String^ get() override
			{
				return _path->Replace(System::IO::Path::DirectorySeparatorChar, '/')->TrimEnd('/');
			}
		}

	public:
		property String^ TargetPath
		{
			String^ get()
			{
				return _path;
			}
		}

		property String^ FullPath
		{
			String^ get()
			{
				return _fullPath;
			}
		}

	public:
		static bool TryParse(String^ targetName, [Out] SvnPathTarget^% target);
		static bool TryParse(String^ targetName, bool allowOperationalRevisions, [Out] SvnPathTarget^% target);

	public:
		static ICollection<SvnPathTarget^>^ Map(System::Collections::Generic::IEnumerable<String^>^ paths);

	internal:
		static bool TryParse(String^ targetName, bool allowOperationalRevisions, [Out] SvnPathTarget ^% target, AprPool^ pool)
		{
			if (String::IsNullOrEmpty(targetName))
				throw gcnew ArgumentNullException("targetName");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			if (!SvnBase::IsNotUri(targetName))
				return false;

			if (allowOperationalRevisions)
			{
				svn_opt_revision_t rev;
				svn_error_t* r;
				const char* truePath;

				const char* path = pool->AllocPath(targetName);

				if (!(r = svn_opt_parse_path(&rev, &truePath, path, pool->Handle)))
				{
					String^ realPath = Utf8_PtrToString(truePath);

					if (!realPath->Contains("://"))
					{
						SvnRevision^ pegRev = SvnRevision::Load(&rev);

						target = gcnew SvnPathTarget(realPath, pegRev);
						return true;
					}
				}
				else
					svn_error_clear(r);
			}
			else
			{
				target = gcnew SvnPathTarget(targetName);
				return true;
			}

			target = nullptr;
			return false;
		}

	public:
		static SvnPathTarget^ FromString(String^ value);
		static SvnPathTarget^ FromString(String^ value, bool allowOperationalRevision);

		static operator SvnPathTarget^(String^ value) { return value ? gcnew SvnPathTarget(value) : nullptr; }

	internal:
		virtual SvnRevision^ GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue) override;
	};
}
