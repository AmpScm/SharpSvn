// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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
		static bool TryParse(String^ targetName, bool allowPegRevisions, [Out] SvnPathTarget^% target);

	internal:
		static bool TryParse(String^ targetName, bool allowPegRevisions, [Out] SvnPathTarget ^% target, AprPool^ pool)
		{
			if (String::IsNullOrEmpty(targetName))
				throw gcnew ArgumentNullException("targetName");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			if (!SvnBase::IsNotUri(targetName))
				return false;

			if (allowPegRevisions)
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
		static SvnPathTarget^ FromString(String^ value, bool allowPegRevision);

		static operator SvnPathTarget^(String^ value) { return value ? gcnew SvnPathTarget(value) : nullptr; }

	internal:
		virtual SvnRevision^ GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue) override;
	};
}
