#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn;

SvnRevision^ SvnRevision::Load(svn_opt_revision_t *revData)
{
	if(!revData)
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
	case SvnRevisionType::Date:
		// apr_time_t is in microseconds since 1-1-1970 UTC; filetime is in 100 nanoseconds
		return gcnew SvnRevision(SvnBase::DateTimeFromAprTime(revData->value.date));
	default:
		throw gcnew ArgumentException("SvnRevisionType unknown", "revData");
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
	case SvnRevisionType::Date:
		{
			r.value.date = SvnBase::AprTimeFromDateTime(DateTime::FromBinary(_value));
		}
		break;
	}
	return r;
}

svn_opt_revision_t* SvnRevision::AllocSvnRevision(AprPool ^pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_opt_revision_t *rev = (svn_opt_revision_t *)pool->Alloc(sizeof(svn_opt_revision_t));

	*rev = ToSvnRevision();

	return rev;
}

bool SvnTarget::TryParse(String^ targetString, [Out] SvnTarget^% target)
{
	if(String::IsNullOrEmpty(targetString))
		throw gcnew ArgumentNullException("targetString");

	SvnUriTarget^ uriTarget = nullptr;
	SvnPathTarget^ pathTarget = nullptr;

	if(targetString->Contains("://") && SvnUriTarget::TryParse(targetString, uriTarget))
	{
		target = uriTarget;
		return true;
	}
	else if(SvnPathTarget::TryParse(targetString, pathTarget))
	{
		target = pathTarget;
		return true;
	}

	target = nullptr;
	return false;
}

bool SvnTarget::TryParse(String^ targetString, [Out] SvnTarget ^% target, AprPool^ pool)
{
	if(String::IsNullOrEmpty(targetString))
		throw gcnew ArgumentNullException("targetString");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	SvnUriTarget^ uriTarget = nullptr;
	SvnPathTarget^ pathTarget = nullptr;

	if(targetString->Contains("://") && SvnUriTarget::TryParse(targetString, uriTarget, pool))
	{
		target = uriTarget;
		return true;
	}
	else if(SvnPathTarget::TryParse(targetString, pathTarget, pool))
	{
		target = pathTarget;
		return true;
	}

	target = nullptr;
	return false;
}

SvnTarget^ SvnTarget::FromUri(Uri^ value)
{
	if(!value)
		throw gcnew ArgumentNullException("value");

	return gcnew SvnUriTarget(value);
}

SvnTarget^ SvnTarget::FromString(String^ value)
{
	if(!value)
		throw gcnew ArgumentNullException("value");

	SvnTarget^ result;

	if(SvnTarget::TryParse(value, result))
	{
		return result;
	}
	else
		throw gcnew System::ArgumentException("Value is not a valid SvnUriTarget and/or SvnPathTarget", "value");
}

SvnPathTarget^ SvnPathTarget::FromString(String^ value)
{
	if(!value)
		throw gcnew ArgumentNullException("value");

	SvnPathTarget^ result;

	if(SvnPathTarget::TryParse(value, result))
	{
		return result;
	}
	else
		throw gcnew System::ArgumentException("Value is not a valid SvnPathTarget", "value");
}

SvnUriTarget^ SvnUriTarget::FromString(String^ value)
{
	if(!value)
		throw gcnew ArgumentNullException("value");

	SvnUriTarget^ result;

	if(SvnUriTarget::TryParse(value, result))
	{
		return result;
	}
	else
		throw gcnew System::ArgumentException("Value is not a valid SvnUriTarget", "value");
}