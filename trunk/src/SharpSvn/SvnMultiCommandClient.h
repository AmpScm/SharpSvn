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

#undef CreateFile

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
        initonly AprBaton<SvnMultiCommandClient^>^ _clientBaton;
        AprPool _pool;
        svn_client_mtcc_t *_mtcc;
        SvnRepositoryOperationArgs ^_roArgs;
        List<System::IDisposable^>^ _refs;

        void Initialize();
        ~SvnMultiCommandClient();

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
        /// <remarks>Unlike most other operations in SharpSvn the args object is used
        /// untile the SvnMultiCommandClient is committed or disposed</remarks>
        bool Open(Uri ^sessionUri, SvnRepositoryOperationArgs ^args);

    public:

        /// <summary>Commits all recorded operations as a new revision.</summary>
        bool Commit();
        /// <summary>Commits all recorded operations as a new revision.</summary>
        bool Commit([Out] SvnCommitResult ^%result);

    public:
        /// <summary>Creates the specified path</summary>
        bool CreateDirectory(String ^path);
        /// <summary>Creates the specified path</summary>
        bool CreateDirectory(String ^path, SvnRepositoryCreateDirectoryArgs ^args);

        /// <summary>Deletes the specified path</summary>
        bool Delete(String ^path);
        /// <summary>Deletes the specified path</summary>
        bool Delete(String ^path, SvnRepositoryDeleteArgs ^args);

        /// <summary>Moves the specified path to the new location</summary>
        bool Move(String ^fromPath, String ^toPath);
        /// <summary>Moves the specified path to the new location</summary>
        bool Move(String ^fromPath, String ^toPath, SvnRepositoryMoveArgs ^args);

        /// <summary>Copies the specified path to the new location</summary>
        bool Copy(String ^fromPath, String ^toPath);
        /// <summary>Copies the specified path to the new location</summary>
        bool Copy(String ^fromPath, __int64 fromRev, String ^toPath);
        /// <summary>Copies the specified path to the new location</summary>
        bool Copy(String ^fromPath, String ^toPath, SvnRepositoryCopyArgs ^args);
        /// <summary>Copies the specified path to the new location</summary>
        bool Copy(String ^fromPath, __int64 fromRev, String ^toPath, SvnRepositoryCopyArgs ^args);

        /// <summary>Sets a property on path, which can be an added or existing node</summary>
        bool SetProperty(String^ path, String^ propertyName, String^ value);
        /// <summary>Sets a property on path, which can be an added or existing node</summary>
        bool SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes);
        /// <summary>Deletes a property on path, which can be an added or existing node</summary>
        bool DeleteProperty(String^ path, String^ propertyName);
        /// <summary>Sets a property on path, which can be an added or existing node</summary>
        bool SetProperty(String^ path, String^ propertyName, String^ value, SvnRepositorySetPropertyArgs ^args);
        /// <summary>Sets a property on path, which can be an added or existing node</summary>
        bool SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes, SvnRepositorySetPropertyArgs ^args);
        /// <summary>Deletes a property on path, which can be an added or existing node</summary>
        bool DeleteProperty(String^ path, String^ propertyName, SvnRepositorySetPropertyArgs ^args);

        /// <summary>Updates the file that already exists at (or was just copied/moved to) PATH</summary>
        bool UpdateFile(String ^path, System::IO::Stream ^newData);
        /// <summary>Updates the file that already exists at (or was just copied/moved to) PATH</summary>
        bool UpdateFile(String ^path, System::IO::Stream ^newData, SvnRepositoryUpdateFileArgs ^args);

        /// <summary>Adds a new file at PATH</summary>
        bool CreateFile(String ^path, System::IO::Stream ^newData);
        /// <summary>Adds a new file at PATH</summary>
        bool CreateFile(String ^path, System::IO::Stream ^newData, SvnRepositoryCreateFileArgs ^args);

        /// <summary>Gets the (new) node kind of the node at PATH</summary>
        bool GetNodeKind(String ^path, SvnNodeKind %kind);
        /// <summary>Gets the (new) node kind of the node at PATH</summary>
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

    private:
        SvnClientArgs^ _currentArgs;

        property SvnClientArgs^ CurrentCommandArgs
        {
            SvnClientArgs^ get() new
            {
                return _currentArgs ? _currentArgs : __super::CurrentCommandArgs;
            }
        }

        ref class McArgsStore sealed
        {
            initonly SvnMultiCommandClient^ _client;
        public:
            inline McArgsStore(SvnMultiCommandClient^ client, SvnClientArgs^ args)
            {
                _client = client;
                assert(! _client->_currentArgs);
                client->_currentArgs = args;
            }
            inline ~McArgsStore()
            {
                _client->_currentArgs = nullptr;
            }
        };

    public:
        /// <summary>
        /// Raised to allow canceling operations. The event is first
        /// raised on the <see cref="SvnClientArgs" /> object and
        /// then on the <see cref="SvnClient" />
        /// </summary>
        DECLARE_EVENT(SvnCancelEventArgs^, Cancel)
        /// <summary>
        /// Raised on progress. The event is first
        /// raised on the <see cref="SvnClientArgs" /> object and
        /// then on the <see cref="SvnClient" />
        /// </summary>
        DECLARE_EVENT(SvnProgressEventArgs^, Progress)
        /// <summary>
        /// Raised on notifications. The event is first
        /// raised on the <see cref="SvnClientArgs" /> object and
        /// then on the <see cref="SvnClient" />
        /// </summary>
        DECLARE_EVENT(SvnNotifyEventArgs^, Notify);
        /// <summary>
        /// Raised on progress. The event is first
        /// raised on the <see cref="SvnClientArgsWithCommit" /> object and
        /// then on the <see cref="SvnClient" />
        /// </summary>
        DECLARE_EVENT(SvnCommittingEventArgs^, Committing)
        /// <summary>
        /// Raised on progress. The event is first
        /// raised on the <see cref="SvnClientArgsWithCommit" /> object and
        /// then on the <see cref="SvnClient" />
        /// </summary>
        DECLARE_EVENT(SvnCommittedEventArgs^, Committed)

        /// <summary>
        /// Raised when a subversion exception occurs.
        /// Set <see cref="SvnErrorEventArgs::Cancel" /> to true to cancel
        /// throwing the exception
        /// </summary>
        DECLARE_EVENT(SvnErrorEventArgs^, SvnError)

        /// <summary>
        /// Raised just before a command is executed. This allows a listener
        /// to cleanup before a new command is started
        /// </summary>
        DECLARE_EVENT(SvnProcessingEventArgs^, Processing)

    protected:
        /// <summary>Raises the <see cref="Cancel" /> event.</summary>
        virtual void OnCancel(SvnCancelEventArgs^ e);
        /// <summary>Raises the <see cref="Progress" /> event.</summary>
        virtual void OnProgress(SvnProgressEventArgs^ e);
        /// <summary>Raises the <see cref="Committing" /> event.</summary>
        virtual void OnCommitting(SvnCommittingEventArgs^ e);
        /// <summary>Raises the <see cref="Committing" /> event.</summary>
        virtual void OnCommitted(SvnCommittedEventArgs^ e);
        /// <summary>Raises the <see cref="Notify" /> event.</summary>
        virtual void OnNotify(SvnNotifyEventArgs^ e);
        /// <summary>Raises the <see cref="Exception" /> event.</summary>
        virtual void OnSvnError(SvnErrorEventArgs^ e);
        /// <summary>Raises the <see cref="Processing" /> event.</summary>
        virtual void OnProcessing(SvnProcessingEventArgs^ e);

    internal:
        virtual void HandleClientCancel(SvnCancelEventArgs^ e) override sealed;
        virtual void HandleClientProgress(SvnProgressEventArgs^ e) override sealed;
        virtual void HandleClientCommitting(SvnCommittingEventArgs^ e) override sealed;
        virtual void HandleClientCommitted(SvnCommittedEventArgs^ e) override sealed;
        virtual void HandleClientNotify(SvnNotifyEventArgs^ e) override sealed;
        virtual void HandleClientError(SvnErrorEventArgs^ e) override sealed;
        virtual void HandleProcessing(SvnProcessingEventArgs^ e) override sealed;
    };
}