// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnTarget.op_Implicit(System.String):SharpSvn.SvnTarget")];

using namespace SharpSvn;
using System::IO::Path;

SvnRevision^ SvnRevision::Load(svn_opt_revision_t *revData)
{
	if (!revData)
		throw gcnew ArgumentNullException("revData");

	SvnRevisionType type = (SvnRevisionType)revData->kind;


	switch(type)
	{
	case SvnRevisionType::None:
		return None;
	case SvnRevisionType::Committed:
		return Committed;
	case SvnRevisionType::Previous:
		return Previous;
	case SvnRevisionType::Base:
		return Base;
	case SvnRevisionType::Working:
		return Working;
	case SvnRevisionType::Head:
		return Head;
	case SvnRevisionType::Number:
		return gcnew SvnRevision(revData->value.number);
	case SvnRevisionType::Time:
		// apr_time_t is in microseconds since 1-1-1970 UTC; filetime is in 100 nanoseconds
		return gcnew SvnRevision(SvnBase::DateTimeFromAprTime(revData->value.date));
	default:
		throw gcnew ArgumentException(SharpSvnStrings::InvalidSvnRevisionTypeValue, "revData");
	}
}

svn_opt_revision_t SvnRevision::ToSvnRevision()
{
	svn_opt_revision_t r;
	memset(&r, 0, sizeof(r));
	r.kind = (svn_opt_revision_kind)_type; // Values are identical by design


	switch(_type)
	{
	case SvnRevisionType::Number:
		r.value.number = (svn_revnum_t)_value;
		break;
	case SvnRevisionType::Time:
		{
			r.value.date = SvnBase::AprTimeFromDateTime(DateTime::FromBinary(_value));
		}
		break;
	}
	return r;
}

svn_opt_revision_t* SvnRevision::AllocSvnRevision(AprPool ^pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	svn_opt_revision_t *rev = (svn_opt_revision_t *)pool->Alloc(sizeof(svn_opt_revision_t));

	*rev = ToSvnRevision();

	return rev;
}

bool SvnTarget::TryParse(String^ targetName, [Out] SvnTarget^% target)
{
	return TryParse(targetName, false, target);
}

bool SvnTarget::TryParse(String^ targetName, bool allowPegRevision, [Out] SvnTarget^% target)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");

	SvnUriTarget^ uriTarget = nullptr;
	SvnPathTarget^ pathTarget = nullptr;

	if (targetName->Contains("://") && SvnUriTarget::TryParse(targetName, allowPegRevision, uriTarget))
	{
		target = uriTarget;
		return true;
	}
	else if(SvnPathTarget::TryParse(targetName, allowPegRevision, pathTarget))
	{
		target = pathTarget;
		return true;
	}

	target = nullptr;
	return false;
}

SvnTarget^ SvnTarget::FromUri(Uri^ value)
{
	if (!value)
		throw gcnew ArgumentNullException("value");

	return gcnew SvnUriTarget(value);
}

SvnRevision^ SvnPathTarget::GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue)
{
	UNUSED_ALWAYS(uriNoneValue);
	return Revision->Or(fileNoneValue);
}

SvnRevision^ SvnUriTarget::GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue)
{
	UNUSED_ALWAYS(fileNoneValue);
	return Revision->Or(uriNoneValue);
}

String^ SvnPathTarget::GetFullTarget(String ^path)
{
	return SvnTools::GetNormalizedFullPath(path);
}

String^ SvnPathTarget::GetTargetPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	path = path->Replace(Path::AltDirectorySeparatorChar, Path::DirectorySeparatorChar);

	String^ dualSeparator = String::Concat(Path::DirectorySeparatorChar, Path::DirectorySeparatorChar);
	String^ singleSeparator = Path::DirectorySeparatorChar.ToString();

	int nRoot = 0;
	if (System::IO::Path::IsPathRooted(path))
		nRoot = System::IO::Path::GetPathRoot(path)->Length;

	int nNext;
	// Remove double backslash
	while ((nNext = path->IndexOf(dualSeparator, nRoot, StringComparison::Ordinal)) >= 0)
		path = path->Remove(nNext, 1);

	// Remove '\.\'
	while ((nNext = path->IndexOf("\\.\\", nRoot, StringComparison::Ordinal)) >= 0)
		path = path->Remove(nNext, 2);

	while (path->StartsWith(".\\", StringComparison::Ordinal))
		path = path->Substring(2);

	if (path->EndsWith("\\.", StringComparison::Ordinal))
		path = path->Substring(0, path->Length-2);

	if (path->Length > nRoot && path->EndsWith(singleSeparator, StringComparison::Ordinal))
	{
		path = path->TrimEnd(Path::DirectorySeparatorChar);
		if(path->Length < nRoot)
			path += singleSeparator;
	}	

	if(path->Length == 0)
		path = ".";

	return path;
}

bool SvnPathTarget::TryParse(String^ targetName, [Out] SvnPathTarget^% target)
{
	return TryParse(targetName, false, target);
}

bool SvnPathTarget::TryParse(String^ targetName, bool allowPegRevision, [Out] SvnPathTarget^% target)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");

	AprPool pool(SvnBase::SmallThreadPool);

	return TryParse(targetName, allowPegRevision, target, %pool);
}

SvnTarget^ SvnTarget::FromString(String^ value)
{
	return FromString(value, false);
}

SvnTarget^ SvnTarget::FromString(String^ value, bool allowPegRevision)
{
	if (!value)
		throw gcnew ArgumentNullException("value");

	SvnTarget^ result;

	if (SvnTarget::TryParse(value, allowPegRevision, result))
	{
		return result;
	}
	else
		throw gcnew System::ArgumentException(SharpSvnStrings::TheTargetIsNotAValidUriOrPathTarget, "value");
}

SvnPathTarget^ SvnPathTarget::FromString(String^ value)
{
	return FromString(value, false);
}

SvnPathTarget^ SvnPathTarget::FromString(String^ value, bool allowPegRevision)
{
	if (!value)
		throw gcnew ArgumentNullException("value");

	SvnPathTarget^ result;

	if (SvnPathTarget::TryParse(value, allowPegRevision, result))
	{
		return result;
	}
	else
		throw gcnew System::ArgumentException(SharpSvnStrings::TheTargetIsNotAValidPathTarget, "value");
}

SvnUriTarget^ SvnUriTarget::FromString(String^ value)
{
	return FromString(value, false);
}

SvnUriTarget^ SvnUriTarget::FromString(String^ value, bool allowPegRevision)
{
	if (!value)
		throw gcnew ArgumentNullException("value");

	SvnUriTarget^ result;

	if (SvnUriTarget::TryParse(value, allowPegRevision, result))
	{
		return result;
	}
	else
		throw gcnew System::ArgumentException(SharpSvnStrings::TheTargetIsNotAValidUriTarget, "value");
}

bool SvnUriTarget::TryParse(String^ targetString, bool allowPegRevision, [Out] SvnUriTarget ^% target, AprPool^ pool)
{
	if (String::IsNullOrEmpty(targetString))
		throw gcnew ArgumentNullException("targetString");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	if(allowPegRevision)
	{
		svn_opt_revision_t rev;
		svn_error_t* r;
		const char* truePath;

		const char* path = pool->AllocPath(targetString);

		if (!(r = svn_opt_parse_path(&rev, &truePath, path, pool->Handle)))
		{
			System::Uri^ uri = nullptr;

			if (System::Uri::TryCreate(Utf8_PtrToString(truePath), UriKind::Absolute, uri))
			{
				SvnRevision^ pegRev = SvnRevision::Load(&rev);

				target = gcnew SvnUriTarget(uri, pegRev);
				return true;
			}
		}
		else
			svn_error_clear(r);
	}
	else
	{
		System::Uri^ uri;

		if(System::Uri::TryCreate(targetString, UriKind::Absolute, uri))
		{
			target = gcnew SvnUriTarget(uri);
			return true;
		}
	}

	target = nullptr;
	return false;
}