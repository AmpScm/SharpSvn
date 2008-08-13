// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	public ref class SvnPropertyValue sealed
	{
		initonly String^ _key;
		initonly String^ _strValue;
		initonly SvnTarget^ _target;
		array<Byte>^ _value;

	public:
		SvnPropertyValue(String^ key, String^ value)
		{
			if (String::IsNullOrEmpty(key))
				throw gcnew ArgumentNullException("key");
			else if (String::IsNullOrEmpty(key))
				throw gcnew ArgumentNullException("value");

			_key = key;
			_strValue = value;
		}

		SvnPropertyValue(String^ key, array<Byte>^ value)
		{
			if (String::IsNullOrEmpty(key))
				throw gcnew ArgumentNullException("key");
			else if (!value)
				throw gcnew ArgumentNullException("value");

			_key = key;
			_value = value;
		}

		SvnPropertyValue(String^ key, array<Byte>^ value, String^ strValue, SvnTarget^ target)
		{
			if (String::IsNullOrEmpty(key))
				throw gcnew ArgumentNullException("key");
			else if (String::IsNullOrEmpty(key))
				throw gcnew ArgumentNullException("value");
			else if (!value)
				throw gcnew ArgumentNullException("value");

			_target = target;
			_key = key;
			_value = value;
			_strValue = strValue;
		}

		SvnPropertyValue(String^ key, array<Byte>^ value, SvnTarget^ target)
		{
			if (String::IsNullOrEmpty(key))
				throw gcnew ArgumentNullException("key");
			else if (!value)
				throw gcnew ArgumentNullException("value");

			_target = target;
			_key = key;
			_value = value;
		}

	internal:

		static SvnPropertyValue^ Create(const char* propertyName, const svn_string_t* value, SvnTarget^ target)
		{
			if (!propertyName)
				throw gcnew ArgumentNullException("propertyName");

			String^ name = SvnBase::Utf8_PtrToString(propertyName);

			return Create(propertyName, value, target, name);
		}

		static SvnPropertyValue^ Create(const char* propertyName, const svn_string_t* value, SvnTarget^ target, String^ name)
		{
			if (!propertyName)
				throw gcnew ArgumentNullException("propertyName");
			else if (!value)
				throw gcnew ArgumentNullException("value");
			else if (!name)
				throw gcnew ArgumentNullException("name");

			Object^ val = SvnBase::PtrToStringOrByteArray(value->data, (int)value->len);
			String^ strVal = dynamic_cast<String^>(val);

			if (strVal)
			{
				if (svn_prop_needs_translation(propertyName))
					strVal = strVal->Replace("\n", Environment::NewLine);

				return gcnew SvnPropertyValue(name, SvnBase::PtrToByteArray(value->data, (int)value->len), strVal, target);
			}
			else
				return gcnew SvnPropertyValue(name, safe_cast<array<Byte>^>(val), target);
		}

	public:
		/// <summary>Gets the <see cref="SvnTarget" /> the <see cref="SvnPropertyValue" /> applies to;
		/// <c>null</c> if it applies to a revision property</summary>
		property SvnTarget^ Target
		{
			SvnTarget^ get()
			{
				return _target;
			}
		}

		property String^ Key
		{
			String^ get()
			{
				return _key;
			}
		}

		property String^ StringValue
		{
			String^ get()
			{
				return _strValue;
			}
		}

		property ICollection<Byte>^ RawValue
		{
			ICollection<Byte>^ get()
			{
				if (!_value)
					_value = System::Text::Encoding::UTF8->GetBytes(_strValue);

				return safe_cast<ICollection<Byte>^>(_value);
			}
		}

		virtual String^ ToString() override
		{
			if (StringValue)
				return StringValue;
			else
				return "<raw>";
		}
	};


	public ref class SvnPropertyCollection sealed : public System::Collections::ObjectModel::KeyedCollection<String^, SvnPropertyValue^>
	{
	protected:
		virtual String^ GetKeyForItem(SvnPropertyValue^ item) override
		{
			if (!item)
				return nullptr;
			else
				return item->Key;
		}

	};

	public ref class SvnRevisionPropertyCollection sealed : public System::Collections::ObjectModel::KeyedCollection<String^, SvnPropertyValue^>
	{
	protected:
		virtual String^ GetKeyForItem(SvnPropertyValue^ item) override
		{
			if (!item)
				return nullptr;
			else
				return item->Key;
		}

	public:
		void Add(String^ key, String^ value)
		{
			this->Add(gcnew SvnPropertyValue(key, value));
		}

		void Add(String^ key, array<Byte>^ value)
		{
			this->Add(gcnew SvnPropertyValue(key, value));
		}
	};

	public ref class SvnTargetPropertyCollection sealed : public System::Collections::ObjectModel::KeyedCollection<SvnTarget^, SvnPropertyValue^>
	{
	protected:
		virtual SvnTarget^ GetKeyForItem(SvnPropertyValue^ item) override
		{
			if (!item)
				return nullptr;
			else
				return item->Target;
		}
	};
}