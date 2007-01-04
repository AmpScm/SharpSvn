
#pragma once

#include "SvnTarget.h"

namespace QQn {
	namespace Svn {
		using namespace System;

		public ref class SvnUriTarget sealed : public SvnTarget
		{
			initonly Uri^ _uri;


		public:
			SvnUriTarget (Uri^ uri, SvnRevision^ revision)
				: SvnTarget(revision)
			{
				if(!uri)
					throw gcnew ArgumentNullException("uri");
				else if(!uri->IsAbsoluteUri)
					throw gcnew ArgumentException("Uri is not absolute", "uri");

				_uri = uri;
			}

			SvnUriTarget (Uri^ uri)
				: SvnTarget(SvnRevision::None)
			{
				if(!uri)
					throw gcnew ArgumentNullException("uri");
				else if(!uri->IsAbsoluteUri)
					throw gcnew ArgumentException("Uri is not absolute", "uri");

				_uri = uri;
			}

			SvnUriTarget (Uri^ uri, long revision)
				: SvnTarget(gcnew SvnRevision(revision))
			{
				if(!uri)
					throw gcnew ArgumentNullException("uri");
				else if(!uri->IsAbsoluteUri)
					throw gcnew ArgumentException("Uri is not absolute", "uri");

				_uri = uri;
			}

			SvnUriTarget (Uri^ uri, DateTime date)
				: SvnTarget(gcnew SvnRevision(date))
			{
				if(!uri)
					throw gcnew ArgumentNullException("uri");
				else if(!uri->IsAbsoluteUri)
					throw gcnew ArgumentException("Uri is not absolute", "uri");

				_uri = uri;
			}

			SvnUriTarget (Uri^ uri, SvnRevisionType type)
				: SvnTarget(gcnew SvnRevision(type))
			{
				if(!uri)
					throw gcnew ArgumentNullException("uri");
				else if(!uri->IsAbsoluteUri)
					throw gcnew ArgumentException("Uri is not absolute", "uri");

				_uri = uri;
			}

			static SvnUriTarget^ FromUri(Uri^ value)
			{
				if(!value)
					throw gcnew ArgumentNullException("value");

				return gcnew SvnUriTarget(value);
			}
			
			static SvnUriTarget^ FromString(String^ value);

			static operator SvnUriTarget^(Uri^ value)				{ return value ? FromUri(value) : nullptr; }
			static explicit operator SvnUriTarget^(String^ value)	{ return value ? FromString(value) : nullptr; }

			property Uri^ Uri
			{
				System::Uri^ get()
				{
					return _uri;
				}
			}

			property String^ TargetName
			{
				virtual String^ get() override
				{
					return _uri->ToString();
				}
			}

			static bool TryParse(String^ path, [Out] SvnUriTarget^% pegUri)
			{
				if(String::IsNullOrEmpty(path))
					throw gcnew ArgumentNullException("path");

				AprPool ^pool = gcnew AprPool();
				try
				{
					return TryParse(path, pegUri, pool);
				}
				finally
				{
					delete pool;
				}
			}

			static bool TryParse(String^ targetString, [Out] SvnUriTarget ^% target, AprPool^ pool)
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
					System::Uri^ uri = nullptr;

					if(System::Uri::TryCreate(Utf8_PtrToString(truePath), UriKind::Absolute, uri))
					{
						SvnRevision^ pegRev = SvnRevision::Load(&rev);

						target = gcnew SvnUriTarget (uri, pegRev);
						return true;
					}
				}

				target = nullptr;
				return false;
			}
		};
	}
}