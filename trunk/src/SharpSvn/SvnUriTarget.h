// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnTarget.h"

namespace SharpSvn {
	using namespace System;

	public ref class SvnUriTarget sealed : public SvnTarget
	{
		initonly Uri^ _uri;

	public:
		SvnUriTarget (Uri^ uri, SvnRevision^ revision)
			: SvnTarget(revision)
		{
			if (!uri)
				throw gcnew ArgumentNullException("uri");
			else if (!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");
			else if (!SvnBase::IsValidReposUri(uri))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

			_uri = CanonicalizeUri(uri);
		}

		SvnUriTarget (Uri^ uri)
			: SvnTarget(SvnRevision::None)
		{
			if (!uri)
				throw gcnew ArgumentNullException("uri");
			else if (!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");
			else if (!SvnBase::IsValidReposUri(uri))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

			_uri = CanonicalizeUri(uri);
		}

		SvnUriTarget (String^ uriString, SvnRevision^ revision)
			: SvnTarget(revision)
		{
			if (String::IsNullOrEmpty(uriString))
				throw gcnew ArgumentNullException("uri");

			System::Uri^ uri = gcnew System::Uri(uriString);

			if (!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uriString");
			else if (!SvnBase::IsValidReposUri(uri))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uriString");

			_uri = CanonicalizeUri(uri);
		}

		SvnUriTarget (String^ uriString)
			: SvnTarget(SvnRevision::None)
		{
			if (String::IsNullOrEmpty(uriString))
				throw gcnew ArgumentNullException("uri");

			System::Uri^ uri = gcnew System::Uri(uriString);

			if (!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uriString");
			else if (!SvnBase::IsValidReposUri(uri))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uriString");

			_uri = CanonicalizeUri(uri);
		}


		SvnUriTarget (Uri^ uri, __int64 revision)
			: SvnTarget(gcnew SvnRevision(revision))
		{
			if (!uri)
				throw gcnew ArgumentNullException("uri");
			else if (!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");
			else if (!SvnBase::IsValidReposUri(uri))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

			_uri = CanonicalizeUri(uri);
		}

		SvnUriTarget (Uri^ uri, DateTime date)
			: SvnTarget(gcnew SvnRevision(date))
		{
			if (!uri)
				throw gcnew ArgumentNullException("uri");
			else if (!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");
			else if (!SvnBase::IsValidReposUri(uri))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

			_uri = CanonicalizeUri(uri);
		}

		static SvnUriTarget^ FromUri(Uri^ value)
		{
			if (!value)
				throw gcnew ArgumentNullException("value");

			return gcnew SvnUriTarget(value);
		}

		static SvnUriTarget^ FromString(String^ value);
		static SvnUriTarget^ FromString(String^ value, bool allowPegRevision);

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
				return SvnBase::UriToString(_uri);
			}
		}

		property String^ FileName
		{
			virtual String^ get() override
			{
				return SvnTools::GetFileName(_uri);
			}
		}

	internal:
		property String^ SvnTargetName
		{
			virtual String^ get() override
			{
				return UriToCanonicalString(_uri);
			}
		}

	public:

		static bool TryParse(String^ path, [Out] SvnUriTarget^% pegUri)
		{
			return TryParse(path, false, pegUri);
		}

		static bool TryParse(String^ path, bool allowPegRevision, [Out] SvnUriTarget^% pegUri)
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			AprPool ^pool = gcnew AprPool();
			try
			{
				return TryParse(path, allowPegRevision, pegUri, pool);
			}
			finally
			{
				delete pool;
			}
		}

		static ICollection<SvnUriTarget^>^ Map(System::Collections::Generic::IEnumerable<System::Uri^>^ uris);

	internal:
		static bool TryParse(String^ targetString, bool allowPegRevision, [Out] SvnUriTarget ^% target, AprPool^ pool);

		virtual SvnRevision^ GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue) override;
	};
}
