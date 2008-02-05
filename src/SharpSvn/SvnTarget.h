// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientContext.h"
#include "SvnEnums.h"

namespace SharpSvn {
	using namespace System;

	public ref class SvnRevision sealed : public IEquatable<SvnRevision^>
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

		SvnRevision(int revision)
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
				return "{" + DateTime(_value).ToString("s", System::Globalization::CultureInfo::InvariantCulture) + "}";
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
				return nullptr;
			}
		}

		property SvnRevisionType RevisionType
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

		virtual bool Equals(Object^ obj) override
		{
			SvnRevision ^rev = dynamic_cast<SvnRevision^>(obj);

			return Equals(rev);
		}

		virtual bool Equals(SvnRevision^ other)
		{
			if(!other)
				return false;

			switch(RevisionType)
			{
			case SvnRevisionType::None:
			case SvnRevisionType::Committed:
			case SvnRevisionType::Previous:
			case SvnRevisionType::Base:
			case SvnRevisionType::Working:
			case SvnRevisionType::Head:
				return true;
			case SvnRevisionType::Number:
			case SvnRevisionType::Date:
				return _value == other->_value;
			default:
				return false;
			}
		}

		virtual int GetHashCode() override
		{
			return _type.GetHashCode() ^ _value.GetHashCode();
		}

		static bool operator == (SvnRevision^ rev1, SvnRevision^ rev2)
		{
			if(!rev1)
				return !rev2;
			else if(!rev2)
				return false;

			return rev1->Equals(rev2);
		}


		static bool operator != (SvnRevision^ rev1, SvnRevision^ rev2)
		{
			return !(rev1 == rev2);
		}

	internal:
		svn_opt_revision_t ToSvnRevision();
		svn_opt_revision_t ToSvnRevision(SvnRevision^ noneValue);

		svn_opt_revision_t* AllocSvnRevision(AprPool ^pool);

	private:
		static initonly SvnRevision^ _none		= gcnew SvnRevision(SvnRevisionType::None);
		static initonly SvnRevision^ _head		= gcnew SvnRevision(SvnRevisionType::Head);
		static initonly SvnRevision^ _working	= gcnew SvnRevision(SvnRevisionType::Working);
		static initonly SvnRevision^ _base		= gcnew SvnRevision(SvnRevisionType::Base);
		static initonly SvnRevision^ _previous	= gcnew SvnRevision(SvnRevisionType::Previous);
		static initonly SvnRevision^ _committed	= gcnew SvnRevision(SvnRevisionType::Committed);
		static initonly SvnRevision^ _zero		= gcnew SvnRevision((__int64)0);
	public:
		static property SvnRevision^ None		{ SvnRevision^ get() { return _none; } }
		static property SvnRevision^ Head		{ SvnRevision^ get() { return _head; } }
		static property SvnRevision^ Working	{ SvnRevision^ get() { return _working; } }
		static property SvnRevision^ Base		{ SvnRevision^ get() { return _base; } }
		static property SvnRevision^ Previous	{ SvnRevision^ get() { return _previous; } }
		static property SvnRevision^ Committed	{ SvnRevision^ get() { return _committed; } }
		static property SvnRevision^ Zero		{ SvnRevision^ get() { return _zero; } }
	};

	ref class SvnUriTarget;
	ref class SvnPathTarget;

	public ref class SvnTarget abstract : public SvnBase, public IEquatable<SvnTarget^>
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
			if(Revision->RevisionType == SvnRevisionType::None)
				return TargetName;
			else
				return TargetName + "@" + Revision->ToString();
		}

		static SvnTarget^ FromUri(Uri^ value);
		static SvnTarget^ FromString(String^ value);

		static operator SvnTarget^(Uri^ value)			{ return value ? FromUri(value) : nullptr; }
		static operator SvnTarget^(String^ value)		{ return value ? FromString(value) : nullptr; }

		virtual bool Equals(Object^ obj) override
		{
			SvnTarget^ target = dynamic_cast<SvnTarget^>(obj);

			return Equals(target);
		}

		virtual bool Equals(SvnTarget^ other)
		{
			if(!other)
				return false;

			if(!String::Equals(other->TargetName, TargetName))
				return false;

			return Revision->Equals(other->Revision);
		}

		virtual int GetHashCode() override
		{
			return TargetName->GetHashCode();
		}


	public:
		static bool TryParse(String^ targetName, [Out] SvnTarget^% target);
	internal:
		virtual svn_opt_revision_t GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue) abstract;
	};
}
