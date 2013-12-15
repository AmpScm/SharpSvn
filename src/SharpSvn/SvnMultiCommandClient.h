// Copyright 2013 The SharpSvn Project
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

namespace SharpSvn {

    public ref class SvnRepositoryOperationBaseArgs abstract : public SvnClientArgs
    {
    public:
        property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::RepositoryOperations;
            }
        }
    };

    public ref class SvnRepositoryCreateDirectoryArgs : public SvnRepositoryOperationBaseArgs {};
    public ref class SvnRepositoryDeleteArgs : public SvnRepositoryOperationBaseArgs {};
    public ref class SvnRepositoryMoveArgs : public SvnRepositoryOperationBaseArgs {};
    public ref class SvnRepositoryCopyArgs : public SvnRepositoryOperationBaseArgs {};
    public ref class SvnRepositorySetPropertyArgs : public SvnRepositoryOperationBaseArgs {};
    public ref class SvnRepositoryUpdateFileArgs : public SvnRepositoryOperationBaseArgs {};
    public ref class SvnRepositoryCreateFileArgs : public SvnRepositoryOperationBaseArgs {};
    public ref class SvnRepositoryGetNodeKindArgs : public SvnRepositoryOperationBaseArgs {};

    public ref class SvnMultiCommandClient : SvnClientContext
    {
        AprPool _pool;
        svn_client_mtcc_t *_mtcc;
        SvnRepositoryOperationArgs ^_roArgs;
    public:
        /// <summary>Creates a new <see cref="SvnMultiCommandClient" /> completely
        /// operating like a command on the <see cref="SvnClient" /></summary>
        SvnMultiCommandClient(SvnClient ^via);

        /// <summary>Creates a new standalone <see cref="SvnMultiCommandClient" /> and
        /// opens it for the specified uri</summary>
        SvnMultiCommandClient(Uri ^sessionUri, SvnRepositoryOperationArgs ^args);

        /// <summary>Creates a new standalone <see cref="SvnMultiCommandClient" /> and
        /// opens it for the specified uri</summary>
        SvnMultiCommandClient(Uri ^sessionUri);

        /// <summary>Creates a new standalone <see cref="SvnMultiCommandClient" /> but
        /// doesn't open it yet</summary>
        SvnMultiCommandClient();

    public:
        /// <summary>Opens a not previously opened  <see cref="SvnMultiCommandClient" />
        /// to the specified url</summary>
        bool Open(Uri ^sessionUri);
        /// <summary>Opens a not previously opened  <see cref="SvnMultiCommandClient" />
        /// to the specified url</summary>
        bool Open(Uri ^sessionUri, SvnRepositoryOperationArgs ^args);

    public:
        bool Commit();
        bool Commit([Out] SvnCommitResult ^%result);

    public:
        bool CreateDirectory(String ^path);
        bool CreateDirectory(String ^path, SvnRepositoryCreateDirectoryArgs ^args);

        bool Delete(String ^path);
        bool Delete(String ^path, SvnRepositoryDeleteArgs ^args);

        bool Move(String ^fromPath, String ^toPath);
        bool Move(String ^fromPath, String ^toPath, SvnRepositoryMoveArgs ^args);

        bool Copy(String ^fromPath, String ^toPath);
        bool Copy(String ^fromPath, __int64 fromRev, String ^toPath);
        bool Copy(String ^fromPath, String ^toPath, SvnRepositoryCopyArgs ^args);
        bool Copy(String ^fromPath, __int64 fromRev, String ^toPath, SvnRepositoryCopyArgs ^args);

        bool SetProperty(String^ path, String^ propertyName, String^ value);
        bool SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes);
        bool DeleteProperty(String^ path, String^ propertyName);
        bool SetProperty(String^ path, String^ propertyName, String^ value, SvnRepositorySetPropertyArgs ^args);
        bool SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes, SvnRepositorySetPropertyArgs ^args);
        bool DeleteProperty(String^ path, String^ propertyName, SvnRepositorySetPropertyArgs ^args);

        bool UpdateFile(String ^path, System::IO::Stream ^newData);
        bool UpdateFile(String ^path, System::IO::Stream ^newData, SvnRepositoryUpdateFileArgs ^args);

        bool CreateFile(String ^path, System::IO::Stream ^newData);
        bool CreateFile(String ^path, System::IO::Stream ^newData, SvnRepositoryCreateFileArgs ^args);

        bool GetNodeKind(String ^path, SvnNodeKind %kind);
        bool GetNodeKind(String ^path, SvnRepositoryGetNodeKindArgs ^args, SvnNodeKind %kind);

    private:
        bool InternalSetProperty(String^ target, String^ propertyName, const svn_string_t* value, SvnRepositorySetPropertyArgs^ args, AprPool^ pool);

    internal:
        bool ViaCommit([Out] SvnCommitResult ^%result)
        {
            return Commit(result);
        }
        bool ViaOpen(Uri ^sessionUri, SvnRepositoryOperationArgs ^args)
        {
            return Open(sessionUri, args);
        }
    };
}