#pragma once
namespace SharpGit {

	public ref class GitId sealed
		: public System::IComparable<GitId^>,
		  public System::IEquatable<GitId^>
	{
		initonly array<System::Byte>^ _id;
		String^ _hex;

		static array<System::Byte>^ _null = gcnew array<System::Byte>(GIT_OID_RAWSZ);
	public:
		GitId()
		{
			_id = _null;
		}

		static initonly GitId^ Empty = gcnew GitId();

	internal:
		GitId(const git_oid *from)
		{
			if (! from)
				throw gcnew System::ArgumentNullException("from");

			array<System::Byte>^ c = gcnew array<System::Byte>(GIT_OID_RAWSZ);
			pin_ptr<System::Byte> pc = &c[0];

			memcpy(pc, from, GIT_OID_RAWSZ);
			_id = c;
		}

		GitId(const git_oid &from)
		{
			array<System::Byte>^ c = gcnew array<System::Byte>(GIT_OID_RAWSZ);
			pin_ptr<System::Byte> pc = &c[0];

			memcpy(pc, &from, GIT_OID_RAWSZ);
			_id = c;
		}

	public:
		GitId(String ^hexString)
		{
			if (String::IsNullOrEmpty("hexString"))
				throw gcnew System::ArgumentNullException("hexString");

			array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(hexString);
			pin_ptr<unsigned char> pBytes = &bytes[0];

			git_oid value;
			if (0 != git_oid_fromstr(&value, reinterpret_cast<const char *>(pBytes)))
				throw gcnew System::ArgumentException("Not a valid hexstring", "hexString");

			array<System::Byte>^ c = gcnew array<System::Byte>(GIT_OID_RAWSZ);
			pin_ptr<System::Byte> pc = &c[0];

			memcpy(pc, &value, GIT_OID_RAWSZ);
			_id = c;
		}

	public:
		virtual String^ ToString() override
		{
			if (!_hex)
			{
				char rslt[GIT_OID_HEXSZ+1];
				memset(rslt, 0, sizeof(rslt));

				array<System::Byte>^ id1 = _id;
				pin_ptr<System::Byte> pc = &id1[0];

				git_oid_fmt(rslt, reinterpret_cast<const git_oid*>(pc));

				return gcnew System::String(rslt);
			}

			return _hex;
		}

		virtual bool Equals(GitId ^other)
		{
			return CompareTo(other) == 0;
		}
		
		virtual bool Equals(Object ^ob) override
		{
			GitId ^other = dynamic_cast<GitId^>(ob);

			if ((Object^)other == nullptr)
				return false;

			return CompareTo(other) == 0;
		}

		virtual int CompareTo(GitId^ other)
		{
			if ((Object^)other == nullptr)
				return -1;

			array<System::Byte>^ id1 = _id;
			array<System::Byte>^ id2 = other->_id;

			pin_ptr<System::Byte> pc1 = &id1[0];
			pin_ptr<System::Byte> pc2 = &id2[0];

			return git_oid_cmp(reinterpret_cast<const git_oid*>(pc1),
							   reinterpret_cast<const git_oid*>(pc2));
		}

		static bool operator ==(GitId ^id1, GitId ^id2)
		{
			bool n1 = ((Object^)id1 == nullptr);
			bool n2 = ((Object^)id2 == nullptr);

			if (n1 || n2)
				return n1 && n2;

			return id1->Equals(id2);
		}

		static bool operator !=(GitId ^id1, GitId ^id2)
		{
			return !(id1 == id2);
		}

		virtual int GetHashCode() override
		{
			return System::BitConverter::ToInt32(_id, 0);
		}

	internal:
		git_oid AsOid()
		{
			git_oid rslt;

			array<System::Byte>^ id1 = _id;
			pin_ptr<System::Byte> pc = &id1[0];

			git_oid_cpy(&rslt, reinterpret_cast<const git_oid*>(pc));
			return rslt;
		}
	};

}