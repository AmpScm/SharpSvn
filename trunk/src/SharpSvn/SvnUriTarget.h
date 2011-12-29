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
				throw gcnew ArgumentNullException("uriString");

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
				throw gcnew ArgumentNullException("uriString");

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
		static SvnUriTarget^ FromString(String^ value, bool allowOperationalRevision);

		static operator SvnUriTarget^(Uri^ value)				{ return value ? FromUri(value) : nullptr; }
		static explicit operator SvnUriTarget^(String^ value)	{ return value ? FromString(value) : nullptr; }
		static operator SvnUriTarget^(ISvnOrigin^ origin)		{ return origin ? gcnew SvnUriTarget(origin->Uri, origin->Target->Revision) : nullptr; }

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
		virtual const char *AllocAsString(AprPool^ pool, bool absolute) override
		{
			UNUSED_ALWAYS(absolute);
			return pool->AllocUri(Uri);
		}

	public:

		static bool TryParse(String^ path, [Out] SvnUriTarget^% pegUri)
		{
			return TryParse(path, false, pegUri);
		}

		static bool TryParse(String^ path, bool allowOperationalRevision, [Out] SvnUriTarget^% pegUri)
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			AprPool ^pool = gcnew AprPool();
			try
			{
				return TryParse(path, allowOperationalRevision, pegUri, pool);
			}
			finally
			{
				delete pool;
			}
		}

		static ICollection<SvnUriTarget^>^ Map(System::Collections::Generic::IEnumerable<System::Uri^>^ uris);

	internal:
		void VerifyBelowRoot(System::Uri^ repositoryRoot)
		{
			// TODO: Throw exception if the current value is not below the repository root
			UNUSED_ALWAYS(repositoryRoot);
		}

	internal:
		static bool TryParse(String^ targetString, bool allowOperationalRevision, [Out] SvnUriTarget ^% target, AprPool^ pool);

		virtual SvnRevision^ GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue) override;
	};

	public ref class SvnUriOrigin sealed : ISvnOrigin
	{
		initonly SvnUriTarget^ _target;
		initonly System::Uri^ _repositoryRoot;
		initonly SvnNodeKind _nodeKind;

	public:
		SvnUriOrigin(SvnUriTarget^ target, Uri^ repositoryRoot)
		{
			if (!target)
				throw gcnew ArgumentNullException("target");
			else if (!repositoryRoot)
				throw gcnew ArgumentNullException("repositoryRoot");

			_target->VerifyBelowRoot(repositoryRoot);

			_target = target;
			_repositoryRoot = repositoryRoot;
			_nodeKind = SvnNodeKind::Unknown;
		}

		SvnUriOrigin(SvnUriTarget^ target, Uri^ repositoryRoot, SvnNodeKind nodeKind)
		{
			if (!target)
				throw gcnew ArgumentNullException("target");
			else if (!repositoryRoot)
				throw gcnew ArgumentNullException("repositoryRoot");

			_target->VerifyBelowRoot(repositoryRoot);

			_target = target;
			_repositoryRoot = repositoryRoot;
			_nodeKind = nodeKind;
		}

	public:
		property SvnUriTarget^ Target
		{
			SvnUriTarget^ get()
			{
				return _target;
			}
		}

		property System::Uri^ Uri
		{
			virtual System::Uri^ get() sealed
			{
				return _target->Uri;
			}
		}

		property System::Uri^ RepositoryRoot
		{
			virtual System::Uri^ get() sealed
			{
				return _repositoryRoot;
			}
		}

		property SvnNodeKind NodeKind
		{
			virtual SvnNodeKind get() sealed
			{
				return _nodeKind;
			}
		}

	private:
		property SvnTarget^ RawTarget
		{
			virtual SvnTarget^ get() sealed = ISvnOrigin::Target::get
			{
				return _target;
			}
		}
	};
}
