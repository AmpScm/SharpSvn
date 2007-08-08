
#pragma once

#include "SvnTarget.h"

namespace SharpSvn {
	using namespace System;

	public ref class SvnPathTarget sealed : public SvnTarget
	{
		String^ _path;
		String^ _fullPath;

		[SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")] // Optimized away by MC++ compiler
		static String^ GetFullPath(String ^path);

	public:
		SvnPathTarget(String^ path, SvnRevision^ revision)
			: SvnTarget(revision)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = CanonicalizePath(path);
			_fullPath = GetFullPath(_path);
		}

		SvnPathTarget(String^ path)
			: SvnTarget(SvnRevision::None)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = CanonicalizePath(path);
			_fullPath = GetFullPath(_path);
		}

		SvnPathTarget(String^ path, __int64 revision)
			: SvnTarget(gcnew SvnRevision(revision))
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = CanonicalizePath(path);
			_fullPath = GetFullPath(_path);
		}

		SvnPathTarget(String^ path, DateTime date)
			: SvnTarget(gcnew SvnRevision(date))
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = CanonicalizePath(path);
			_fullPath = GetFullPath(_path);
		}

		SvnPathTarget(String^ path, SvnRevisionType type)
			: SvnTarget(gcnew SvnRevision(type))
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = CanonicalizePath(path);
			_fullPath = GetFullPath(_path);
		}

		property String^ TargetName
		{
			virtual String^ get() override
			{
				return _fullPath->Replace(System::IO::Path::DirectorySeparatorChar, '/');
			}
		}

		property String^ Path
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

	internal:
		static bool TryParse(String^ targetName, [Out] SvnPathTarget ^% target, AprPool^ pool)
		{
			if(String::IsNullOrEmpty(targetName))
				throw gcnew ArgumentNullException("targetName");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			svn_opt_revision_t rev;
			const char* truePath;

			const char* path = pool->AllocPath(targetName);

			if(!svn_opt_parse_path(&rev, &truePath, path, pool->Handle))
			{

				String^ realPath = Utf8_PtrToString(truePath);

				if(!realPath->Contains("://"))
				{
					SvnRevision^ pegRev = SvnRevision::Load(&rev);

					target = gcnew SvnPathTarget(realPath, pegRev);
					return true;
				}

			}

			target = nullptr;
			return false;
		}

	public:
		static SvnPathTarget^ FromString(String^ value);
		static operator SvnPathTarget^(String^ value) { return value ? FromString(value) : nullptr; }

	internal:
		virtual svn_opt_revision_t GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue) override;
	};
}
