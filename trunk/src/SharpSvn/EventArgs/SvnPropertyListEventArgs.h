// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

public ref class SvnPropertyListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		apr_hash_t* _propHash;
		SvnPropertyCollection^ _properties;
		AprPool^ _pool;

	internal:
		SvnPropertyListEventArgs(const char *path, apr_hash_t* prop_hash, AprPool ^pool)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!prop_hash)
				throw gcnew ArgumentNullException("prop_hash");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_path = SvnBase::Utf8_PtrToString(path);
			_propHash = prop_hash;
			_pool = pool;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property SvnPropertyCollection^ Properties
		{
			SvnPropertyCollection^ get()
			{
				if (!_properties && _propHash && _pool)
					_properties = SvnBase::CreatePropertyDictionary(_propHash, _pool, path);

				return _properties;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Path->GetHashCode();
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(Properties);
					//					GC::KeepAlive(Entry);
				}
			}
			finally
			{
				_propHash = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

}