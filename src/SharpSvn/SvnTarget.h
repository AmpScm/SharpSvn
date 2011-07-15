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

#include "SvnClientContext.h"
#include "SvnEnums.h"

namespace SharpSvn {
	using namespace System;

	public ref class SvnRevision sealed : public IEquatable<SvnRevision^>
	{
		initonly SvnRevisionType _type;
		initonly __int64 _value;

	internal:
		static SvnRevision^ Load(const svn_opt_revision_t* revData);
	public:
		SvnRevision()
		{
			_type = SvnRevisionType::None;
		}

		SvnRevision(int revision)
		{
			if (revision < 0)
				throw gcnew ArgumentOutOfRangeException("revision", revision, SharpSvnStrings::RevisionMustBeGreaterThanOrEqualToZero);

			_type = SvnRevisionType::Number;
			_value = revision;
		}

		SvnRevision(__int64 revision)
		{
			if (revision < 0)
				throw gcnew ArgumentOutOfRangeException("revision", revision, SharpSvnStrings::RevisionMustBeGreaterThanOrEqualToZero);

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
			_type = SvnRevisionType::Time;
			_value = date.ToUniversalTime().Ticks;
		}

		virtual String^ ToString() override
		{
			switch(_type)
			{
			case SvnRevisionType::None:
				return "";
			case SvnRevisionType::Number:
				return _value.ToString(System::Globalization::CultureInfo::InvariantCulture);
			case SvnRevisionType::Time:
				return "{" + DateTime(_value, DateTimeKind::Utc).ToString("s", System::Globalization::CultureInfo::InvariantCulture) + "}";
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
				if (_type == SvnRevisionType::Number)
					return _value;
				else
					return -1;
			}
		}

		property DateTime Time
		{
			DateTime get()
			{
				if (_type == SvnRevisionType::Time)
					return DateTime(_value, DateTimeKind::Utc);

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
			if (!other)
				return false;

			if (other->RevisionType != RevisionType)
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
			case SvnRevisionType::Time:
				return _value == other->_value;
			default:
				return false;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return _type.GetHashCode() ^ _value.GetHashCode();
		}

		/// <summary>Gets a boolean indicating whether the revisionnumber requires a workingcopy to make any sense</summary>
		property bool RequiresWorkingCopy
		{
			bool get()
			{
				switch(RevisionType)
				{
				case SvnRevisionType::None:
				case SvnRevisionType::Time:
				case SvnRevisionType::Head:
				case SvnRevisionType::Number:
					return false;
				default:
					return true;
				}
			}
		}

		/// <summary>Gets a boolean whether the revision specifies an explicit repository revision</summary>
		/// <remarks>Explicit: Is set, doesn't require a workingcopy and is repository wide applyable</remarks>
		property bool IsExplicit
		{
			bool get()
			{
				return (RevisionType != SvnRevisionType::None) &&
					!RequiresWorkingCopy;
			}
		}

		static bool operator == (SvnRevision^ rev1, SvnRevision^ rev2)
		{
			if (!rev1)
				return !rev2;
			else if (!rev2)
				return false;

			return rev1->Equals(rev2);
		}

		static bool operator != (SvnRevision^ rev1, SvnRevision^ rev2)
		{
			return !(rev1 == rev2);
		}

		static operator SvnRevision^(__int64 value)
		{
			if (value == 0)
				return Zero;
			else if(value == 1)
				return One;
			else
				return gcnew SvnRevision(value);
		}

		static operator SvnRevision^(DateTime value)
		{
			return gcnew SvnRevision(value);
		}

		static operator SvnRevision^(SvnRevisionType value)
		{
			switch(value)
			{
			case SvnRevisionType::None:
				return None;
			case SvnRevisionType::Head:
				return Head;
			case SvnRevisionType::Working:
				return Working;
			case SvnRevisionType::Base:
				return Base;
			case SvnRevisionType::Previous:
				return Previous;
			case SvnRevisionType::Committed:
				return Committed;
			default:
				return gcnew SvnRevision(value);
			}
		}

	internal:
		SvnRevision^ Or(SvnRevision^ alternate)
		{
			if (RevisionType != SvnRevisionType::None)
				return this;
			else
				return alternate ? alternate : SvnRevision::None;
		}

	internal:
		svn_opt_revision_t ToSvnRevision();

		svn_opt_revision_t* AllocSvnRevision(AprPool ^pool);

	private:
		static initonly SvnRevision^ _none		= gcnew SvnRevision(SvnRevisionType::None);
		static initonly SvnRevision^ _head		= gcnew SvnRevision(SvnRevisionType::Head);
		static initonly SvnRevision^ _working	= gcnew SvnRevision(SvnRevisionType::Working);
		static initonly SvnRevision^ _base		= gcnew SvnRevision(SvnRevisionType::Base);
		static initonly SvnRevision^ _previous	= gcnew SvnRevision(SvnRevisionType::Previous);
		static initonly SvnRevision^ _committed	= gcnew SvnRevision(SvnRevisionType::Committed);
		static initonly SvnRevision^ _zero		= gcnew SvnRevision((__int64)0);
		static initonly SvnRevision^ _one		= gcnew SvnRevision((__int64)1);
	public:
		static property SvnRevision^ None		{ SvnRevision^ get() { return _none; } }
		static property SvnRevision^ Head		{ SvnRevision^ get() { return _head; } }
		static property SvnRevision^ Working	{ SvnRevision^ get() { return _working; } }
		static property SvnRevision^ Base		{ SvnRevision^ get() { return _base; } }
		static property SvnRevision^ Previous	{ SvnRevision^ get() { return _previous; } }
		static property SvnRevision^ Committed	{ SvnRevision^ get() { return _committed; } }
		static property SvnRevision^ Zero		{ SvnRevision^ get() { return _zero; } }
		static property SvnRevision^ One		{ SvnRevision^ get() { return _one; } }
	};

	ref class SvnUriTarget;
	ref class SvnPathTarget;
	ref class SvnTarget;

	/// <summary>Generic encapsulation of a specific revision of a node in subversion</summary>
	public interface struct ISvnOrigin
	{
		/// <summary>(Required) The Uri of <see cref="Target" />. Only differs from target if <see cref="Target" />
		/// specifies a <see cref="SvnPathTarget" /></summary>
		property System::Uri^ Uri
		{
			System::Uri^ get() abstract;
		}

		/// <summary>The target specified</summary>
		property SvnTarget^ Target
		{
			SvnTarget^ get() abstract;
		}

		/// <summary>(Required) The repository root of <see cref="Target" /></summary>
		property System::Uri^ RepositoryRoot
		{
			System::Uri^ get() abstract;
		}

		/// <summary>(Optional) The node kind of <see cref="Target" /></summary>
		property SvnNodeKind NodeKind
		{
			SvnNodeKind get() abstract;
		}
	};

	public ref class SvnTarget abstract : public SvnBase, public IEquatable<SvnTarget^>
	{
		initonly SvnRevision^ _revision;
	internal:
		SvnTarget(SvnRevision^ revision)
		{
			if (!revision)
				_revision = SvnRevision::None;
			else
				_revision = revision;
		}

	public:
		/// <summary>Gets the operational revision</summary>
		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
		}

		/// <summary>Gets the target name in normalized format</summary>
		property String^ TargetName
		{
			virtual String^ get() abstract;
		}

		property String^ FileName
		{
			virtual String^ get() abstract;
		}

	internal:
		property String^ SvnTargetName
		{
			virtual String^ get()
			{
				return TargetName;
			}
		}

	public:
		/// <summary>Gets the SvnTarget as string</summary>
		virtual String^ ToString() override
		{
			if (Revision->RevisionType == SvnRevisionType::None)
				return TargetName;
			else
				return TargetName + "@" + Revision->ToString();
		}

		static SvnTarget^ FromUri(Uri^ value);
		static SvnTarget^ FromString(String^ value);
		static SvnTarget^ FromString(String^ value, bool allowOperationalRevision);

		static operator SvnTarget^(Uri^ value)			{ return value ? FromUri(value) : nullptr; }
		static operator SvnTarget^(String^ value)		{ return value ? FromString(value, false) : nullptr; }
		static operator SvnTarget^(ISvnOrigin^ origin)  { return origin ? origin->Target : nullptr; }

		virtual bool Equals(Object^ obj) override
		{
			SvnTarget^ target = dynamic_cast<SvnTarget^>(obj);

			return Equals(target);
		}

		virtual bool Equals(SvnTarget^ other)
		{
			if (!other)
				return false;

			if (!String::Equals(other->SvnTargetName, SvnTargetName))
				return false;

			return Revision->Equals(other->Revision);
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return TargetName->GetHashCode();
		}

	public:
		static bool TryParse(String^ targetName, [Out] SvnTarget^% target);
		static bool TryParse(String^ targetName, bool allowOperationalRevision, [Out] SvnTarget^% target);
	internal:
		virtual SvnRevision^ GetSvnRevision(SvnRevision^ fileNoneValue, SvnRevision^ uriNoneValue) abstract;
	};
}
