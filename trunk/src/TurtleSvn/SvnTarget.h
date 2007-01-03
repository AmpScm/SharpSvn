
#pragma once

#include "SvnClientContext.h"

namespace QQn {
	namespace Svn {
		using namespace System;

		public enum class PegType
		{
			None		= svn_opt_revision_unspecified, 
			Number		= svn_opt_revision_number,
			Date		= svn_opt_revision_date,
			Committed	= svn_opt_revision_committed,
			Previous	= svn_opt_revision_previous,
			Base		= svn_opt_revision_base,
			Working		= svn_opt_revision_working,
			Head		= svn_opt_revision_head
		};

		public ref class PegRevision sealed
		{
			initonly PegType _type;
			initonly long _rev;
			initonly __int64 _date;

		internal:
			static PegRevision^ Load(svn_opt_revision_t* revData);
		public:
			PegRevision()
			{
				_type = PegType::None;
			}

			PegRevision(long revision)
			{
				if(revision < 0)
					throw gcnew ArgumentOutOfRangeException("revision");

				_type = PegType::Number;
				_rev = revision;
			}

			PegRevision(PegType type)
			{
				switch(type)
				{
				case PegType::None:
				case PegType::Committed:
				case PegType::Previous:
				case PegType::Base:
				case PegType::Working:
				case PegType::Head:
					_type = type;
					break;
				default:
					throw gcnew ArgumentOutOfRangeException("type");
				}
			}

			PegRevision(DateTime date)
			{
				_type = PegType::Date;
				_date = date.ToBinary();
			}

			virtual String^ ToString() override
			{
				switch(_type)
				{
				case PegType::None:
					return "";
				case PegType::Number:
					return _rev.ToString(System::Globalization::CultureInfo::InvariantCulture);
				case PegType::Date:
					return "{" + DateTime(_date).ToString("s") + "}";
				case PegType::Committed:
					return "COMMITTED";
				case PegType::Previous:
					return "PREVIOUS";
				case PegType::Base:
					return "BASE";
				case PegType::Working:
					return "WORKING";
				case PegType::Head:
					return "HEAD";
				default:
					throw gcnew InvalidOperationException("Invalid PegType set");
				}
			}

			property PegType Type
			{
				PegType get()
				{
					return _type;
				}
			}

			property long Revision
			{
				long get()
				{
					if(_type == PegType::Number)
						return _rev;
					else
						return 0;
				}
			}

			property DateTime Date
			{
				DateTime get()
				{
					if(_type == PegType::Date)
						return DateTime(_date);
					
					return DateTime::MinValue;
				}
			}

			static initonly PegRevision^ None		= gcnew PegRevision(PegType::None);
			static initonly PegRevision^ Head		= gcnew PegRevision(PegType::Head);
			static initonly PegRevision^ Working	= gcnew PegRevision(PegType::Working);
			static initonly PegRevision^ Base		= gcnew PegRevision(PegType::Base);
			static initonly PegRevision^ Previous	= gcnew PegRevision(PegType::Previous);
			static initonly PegRevision^ Committed	= gcnew PegRevision(PegType::Committed);
		};

		ref class SvnUriTarget;
		ref class SvnPathTarget;

		public ref class SvnTarget abstract : public SvnBase
		{
			initonly PegRevision^ _revision;
		protected:
			SvnTarget(PegRevision^ revision)
			{
				if(revision == nullptr)
					_revision = PegRevision::None;
				else
					_revision = revision;
			}

		public:
			property PegRevision^ Revision
			{
				PegRevision^ get()
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
				if(Revision->Type == PegType::None)
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
			static bool TryParse(String^ targetString, [Out] SvnTarget^% target, AprPool^ pool);
		};
	}
}