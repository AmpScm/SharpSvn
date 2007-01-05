
#pragma once

#include "SvnTarget.h"

namespace TurtleSvn {
	using namespace System;

	public ref class SvnPathTarget sealed : public SvnTarget
	{
		String^ _path;
		String^ _fullPath;

	public:
		SvnPathTarget(String^ path, SvnRevision^ revision)
			: SvnTarget(revision)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_fullPath = System::IO::Path::GetFullPath(path);
		}

		SvnPathTarget(String^ path)
			: SvnTarget(SvnRevision::None)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_fullPath = System::IO::Path::GetFullPath(path);
		}

		SvnPathTarget(String^ path, long revision)
			: SvnTarget(gcnew SvnRevision(revision))
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_fullPath = System::IO::Path::GetFullPath(path);
		}

		SvnPathTarget(String^ path, DateTime date)
			: SvnTarget(gcnew SvnRevision(date))
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_fullPath = System::IO::Path::GetFullPath(path);
		}

		SvnPathTarget(String^ path, SvnRevisionType type)
			: SvnTarget(gcnew SvnRevision(type))
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_fullPath = System::IO::Path::GetFullPath(path);
		}

		property String^ TargetName
		{
			virtual String^ get() override
			{
				return _fullPath;
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
		static bool TryParse(String^ targetString, [Out] SvnPathTarget^% target)
		{
			if(String::IsNullOrEmpty(targetString))
				throw gcnew ArgumentNullException("targetString");

			AprPool ^pool = gcnew AprPool();
			try
			{
				return TryParse(targetString, target, pool);
			}
			finally
			{
				delete pool;
			}
		}

		static bool TryParse(String^ targetString, [Out] SvnPathTarget ^% target, AprPool^ pool)
		{
			if(String::IsNullOrEmpty(targetString))
				throw gcnew ArgumentNullException("targetString");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			svn_opt_revision_t rev;
			const char* truePath;

			const char* path = Utf8_StringToPtr(targetString, pool);

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

		static SvnPathTarget^ FromString(String^ value);
		static explicit operator SvnPathTarget^(String^ value) { return value ? FromString(value) : nullptr; }
	};
}
