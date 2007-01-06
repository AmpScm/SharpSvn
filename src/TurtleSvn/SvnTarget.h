
#pragma once

#include "SvnClientContext.h"
#include "SvnEnums.h"

namespace TurtleSvn {
	using namespace System;

	public ref class SvnRevision sealed
	{
		initonly SvnRevisionType _type;
		initonly __int64 _value;

	internal:
		static SvnRevision^ Load(svn_opt_revision_t* revData);
	public:
		SvnRevision()
		{
			_type = SvnRevisionType::None;
		}

		SvnRevision(long revision)
		{
			if(revision < 0)
				throw gcnew ArgumentOutOfRangeException("revision");

			_type = SvnRevisionType::Number;
			_value = revision;
		}

		SvnRevision(__int64 revision)
		{
			if(revision < 0)
				throw gcnew ArgumentOutOfRangeException("revision");

			_type = SvnRevisionType::Number;
			_value = revision;
		}

		SvnRevision(SvnRevisionType type)
		{
			switch(type)
			{
			case SvnRevisionType::None:
			case SvnRevisionType::Committed:
			case SvnRevisionType::Previous:
			case SvnRevisionType::Base:
			case SvnRevisionType::Working:
			case SvnRevisionType::Head:
				_type = type;
				break;
			default:
				throw gcnew ArgumentOutOfRangeException("type");
			}
		}

		SvnRevision(DateTime date)
		{
			_type = SvnRevisionType::Date;
			_value = date.ToBinary();
		}

		virtual String^ ToString() override
		{
			switch(_type)
			{
			case SvnRevisionType::None:
				return "";
			case SvnRevisionType::Number:
				return _value.ToString(System::Globalization::CultureInfo::InvariantCulture);
			case SvnRevisionType::Date:
				return "{" + DateTime(_value).ToString("s") + "}";
			case SvnRevisionType::Committed:
				return "COMMITTED";
			case SvnRevisionType::Previous:
				return "PREVIOUS";
			case SvnRevisionType::Base:
				return "BASE";
			case SvnRevisionType::Working:
				return "WORKING";
			case SvnRevisionType::Head:
				return "HEAD";
			default:
				throw gcnew InvalidOperationException("Invalid SvnRevisionType set");
			}
		}

		property SvnRevisionType Type
		{
			SvnRevisionType get()
			{
				return _type;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				if(_type == SvnRevisionType::Number)
					return _value;
				else
					return 0;
			}
		}

		property DateTime Date
		{
			DateTime get()
			{
				if(_type == SvnRevisionType::Date)
					return DateTime(_value);

				return DateTime::MinValue;
			}
		}

	internal:
		svn_opt_revision_t ToSvnRevision();

	public:
		static initonly SvnRevision^ None		= gcnew SvnRevision(SvnRevisionType::None);
		static initonly SvnRevision^ Head		= gcnew SvnRevision(SvnRevisionType::Head);
		static initonly SvnRevision^ Working	= gcnew SvnRevision(SvnRevisionType::Working);
		static initonly SvnRevision^ Base		= gcnew SvnRevision(SvnRevisionType::Base);
		static initonly SvnRevision^ Previous	= gcnew SvnRevision(SvnRevisionType::Previous);
		static initonly SvnRevision^ Committed	= gcnew SvnRevision(SvnRevisionType::Committed);
	};

	ref class SvnUriTarget;
	ref class SvnPathTarget;

	public ref class SvnTarget abstract : public SvnBase
	{
		initonly SvnRevision^ _revision;
	protected:
		SvnTarget(SvnRevision^ revision)
		{
			if(revision == nullptr)
				_revision = SvnRevision::None;
			else
				_revision = revision;
		}

	public:
		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
		}

		property String^ TargetName
		{
			virtual String^ get() = 0;
		}

		virtual String^ ToString() override
		{
			if(Revision->Type == SvnRevisionType::None)
				return TargetName;
			else
				return TargetName + "@" + Revision->ToString();
		}

		static SvnTarget^ FromUri(Uri^ value);
		static SvnTarget^ FromString(String^ value);

		static operator SvnTarget^(Uri^ value)					{ return value ? FromUri(value) : nullptr; }
		static explicit operator SvnTarget^(String^ value)		{ return value ? FromString(value) : nullptr; }


	public:
		static bool TryParse(String^ targetString, [Out] SvnTarget^% target);
	internal:
		static bool TryParse(String^ targetString, [Out] SvnTarget^% target, AprPool^ pool);
	};
}
