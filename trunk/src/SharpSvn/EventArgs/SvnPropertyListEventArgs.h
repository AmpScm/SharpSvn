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
					_properties = SvnBase::CreatePropertyDictionary(_propHash, _pool);

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