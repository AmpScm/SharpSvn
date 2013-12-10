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

    public ref class SvnInheritedPropertyListEventArgs : public SvnCancelEventArgs
    {
        initonly int _index;
        Uri^ _uri;
        String^ _path;
        String^ _fullPath;
        SvnPropertyCollection^ _properties;

        const char *_path_or_url;
        apr_hash_t *_propHash;
        AprPool^ _pool;

    internal:
        SvnInheritedPropertyListEventArgs(const char *path_or_url, apr_hash_t *prop_hash, int index, AprPool^ pool)
        {
            if (!path_or_url)
                throw gcnew ArgumentNullException("path_or_url");

            _index = index;
            _path_or_url = path_or_url;
            _propHash = prop_hash;
            _pool = pool;
        }

    public:
        property String^ Path
        {
            String^ get()
            {
                if (!_path && _path_or_url && !svn_path_is_url(_path_or_url) && _pool)
                    _path = SvnBase::Utf8_PathPtrToString(_path_or_url, _pool);

                return _path;
            }
        }

        property String^ FullPath
        {
            String^ get()
            {
                if (!_fullPath && Path)
                    _fullPath = SvnTools::GetNormalizedFullPath(Path);

                return _fullPath;
            }
        }

        property System::Uri^ Uri
        {
            System::Uri^ get()
            {
                if (!_uri && _path_or_url && svn_path_is_url(_path_or_url))
                    _uri = SvnBase::Utf8_PtrToUri(_path_or_url, _index ? SvnNodeKind::Directory : SvnNodeKind::Unknown);

                return _uri;
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
            return Path ? Path->GetHashCode() : (Uri ? Uri->GetHashCode() : 0);
        }

    protected public:
        virtual void Detach(bool keepProperties) override
        {
            try
            {
                if (keepProperties)
                {
                    GC::KeepAlive(Path);
                    GC::KeepAlive(Uri);
                    GC::KeepAlive(Properties);
                }
            }
            finally
            {
                _path_or_url = nullptr;
                _propHash = nullptr;
                _pool = nullptr;
                __super::Detach(keepProperties);
            }
        }
    };
}
