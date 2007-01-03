#include "stdafx.h"

#include "SvnAll.h"

using namespace QQn::Svn;

PegRevision^ PegRevision::Load(svn_opt_revision_t *revData)
{
	if(!revData)
		throw gcnew ArgumentNullException("revData");

	PegType type = (PegType)revData->kind;


	switch(type)
	{
		case PegType::None:
			return None;
		case PegType::Committed:
			return Committed;
		case PegType::Previous:
			return Previous;
		case PegType::Base:
			return Base;
		case PegType::Working:
			return Working;
		case PegType::Head:
			return Head;
		case PegType::Number:
			return gcnew PegRevision(revData->value.number);
		case PegType::Date:
			// apr_time_t is in microseconds since 1-1-1970 UTC; filetime is in 100 nanoseconds
			{
				__int64 aprTimeBase = DateTime(1970,1,1).ToBinary();
				return gcnew PegRevision(System::DateTime(revData->value.date*10 + aprTimeBase));
			}
		default:
			throw gcnew ArgumentException("PegType unknown", "revData");
	}	
}

bool SvnTarget::TryParse(String^ targetString, SvnTarget^% target)
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

bool SvnTarget::TryParse(String^ targetString, SvnTarget ^% target, AprPool^ pool)
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