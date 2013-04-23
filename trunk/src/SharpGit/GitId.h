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

	namespace Plumbing
	{
		public enum class GitObjectKind
		{
			/// <summary>Object can be any of the following</summary>
			Any = GIT_OBJ_ANY,

			/// <summary>Object is invalid</summary>
			Bad = GIT_OBJ_BAD,

			/// <summary>Reserved</summary>
			Ext0 = GIT_OBJ__EXT1,

			Commit = GIT_OBJ_COMMIT,

			Tree = GIT_OBJ_TREE,

			Blob = GIT_OBJ_BLOB,

			Tag = GIT_OBJ_TAG,

			/// <summary>Reserved</summary>
			Ext5 = GIT_OBJ__EXT2,

			/// <summary>GIT_OBJ_OFS_DELTA</summary>
			DeltaOffset = GIT_OBJ_OFS_DELTA,

			DeltaReference = GIT_OBJ_REF_DELTA,
		};

		public enum class GitError
		{
			NoMemory			= GITERR_NOMEMORY,
			OperatingSystem		= GITERR_OS,
			Invalid				= GITERR_INVALID,
			Reference			= GITERR_REFERENCE,
			Zlib				= GITERR_ZLIB,
			Repository			= GITERR_REPOSITORY,
			Configuration		= GITERR_CONFIG,
			RegularExpression	= GITERR_REGEX,
			ObjectDatabase		= GITERR_ODB,
			Index				= GITERR_INDEX,
			Object				= GITERR_OBJECT,
			Network				= GITERR_NET,
			Tag					= GITERR_TAG,
			Tree				= GITERR_TREE,
			Indexer				= GITERR_INDEXER,
			SecureSockets		= GITERR_SSL,
			Submodule			= GITERR_SUBMODULE,
			Thread				= GITERR_THREAD,
			Stash				= GITERR_STASH,
			CheckOut			= GITERR_CHECKOUT,
			FetchHead			= GITERR_FETCHHEAD,
			Merge				= GITERR_MERGE,
		};

	}

	using System::ArgumentNullException;
	using System::ArgumentOutOfRangeException;

	[System::Serializable]
	public ref class GitException : public System::Exception
	{
		initonly SharpGit::Plumbing::GitError _err;

	protected:
		GitException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
			: Exception(info, context)
		{
			if (!info)
				throw gcnew ArgumentNullException("info");

			_err = (SharpGit::Plumbing::GitError)info->GetInt32("_err");
		}

	public:
		GitException()
		{
		}

		GitException(String^ message)
			: Exception(message)
		{
		}

		GitException(SharpGit::Plumbing::GitError err, String^ message)
			: Exception(message)
		{
			_err = err;
		}

		GitException(String^ message, Exception^ inner)
			: Exception(message, inner)
		{
		}

		GitException(SharpGit::Plumbing::GitError err, String^ message, Exception^ inner)
			: Exception(message, inner)
		{
			_err = err;
		}

		/// <summary>Gets the raw subversion error code</summary>
		property SharpGit::Plumbing::GitError GitError
		{
			SharpGit::Plumbing::GitError get()
			{
				return _err;
			}
		}

		/// <summary>Gets the root cause of the exception; commonly the most <see cref="InnerException" /></summary>
		property Exception^ RootCause
		{
			Exception^ get()
			{
				Exception^ e = this;
				while (e->InnerException)
					e = e->InnerException;

				return e;
			}
		}

		Exception^ GetCause(System::Type^ exceptionType)
		{
			if (!exceptionType)
				throw gcnew ArgumentNullException("exceptionType");
			else if (!Exception::typeid->IsAssignableFrom(exceptionType))
				throw gcnew ArgumentOutOfRangeException("exceptionType");

			Exception^ e = this;

			while (e)
			{
				if (exceptionType->IsAssignableFrom(e->GetType()))
					return e;

				e = e->InnerException;
			}

			return nullptr;
		}

		generic<typename T> where T : Exception
		T GetCause()
		{
			return (T)GetCause(T::typeid);
		}

	public:
		[System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::LinkDemand, Flags = System::Security::Permissions::SecurityPermissionFlag::SerializationFormatter)]
		virtual void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context) override
		{
			if (!info)
				throw gcnew ArgumentNullException("info");
			Exception::GetObjectData(info, context);

			info->AddValue("_err", (int)_err);
		}
	};
}
