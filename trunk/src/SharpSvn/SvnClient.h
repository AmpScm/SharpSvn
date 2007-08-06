
#pragma once

#include "SvnClientContext.h"
#include "SvnUriTarget.h"
#include "SvnPathTarget.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "SvnCommitItem.h"
#include "SvnMergeInfo.h"
#include "AprBaton.h"

namespace SharpSvn {

	ref class SvnCancelEventArgs;
	ref class SvnProgressEventArgs;
	ref class SvnLogEventArgs;
	ref class SvnNotifyEventArgs;


	using System::Runtime::InteropServices::GCHandle;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;
	using System::IO::Stream;
	using System::IO::FileStream;

	/// <summary>Subversion client instance; main entrance to Subversion api</summary>
	/// <remarks><para>Please note: the subversion is not thread safe. You can use an SvnClient from a single thread at a time</para>
	/// You can use multiple SvnClients in multiple threads at the same time</remarks>
	public ref class SvnClient : public SvnClientContext
	{
		initonly SharpSvn::Apr::AprBaton<SvnClient^>^ _clientBatton;
		AprPool^ _pool;
		SvnClientArgs^ _currentArgs;
		bool _noLogMessageRequired;
	internal:
		property SvnClientArgs^ CurrentArgs
		{
			SvnClientArgs^ get()
			{
				return _currentArgs;
			}
		}

		static String^ GetSvnPath(String ^path)
		{
			return System::IO::Path::GetFullPath(path)->Replace(System::IO::Path::DirectorySeparatorChar, '/');
		}

		bool IsRecursive(SvnDepth depth)		
		{ 
			switch(depth)
			{
			case SvnDepth::Empty:
			case SvnDepth::Files:
				return false;
			case SvnDepth::Infinity:
				return true;
			default:
				throw gcnew ArgumentException("Depth must be Empty, Files or Infinity in 1.4 compatible compilation", "depth");
			}
		}

		bool IsNotRecursive(SvnDepth depth)
		{
			switch(depth)
			{
			case SvnDepth::Empty:
			case SvnDepth::Files:
				return true;
			case SvnDepth::Infinity:
				return false;
			default:
				throw gcnew ArgumentException("Depth must be Empty, Files or Infinity in 1.4 compatible compilation", "depth");
			}
		}

	public:
		///<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
		SvnClient();
	internal:
		///<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
		SvnClient(AprPool^ pool);

	private:
		void Initialize();

	public:
		property static Version^ Version
		{
			System::Version^ get();
		}

		property static System::Version^ WrapperVersion
		{
			System::Version^ get();
		}

		ref class SvnClientConfiguration sealed
		{
			SvnClient^ _client;
		internal:
			SvnClientConfiguration(SvnClient^ client)
			{
				if(!client)
					throw gcnew ArgumentNullException("client");

				_client = client;
			}

		public:
			/// <summary>Gets or sets a boolean indicating whether commits will fail if no log message is provided</summary>
			property bool LogMessageRequired
			{
				bool get()
				{
					return !_client->_noLogMessageRequired;
				}
				void set(bool value)
				{
					_client->_noLogMessageRequired = !value;
				}
			}
		};

		SvnClientConfiguration^ _config;

		property SvnClientConfiguration^ Configuration
		{
			SvnClientConfiguration^ get()
			{
				if(!_config)
					_config = gcnew SvnClientConfiguration(this);

				return _config;
			}
		}

		/////////////////////////////////////////
#pragma region // Client events
	private:
		EventHandler<SvnClientCancelEventArgs^>^	_clientCancel;
		EventHandler<SvnClientProgressEventArgs^>^	_clientProgress;
		EventHandler<SvnClientNotifyEventArgs^>^	_clientNotify;
		EventHandler<SvnClientBeforeCommitEventArgs^>^	_clientBeforeCommit;
		EventHandler<SvnClientConflictResolveEventArgs^>^	_clientConflictResolver;
	public:
		event EventHandler<SvnClientCancelEventArgs^>^		ClientCancel
		{
			void add(EventHandler<SvnClientCancelEventArgs^>^ e)		{ _clientCancel += e; }
			void remove(EventHandler<SvnClientCancelEventArgs^>^ e)		{ _clientCancel -= e; }
		}

		event EventHandler<SvnClientProgressEventArgs^>^	ClientProgress
		{
			void add(EventHandler<SvnClientProgressEventArgs^>^ e)		{ _clientProgress += e; }
			void remove(EventHandler<SvnClientProgressEventArgs^>^ e)	{ _clientProgress -= e; }
		}

		event EventHandler<SvnClientNotifyEventArgs^>^		ClientNotify
		{
			void add(EventHandler<SvnClientNotifyEventArgs^>^ e)		{ _clientNotify += e; }
			void remove(EventHandler<SvnClientNotifyEventArgs^>^ e)		{ _clientNotify -= e; }
		}

		event EventHandler<SvnClientBeforeCommitEventArgs^>^	ClientBeforeCommit
		{
			void add(EventHandler<SvnClientBeforeCommitEventArgs^>^ e)		{ _clientBeforeCommit += e; }
			void remove(EventHandler<SvnClientBeforeCommitEventArgs^>^ e)	{ _clientBeforeCommit -= e; }
		}

		event EventHandler<SvnClientConflictResolveEventArgs^>^	ClientConflictResolver
		{
			void add(EventHandler<SvnClientConflictResolveEventArgs^>^ e)		{ _clientConflictResolver += e; }
			void remove(EventHandler<SvnClientConflictResolveEventArgs^>^ e)	{ _clientConflictResolver -= e; }
		}

	protected:
		virtual void OnClientCancel(SvnClientCancelEventArgs^ e);
		virtual void OnClientProgress(SvnClientProgressEventArgs^ e);
		virtual void OnClientBeforeCommit(SvnClientBeforeCommitEventArgs^ e);
		virtual void OnClientNotify(SvnClientNotifyEventArgs^ e);
		virtual void OnClientConflictResolver(SvnClientConflictResolveEventArgs^ e);

	internal:

		void HandleClientCancel(SvnClientCancelEventArgs^ e);
		void HandleClientProgress(SvnClientProgressEventArgs^ e);
		void HandleClientGetCommitLog(SvnClientBeforeCommitEventArgs^ e);
		void HandleClientNotify(SvnClientNotifyEventArgs^ e);
		void HandleClientConflictResolver(SvnClientConflictResolveEventArgs^ e);

		const char* GetEolPtr(SvnEolStyle style);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Checkout Client Command

		/// <summary>Performs a recursive checkout of <paramref name="url" /> to <paramref name="path" /></summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void CheckOut(SvnUriTarget^ url, String^ path);
		/// <summary>Performs a recursive checkout of <paramref name="url" /> to <paramref name="path" /></summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void CheckOut(SvnUriTarget^ url, String^ path, [Out] __int64% revision);

		/// <summary>Performs a checkout of <paramref name="url" /> to <paramref name="path" /> to the specified param</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args);
		/// <summary>Performs a checkout of <paramref name="url" /> to <paramref name="path" /> to the specified param</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args, [Out] __int64% revision);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Update Client Command

		/// <summary>Recursively updates the specified path to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Update(String^ path);
		/// <summary>Recursively updates the specified path to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Update(String^ path, [Out] __int64% revision);
		/// <summary>Recursively updates the specified path</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(String^ path, SvnUpdateArgs^ args);		
		/// <summary>Recursively updates the specified path to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(String^ path, SvnUpdateArgs^ args, [Out] __int64% revision);

		/// <summary>Recursively updates the specified paths to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Update(ICollection<String^>^ paths);
		/// <summary>Recursively updates the specified paths to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Update(ICollection<String^>^ paths, [Out] IList<__int64>^% revisions);
		/// <summary>Updates the specified paths to the specified revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool Update(ICollection<String^>^ paths, SvnUpdateArgs^ args);
		/// <summary>Updates the specified paths to the specified revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool Update(ICollection<String^>^ paths, SvnUpdateArgs^ args, [Out] IList<__int64>^% revisions);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Export Client Command
		/// <summary>Recursively exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		void Export(SvnTarget^ from, String^ toPath);
		/// <summary>Recursively exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		void Export(SvnTarget^ from, String^ toPath, [Out] __int64% revision);
		/// <summary>Recursively exports the specified target to the specified path, optionally overwriting existing files</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		void Export(SvnTarget^ from, String^ toPath, bool overwrite);
		/// <summary>Recursively exports the specified target to the specified path, optionally overwriting existing files</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		void Export(SvnTarget^ from, String^ toPath, bool overwrite, [Out] __int64% revision);
		/// <summary>Exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		bool Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args);
		/// <summary>Exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		bool Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args, [Out] __int64% revision);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Switch Client Command

		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Switch(String^ path, SvnUriTarget^ target);

		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Switch(String^ path, SvnUriTarget^ target, [Out] __int64% revision);

		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args);

		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args, [Out] __int64% revision);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Add Client Command
		/// <summary>Recursively adds the specified path</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Add(String^ path);

		/// <summary>Adds the specified path</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Add(String^ path, SvnDepth depth);

		/// <summary>Cleans up the specified path, removing all workingcopy locks left behind by crashed clients</summary>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Add(String^ path, SvnAddArgs^ args);
#pragma endregion

	internal:
		generic<typename T>
		where T : SvnClientEventArgs
		ref class InfoItemList : public System::Collections::ObjectModel::Collection<T>
		{
		public:
			InfoItemList()
			{}

		internal:
			void HandleItem(Object^ sender, T e)
			{
				UNUSED_ALWAYS(sender);
				e->Detach(true);
				Add(e);
			}

			property EventHandler<T>^ Handler
			{
				EventHandler<T>^ get()
				{
					return gcnew EventHandler<T>(this, &InfoItemList<T>::HandleItem);
				}
			}
		};
	public:
		/////////////////////////////////////////
#pragma region // Status Client Command
		/// <summary>Recursively gets 'interesting' status data for the specified path</summary>
		void Status(String^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler);
		/// <summary>Gets status data for the specified path</summary>
		bool Status(String^ path, SvnStatusArgs^ args, EventHandler<SvnStatusEventArgs^>^ statusHandler);

		/// <summary>Recursively gets a list of 'interesting' status data for the specified path</summary>
		void GetStatus(String^ path, [Out] IList<SvnStatusEventArgs^>^% statuses);
		/// <summary>Gets the status data for the specified path</summary>
		void GetStatus(String^ path, [Out] SvnStatusEventArgs^% status);
		/// <summary>Gets a list of status data for the specified path</summary>
		bool GetStatus(String^ path, SvnStatusArgs^ args, [Out] IList<SvnStatusEventArgs^>^% statuses);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Log Client Command
		/// <summary>Gets log messages of the specified target</summary>
		void Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler);
		/// <summary>Gets log messages of the specified target</summary>
		bool Log(SvnTarget^ target, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);
		/// <summary>Gets log messages of the specified target</summary>
		void Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, EventHandler<SvnLogEventArgs^>^ logHandler);
		/// <summary>Gets log messages of the specified target</summary>
		bool Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <summary>Gets log messages of the specified target</summary>
		void GetLog(SvnTarget^ target, [Out] IList<SvnLogEventArgs^>^% logItems);
		/// <summary>Gets log messages of the specified target</summary>
		bool GetLog(SvnTarget^ target, SvnLogArgs^ args, [Out] IList<SvnLogEventArgs^>^% logItems);
		/// <summary>Gets log messages of the specified target</summary>
		void GetLog(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, [Out] IList<SvnLogEventArgs^>^% logItems);
		/// <summary>Gets log messages of the specified target</summary>
		bool GetLog(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, SvnLogArgs^ args, [Out] IList<SvnLogEventArgs^>^% logItems);

	private:
		bool InternalLog(ICollection<String^>^ paths, SvnRevision^ pegRevision, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Cat Client Command
		void Cat(SvnTarget^ target, Stream^ toStream);
		bool Cat(SvnTarget^ target, Stream^ toStream, SvnCatArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // List Client Command

		void List(SvnTarget^ target, EventHandler<SvnListEventArgs^>^ listHandler);
		bool List(SvnTarget^ target, SvnListArgs^ args, EventHandler<SvnListEventArgs^>^ listHandler);

		void GetList(SvnTarget^ target, [Out] IList<SvnListEventArgs^>^% list);
		bool GetList(SvnTarget^ target, SvnListArgs^ args, [Out] IList<SvnListEventArgs^>^% list);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Info Client Command
		/// <summary>Gets information about the specified target</summary>
		void Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler);
		/// <summary>Gets information about the specified target</summary>
		bool Info(SvnTarget^ target, SvnInfoArgs^ args, EventHandler<SvnInfoEventArgs^>^ infoHandler);

		/// <summary>Gets information about the specified target</summary>
		void GetInfo(SvnTarget^ target, [Out] SvnInfoEventArgs^% info);
		/// <summary>Gets information about the specified target</summary>
		bool GetInfo(SvnTarget^ target, SvnInfoArgs^ args, [Out] IList<SvnInfoEventArgs^>^% info);

#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // MkDir Client Command

		void MkDir(String^ path);
		bool MkDir(String^ path, SvnMkDirArgs^ args);

		void MkDir(ICollection<String^>^ paths);
		bool MkDir(ICollection<String^>^ paths, SvnMkDirArgs^ args);

		void RemoteMkDir(Uri^ uri);
		bool RemoteMkDir(Uri^ uri, SvnMkDirArgs^ args);
		bool RemoteMkDir(Uri^ uri, SvnMkDirArgs^ args, [Out] SvnCommitInfo^% commitInfo);
		bool RemoteMkDir(ICollection<Uri^>^ uris, SvnMkDirArgs^ args);
		bool RemoteMkDir(ICollection<Uri^>^ uris, SvnMkDirArgs^ args, [Out] SvnCommitInfo^% commitInfo);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // MkDir Client Command

		void Delete(String^ path);
		bool Delete(String^ path, SvnDeleteArgs^ args);

		void Delete(ICollection<String^>^ paths);
		bool Delete(ICollection<String^>^ paths, SvnDeleteArgs^ args);

		void RemoteDelete(Uri^ uri);
		bool RemoteDelete(Uri^ uri, SvnDeleteArgs^ args);
		bool RemoteDelete(Uri^ uri, SvnDeleteArgs^ args, [Out] SvnCommitInfo^% commitInfo);
		void RemoteDelete(ICollection<Uri^>^ uris);
		bool RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args);
		bool RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args, [Out] SvnCommitInfo^% commitInfo);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Import Client Command

		/// <summary>Performs a working copy import to the specified Uri, 
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		void Import(String^ path, Uri^ target);
		/// <summary>Performs a working copy import to the specified Uri, 
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		void Import(String^ path, Uri^ target, [Out] SvnCommitInfo^% commitInfo);
		/// <summary>Performs a working copy import to the specified Uri, 
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		bool Import(String^ path, Uri^ target, SvnImportArgs^ args);
		/// <summary>Performs a working copy import to the specified Uri, 
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		bool Import(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitInfo^% commitInfo);

		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		void RemoteImport(String^ path, Uri^ target);
		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		void RemoteImport(String^ path, Uri^ target, [Out] SvnCommitInfo^% commitInfo);
		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		bool RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args);
		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		bool RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitInfo^% commitInfo);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Commit Client Command
		void Commit(String^ path);
		void Commit(String^ path, [Out] SvnCommitInfo^% commitInfo);
		void Commit(ICollection<String^>^ paths);
		void Commit(ICollection<String^>^ paths, [Out] SvnCommitInfo^% commitInfo);
		bool Commit(String^ path, SvnCommitArgs^ args);
		bool Commit(String^ path, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo);
		bool Commit(ICollection<String^>^ paths, SvnCommitArgs^ args);
		bool Commit(ICollection<String^>^ paths, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // Revert Client Command
		void Revert(String^ path);
		bool Revert(String^ path, SvnRevertArgs^ args);
		void Revert(ICollection<String^>^ paths);
		bool Revert(ICollection<String^>^ paths, SvnRevertArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Resolved Client Command
		void Resolved(String^ path);
		void Resolved(String^ path, SvnAccept accept);
		bool Resolved(String^ path, SvnResolvedArgs^ args);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // Cleanup Client Command

		/// <summary>Cleans up the specified path, removing all workingcopy locks left behind by crashed clients</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void CleanUp(String^ path);
		/// <summary>Cleans up the specified path, removing all workingcopy locks left behind by crashed clients</summary>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool CleanUp(String^ path, SvnCleanUpArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Copy Client Command
		void Copy(SvnTarget^ sourceTarget, String^ toPath);
		void Copy(ICollection<SvnTarget^>^ sourceTargets, String^ toPath);
		bool Copy(SvnTarget^ sourceTarget, String^ toPath, SvnCopyArgs^ args);
		bool Copy(ICollection<SvnTarget^>^ sourceTargets, String^ toPath, SvnCopyArgs^ args);
		void RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri);
		void RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri);
		void RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo);
		void RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo);
		bool RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args);
		bool RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args);
		bool RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitInfo^% commitInfo);
		bool RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitInfo^% commitInfo);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Move Client Command
		void Move(String^ sourcePath, String^ toPath);
		void Move(ICollection<String^>^ sourcePaths, String^ toPath);
		bool Move(String^ sourcePath, String^ toPath, SvnMoveArgs^ args);
		bool Move(ICollection<String^>^ sourcePaths, String^ toPath, SvnMoveArgs^ args);
		void RemoteMove(Uri^ sourceUri, Uri^ toUri);
		void RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri);
		void RemoteMove(Uri^ sourceUri, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo);
		void RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo);
		bool RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args);
		bool RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args);
		bool RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitInfo^% commitInfo);
		bool RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitInfo^% commitInfo);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Lock Client Command
		void Lock(String^ target, String^ comment);
		void Lock(Uri^ target, String^ comment);
		void Lock(ICollection<String^>^ targets, String^ comment);
		void Lock(ICollection<Uri^>^ targets, String^ comment);
		bool Lock(String^ target, SvnLockArgs^ args);
		bool Lock(Uri^ target, SvnLockArgs^ args);
		bool Lock(ICollection<String^>^ targets, SvnLockArgs^ args);
		bool Lock(ICollection<Uri^>^ targets, SvnLockArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Lock Client Command
		void Unlock(String^ target);
		void Unlock(Uri^ target);
		void Unlock(ICollection<String^>^ targets);
		void Unlock(ICollection<Uri^>^ targets);
		bool Unlock(String^ target, SvnUnlockArgs^ args);
		bool Unlock(Uri^ target, SvnUnlockArgs^ args);
		bool Unlock(ICollection<Uri^>^ targets, SvnUnlockArgs^ args);
		bool Unlock(ICollection<String^>^ targets, SvnUnlockArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // SetProperty Client Command
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		void SetProperty(String^ path, String^ propertyName, String^ value);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		void SetProperty(String^ path, String^ propertyName, IList<char>^ bytes);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(String^ path, String^ propertyName, String^ value, SvnSetPropertyArgs^ args);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(String^ path, String^ propertyName, IList<char>^ bytes, SvnSetPropertyArgs^ args);
		/// <summary>Removes the specified property from the specfied path</summary>
		void DeleteProperty(String^ path, String^ propertyName);
		/// <summary>Removes the specified property from the specfied path</summary>
		bool DeleteProperty(String^ path, String^ propertyName, SvnSetPropertyArgs^ args);

	internal:
		bool InternalSetProperty(String^ path, String^ propertyName, const svn_string_t* value, SvnSetPropertyArgs^ args, AprPool^ pool);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // GetProperty Client Command
		/// <summary>Gets the specified property from the specfied path</summary>
		/// <returns>true if property is set, otherwise false</returns>
		/// <exception type="SvnException">path is not a valid workingcopy path</exception>
		void GetProperty(SvnTarget^ target, String^ propertyName, [Out] String^% value);
		/// <summary>Gets the specified property from the specfied path</summary>
		/// <returns>true if property is set, otherwise false</returns>
		/// <exception type="SvnException">path is not a valid workingcopy path</exception>
		void GetProperty(SvnTarget^ target, String^ propertyName, [Out] IList<char>^% bytes);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool GetProperty(SvnTarget^ target, String^ propertyName, SvnGetPropertyArgs^ args, [Out] IDictionary<SvnTarget^, String^>^% properties);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool GetProperty(SvnTarget^ target, String^ propertyName, SvnGetPropertyArgs^ args, [Out] IDictionary<SvnTarget^, IList<char>^>^% properties);

		/// <summary>Tries to get a property from the specified path</summary>
		/// <remarks>Eats all (non-argument) exceptions</remarks>
		/// <returns>True if the property is fetched, otherwise false</returns>
		bool TryGetProperty(SvnTarget^ target, String^ propertyName, [Out] String^% value);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Properties List Client Command
		void PropertyList(SvnTarget^ target, EventHandler<SvnPropertyListEventArgs^>^ listHandler);
		bool PropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, EventHandler<SvnPropertyListEventArgs^>^ listHandler);

		void GetPropertyList(SvnTarget^ target, [Out] IList<SvnPropertyListEventArgs^>^% list);
		bool GetPropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, [Out] IList<SvnPropertyListEventArgs^>^% list);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // GetProperty Client Command
		/// <summary>
		/// Merges the changes from <paramref name="mergeFrom" /> to <paramref name="mergeTo" /> into <paramRef name="targetPath" />
		/// </summary>
		void Merge(SvnTarget^ mergeFrom, SvnTarget^ mergeTo, String^ targetPath);
		/// <summary>
		/// Merges the changes from <paramref name="mergeFrom" /> to <paramref name="mergeTo" /> into <paramRef name="targetPath" />
		/// </summary>
		bool Merge(SvnTarget^ mergeFrom, SvnTarget^ mergeTo, String^ targetPath, SvnMergeArgs^ args);

		void Merge(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, String^ targetPath);
		bool Merge(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, String^ targetPath, SvnMergeArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Diff Client Command
		void Diff(SvnTarget^ from, SvnTarget^ to, [Out]FileStream^% result);
		bool Diff(SvnTarget^ from, SvnTarget^ to, SvnDiffArgs^ args, [Out]FileStream^% result);
		void Diff(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, [Out]FileStream^% result);
		bool Diff(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, SvnDiffArgs^ args, [Out]FileStream^% result);
#pragma endregion

public:
		/////////////////////////////////////////
#pragma region // Diff Summary Command
		void DiffSummary(SvnTarget^ from, SvnTarget^ to, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler);
		bool DiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler);
		void GetDiffSummary(SvnTarget^ from, SvnTarget^ to, [Out] IList<SvnDiffSummaryEventArgs^>^% list);
		bool GetDiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, [Out] IList<SvnDiffSummaryEventArgs^>^% list);
#pragma endregion


	public:
		void AddToChangeList(String^ path, String^ changeList);
		bool AddToChangeList(String^ path, String^ changeList, SvnAddToChangeListArgs^ args);
		void AddToChangeList(ICollection<String^>^ paths, String^ changeList);
		bool AddToChangeList(ICollection<String^>^ paths, String^ changeList, SvnAddToChangeListArgs^ args);
		void RemoveFromChangeList(String^ path, String^ changeList);
		bool RemoveFromChangeList(String^ path, String^ changeList, SvnRemoveFromChangeListArgs^ args);
		void RemoveFromChangeList(ICollection<String^>^ paths, String^ changeList);
		bool RemoveFromChangeList(ICollection<String^>^ paths, String^ changeList, SvnRemoveFromChangeListArgs^ args);
		void ListChangeList(String^ changeList, String^ rootPath, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler);
		bool ListChangeList(String^ changeList, String^ rootPath, SvnListChangeListArgs^ args, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler);

		void GetChangeList(String^ changeList, String^ rootPath, [Out]IList<String^>^% list);
		bool GetChangeList(String^ changeList, String^ rootPath, SvnListChangeListArgs^ args, [Out]IList<String^>^% list);
		void GetChangeList(String^ changeList, String^ rootPath, [Out]IList<SvnListChangeListEventArgs^>^% list);
		bool GetChangeList(String^ changeList, String^ rootPath, SvnListChangeListArgs^ args, [Out]IList<SvnListChangeListEventArgs^>^% list);

	public:
		void GetMergeInfo(SvnTarget ^target, [Out]SvnMergeInfo^% mergeInfo);
		bool GetMergeInfo(SvnTarget ^target, SvnGetMergeInfoArgs^ args, [Out]SvnMergeInfo^% mergeInfo);
		bool TryGetMergeInfo(SvnTarget ^target, [Out]SvnMergeInfo^% mergeInfo);

	public:
		/// <summary>Gets the repository Uri of a path, or <c>null</c> if path is not versioned</summary>
		Uri^ GetUriFromWorkingCopy(String^ path);

		/// <summary>Gets the Uuid of a Uri, or <see cref="Guid::Empty" /> if path is not versioned</summary>
		/// <returns>true if successfull, otherwise false</returns>
		bool GetUuidFromUri(Uri^ uri, [Out] Guid% uuid);

	private:
		~SvnClient();
	};
}
