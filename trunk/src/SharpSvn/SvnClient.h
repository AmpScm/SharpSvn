
#pragma once

#include "SvnClientContext.h"
#include "SvnUriTarget.h"
#include "SvnPathTarget.h"
#include "SvnClientEventArgs.h"
#include "SvnCommitItem.h"
#include "SvnMergeInfo.h"
#include "AprBaton.h"

#include "SvnClientConfiguration.h"

namespace SharpSvn {

	ref class SvnCommittingEventArgs;
	ref class SvnCancelEventArgs;
	ref class SvnProgressEventArgs;
	ref class SvnLogEventArgs;
	ref class SvnNotifyEventArgs;

	ref class SvnClientArgs;
	ref class SvnClientArgsWithCommit;
	ref class SvnClientArgsWithConflict;
	ref class SvnCleanUpArgs;
	ref class SvnCheckOutArgs;
	ref class SvnUpdateArgs;
	ref class SvnExportArgs;
	ref class SvnSwitchArgs;
	ref class SvnRelocateArgs;
	ref class SvnAddArgs;
	ref class SvnCommitArgs;
	ref class SvnStatusArgs;
	ref class SvnLogArgs;
	ref class SvnInfoArgs;
	ref class SvnCreateDirectoryArgs;
	ref class SvnDeleteArgs;
	ref class SvnImportArgs;
	ref class SvnListArgs;
	ref class SvnRevertArgs;
	ref class SvnResolvedArgs;
	ref class SvnCopyArgs;
	ref class SvnMoveArgs;
	ref class SvnSetPropertyArgs;
	ref class SvnGetPropertyArgs;
	ref class SvnPropertyListArgs;
	ref class SvnSetRevisionPropertyArgs;
	ref class SvnGetRevisionPropertyArgs;
	ref class SvnRevisionPropertyListArgs;
	ref class SvnLockArgs;
	ref class SvnUnlockArgs;
	ref class SvnWriteArgs;
	ref class SvnMergeArgs;
	ref class SvnDiffArgs;
	ref class SvnDiffSummaryArgs;
	ref class SvnBlameArgs;
	ref class SvnAddToChangeListArgs;
	ref class SvnRemoveFromChangeListArgs;
	ref class SvnListChangeListArgs;
	ref class SvnGetSuggestedMergeSourcesArgs;
	ref class SvnGetAppliedMergeInfoArgs;
	ref class SvnGetAvailableMergeInfoArgs;
	ref class SvnCreateRepositoryArgs;
	ref class SvnDeleteRepositoryArgs;
	ref class SvnRecoverRepositoryArgs;

	using System::Runtime::InteropServices::GCHandle;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;
	using System::IO::Stream;
	using System::IO::FileStream;

	/// <summary>Subversion client instance; main entrance to Subversion api</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClient : public SvnClientContext
	{
		initonly SharpSvn::Apr::AprBaton<SvnClient^>^ _clientBatton;
		AprPool _pool;
		SvnClientArgs^ _currentArgs;
	internal:
		bool _noLogMessageRequired;
	internal:
		property SvnClientArgs^ CurrentArgs
		{
			SvnClientArgs^ get()
			{
				return _currentArgs;
			}
		}

	public:
		///<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
		SvnClient();

	private:
		static initonly String^ _administrativeDirName = Utf8_PtrToString(_admDir);
		void Initialize();

	public:
		/// <summary>Gets the version number of SharpSvn's encapsulated subversion library</summary>
		property static Version^ Version
		{
			System::Version^ get();
		}

		/// <summary>Gets the version of the SharpSvn library</summary>
		property static System::Version^ SharpSvnVersion
		{
			System::Version^ get();
		}

		/// <summary>
		/// Gets the name of the subversion administrative directories. Most commonly ".svn"
		/// </summary>
		property static String^ AdministrativeDirectoryName
		{
			String^ get()
			{
				return _administrativeDirName;
			}
		}

	private:
		SvnClientConfiguration^ _config;

	public:
		/// <summary>Gets the <see cref="SvnClientConfiguration" /> instance of this <see cref="SvnClient"/></summary>
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
	public:
		/// <summary>
		/// Raised to allow canceling operations. The event is first 
		/// raised on the <see cref="SvnClientArgs" /> object and 
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnCancelEventArgs^>^		Cancel;
		/// <summary>
		/// Raised on progress. The event is first 
		/// raised on the <see cref="SvnClientArgs" /> object and 
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnProgressEventArgs^>^		Progress;
		/// <summary>
		/// Raised on notifications. The event is first 
		/// raised on the <see cref="SvnClientArgs" /> object and 
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnNotifyEventArgs^>^		Notify;
		/// <summary>
		/// Raised on progress. The event is first 
		/// raised on the <see cref="SvnClientArgsWithCommit" /> object and 
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnCommittingEventArgs^>^	Committing;
		/// <summary>
		/// Raised on progress. The event is first 
		/// raised on the <see cref="SvnClientArgsWithConflict" /> object and 
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnConflictEventArgs^>^		Conflict;

		/// <summary>
		/// Raised when a subversion exception occurs.
		/// Set <see cref="SvnErrorEventArgs::Cancel" /> to true to cancel 
		/// throwing the exception
		/// </summary>
		event EventHandler<SvnErrorEventArgs^>^		SvnError;

	protected:
		/// <summary>Invokes the <see cref="Cancel" /> event</summary>
		virtual void OnCancel(SvnCancelEventArgs^ e);
		/// <summary>Invokes the <see cref="Progress" /> event</summary>
		virtual void OnProgress(SvnProgressEventArgs^ e);
		/// <summary>Invokes the <see cref="Committing" /> event</summary>
		virtual void OnCommitting(SvnCommittingEventArgs^ e);
		/// <summary>Invokes the <see cref="Notify" /> event</summary>
		virtual void OnNotify(SvnNotifyEventArgs^ e);
		/// <summary>Invokes the <see cref="Conflict" /> event</summary>
		virtual void OnConflict(SvnConflictEventArgs^ e);
		/// <summary>Invokes the <see cref="Exception" /> event</summary>
		virtual void OnSvnError(SvnErrorEventArgs^ e);

	internal:
		void HandleClientCancel(SvnCancelEventArgs^ e);
		void HandleClientProgress(SvnProgressEventArgs^ e);
		void HandleClientGetCommitLog(SvnCommittingEventArgs^ e);
		void HandleClientNotify(SvnNotifyEventArgs^ e);
		void HandleClientConflictResolver(SvnConflictEventArgs^ e);
		void HandleClientError(SvnErrorEventArgs^ e);

		static const char* GetEolPtr(SvnLineStyle style);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Checkout Client Command

		/// <overloads>Check out a working copy from a repository. (<c>svn checkout</c>)</overloads>
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
		/// <overloads>Bring changes from the repository into the working copy (<c>svn update</c>)</overloads>
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
		/// <overloads>Create an unversioned copy of a tree (<c>svn export</c>)</overloads>
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
		/// <overloads>Update the working copy to a different URL (<c>svn switch</c>)</overloads>
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

		/////////////////////////////////////////
#pragma region // Relocate Client Command
		/// <overloads>Update the working copy to a different repository (<c>svn switch --relocate</c>)</overloads>
		void Relocate(String^ path, Uri^ from, Uri^ to);

		bool Relocate(String^ path, Uri^ from, Uri^ to, SvnRelocateArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Add Client Command
		/// <overloads>Put files and directories under version control, scheduling them for addition to repository.
		/// They will be added in next commit(<c>svn add</c>)</overloads>
		/// <summary>Recursively adds the specified path</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Add(String^ path);

		/// <summary>Adds the specified path</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		void Add(String^ path, SvnDepth depth);

		/// <summary>Adds the specified path</summary>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Add(String^ path, SvnAddArgs^ args);
#pragma endregion

	internal:
		generic<typename T>
		where T : SvnEventArgs
		ref class InfoItemCollection : public System::Collections::ObjectModel::Collection<T>
		{
		public:
			InfoItemCollection()
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
					return gcnew EventHandler<T>(this, &InfoItemCollection<T>::HandleItem);
				}
			}
		};
	public:
		/////////////////////////////////////////
#pragma region // Status Client Command
		/// <overloads>Retrieve the status of working copy files and directories (<c>svn status</c>)</overloads>
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
		/// <overloads>Streamingly retrieve the log messages for a set of revision(s) and/or file(s). (<c>svn log</c>)</overloads>
		/// <summary>Gets log messages of the specified target</summary>
		void Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler);
		/// <summary>Gets log messages of the specified target</summary>
		bool Log(SvnTarget^ target, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <summary>Gets log messages of the specified target</summary>
		void Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, EventHandler<SvnLogEventArgs^>^ logHandler);
		/// <summary>Gets log messages of the specified target</summary>
		bool Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <overloads>Retrieve the log messages for a set of revision(s) and/or file(s). (<c>svn log</c>)</overloads>
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
#pragma region // Write Command
		/// <overloads>Writes the content of specified files or URLs to a stream. (<c>svn cat</c>)</overloads>
		void Write(SvnTarget^ target, Stream^ toStream);
		bool Write(SvnTarget^ target, Stream^ toStream, SvnWriteArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // List Client Command
		/// <overloads>Streamingly lists directory entries in the repository. (<c>svn list</c>)</overloads>
		void List(SvnTarget^ target, EventHandler<SvnListEventArgs^>^ listHandler);
		bool List(SvnTarget^ target, SvnListArgs^ args, EventHandler<SvnListEventArgs^>^ listHandler);

		/// <overloads>Gets a list of directory entries in the repository. (<c>svn list</c>)</overloads>
		void GetList(SvnTarget^ target, [Out] IList<SvnListEventArgs^>^% list);
		bool GetList(SvnTarget^ target, SvnListArgs^ args, [Out] IList<SvnListEventArgs^>^% list);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Info Client Command
		/// <overloads>Streamingly retrieves information about a local or remote item (<c>svn info</c>)</overloads>
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
#pragma region // CreateDirectory Client Command

		/// <overloads>Create a new directory under version control, scheduling (<c>svn mkdir</c>)</overloads>
		void CreateDirectory(String^ path);
		bool CreateDirectory(String^ path, SvnCreateDirectoryArgs^ args);

		void CreateDirectory(ICollection<String^>^ paths);
		bool CreateDirectory(ICollection<String^>^ paths, SvnCreateDirectoryArgs^ args);

		/// <overloads>Create a new directory under version control at the repository (<c>svn mkdir</c>)</overloads>
		void RemoteCreateDirectory(Uri^ uri);
		bool RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args);
		bool RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args, [Out] SvnCommitInfo^% commitInfo);
		bool RemoteCreateDirectory(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args);
		bool RemoteCreateDirectory(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args, [Out] SvnCommitInfo^% commitInfo);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // CreateDirectory Client Command

		/// <overloads>Remove files and directories from version control, scheduling (<c>svn delete|remove</c>)</overloads>
		void Delete(String^ path);
		bool Delete(String^ path, SvnDeleteArgs^ args);

		void Delete(ICollection<String^>^ paths);
		bool Delete(ICollection<String^>^ paths, SvnDeleteArgs^ args);

		/// <overloads>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</overloads>
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

		/// <overloads>Commit an unversioned file or tree into the repository (<c>svn import</c> followed by <c>svn checkout</c>)</overloads>
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

		/// <overloads>Commit an unversioned file or tree into the repository (<c>svn import</c>)</overloads>
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
		/// <overloads>Send changes from your working copy to the repository (<c>svn commit</c>)</overloads>
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
		/// <overloads>Restore pristine working copy file (undo most local edits) (<c>svn revert</c>)</overloads>
		void Revert(String^ path);
		bool Revert(String^ path, SvnRevertArgs^ args);
		void Revert(ICollection<String^>^ paths);
		bool Revert(ICollection<String^>^ paths, SvnRevertArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Resolved Client Command
		/// <overloads>Remove 'conflicted' state on working copy files or directories (<c>svn resolved</c>)</overloads>
		void Resolved(String^ path);
		void Resolved(String^ path, SvnConflictResult which);
		bool Resolved(String^ path, SvnResolvedArgs^ args);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // Cleanup Client Command
		/// <overloads> Recursively clean up the working copy, removing locks, resuming unfinished operations, etc.(<c>svn cleanup</c>)</overloads>
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
		/// <overloads>Duplicate something in working copy, remembering history (<c>svn copy</c>)</overloads>
		void Copy(SvnTarget^ sourceTarget, String^ toPath);
		void Copy(ICollection<SvnTarget^>^ sourceTargets, String^ toPath);
		bool Copy(SvnTarget^ sourceTarget, String^ toPath, SvnCopyArgs^ args);
		bool Copy(ICollection<SvnTarget^>^ sourceTargets, String^ toPath, SvnCopyArgs^ args);
		/// <overloads>Duplicate something in repository, remembering history (<c>svn copy</c>)</overloads>
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
		/// <overloads>Move and/or rename something in working copy, remembering history (<c>svn move</c>)</overloads>
		void Move(String^ sourcePath, String^ toPath);
		void Move(ICollection<String^>^ sourcePaths, String^ toPath);
		bool Move(String^ sourcePath, String^ toPath, SvnMoveArgs^ args);
		bool Move(ICollection<String^>^ sourcePaths, String^ toPath, SvnMoveArgs^ args);
		/// <overloads>Move and/or rename something in repository, remembering history (<c>svn move</c>)</overloads>
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
		/// <overloads>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</overloads>
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
		/// <overloads>Unlock working copy paths or URLs (<c>svn unlock</c>)</overloads>
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
		/// <overloads>Set the value of a property on files, dirs (<c>svn propset</c>)</overloads>
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

		/// <overloads>Deletes a property from files, dirs (<c>svn propdel</c>)</overloads>
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
		/// <overloads>Retrieves the value of a property on files, dirs, or revisions (<c>svn propget</c>)</overloads>
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

		/// <summary>Tries to get a property from the specified path (<c>svn propget</c>)</summary>
		/// <remarks>Eats all (non-argument) exceptions</remarks>
		/// <returns>True if the property is fetched, otherwise false</returns>
		/// <remarks>Equivalent to GetProperty with <see cref="SvnGetPropertyArgs" />'s ThrowOnError set to false</remarks>
		bool TryGetProperty(SvnTarget^ target, String^ propertyName, [Out] String^% value);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Properties List Client Command
		/// <overloads>Streamingly lists all properties on files or dirs (<c>svn proplist</c>)</overloads>
		void PropertyList(SvnTarget^ target, EventHandler<SvnPropertyListEventArgs^>^ listHandler);
		bool PropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, EventHandler<SvnPropertyListEventArgs^>^ listHandler);

		/// <overloads>Gets all properties on files or dirs (<c>svn proplist</c>)</overloads>
		void GetPropertyList(SvnTarget^ target, [Out] IList<SvnPropertyListEventArgs^>^% list);
		bool GetPropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, [Out] IList<SvnPropertyListEventArgs^>^% list);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // SetRevisionProperty Client Command
		/// <overloads>Sets the value of a revision property on files, dirs in a specific revision (<c>svn propset --revision</c>)</overloads>
		void SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, String^ value);
		void SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, IList<char>^ bytes);
		bool SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args, String^ value);
		bool SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args, IList<char>^ bytes);

		/// <overloads>Deletes the value of a revision property on files, dirs in a specific revision(<c>svn propdel --revision</c>)</overloads>
		void DeleteRevisionProperty(SvnUriTarget^ target, String^ propertyName);
		bool DeleteRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args);

	internal:
		bool InternalSetRevisionProperty(SvnUriTarget^ target, String^ propertyName, const svn_string_t* value, SvnSetRevisionPropertyArgs^ args, AprPool^ pool);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // GetRevisionProperty Client Command
		/// <overloads>Gets the value of a revision property on files or dirs in a specific revision (<c>svn propget --revision</c>)</overloads>
		void GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] String^% value);
		void GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] IList<char>^% value);
		bool GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] String^% value);
		bool GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] IList<char>^% value);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Properties List Client Command
		/// <overloads>Gets all revision properties on a revision on the specified target (<c>svn proplist --revision</c>)</overloads>
		void GetRevisionPropertyList(SvnUriTarget^ target, [Out] IDictionary<String^, Object^>^% list);
		bool GetRevisionPropertyList(SvnUriTarget^ target, SvnRevisionPropertyListArgs^ args, [Out] IDictionary<String^, Object^>^% list);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // Merge Client Command
		/// <overloads>Merges the differences between two sources to a working copy path (<c>svn merge</c>)</overloads>
		/// <summary>
		/// Merges the changes from <paramref name="mergeFrom" /> to <paramref name="mergeTo" /> into <paramRef name="targetPath" />
		/// </summary>
		void Merge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo);
		/// <summary>
		/// Merges the changes from <paramref name="mergeFrom" /> to <paramref name="mergeTo" /> into <paramRef name="targetPath" />
		/// </summary>
		bool Merge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo, SvnMergeArgs^ args);

		void Merge(String^ targetPath, SvnTarget^ source, SvnMergeRange^ mergeRange);
		void Merge(String^ targetPath, SvnTarget^ source, ICollection<SvnMergeRange^>^ mergeRange);
		bool Merge(String^ targetPath, SvnTarget^ source, SvnMergeRange^ mergeRange, SvnMergeArgs^ args);
		bool Merge(String^ targetPath, SvnTarget^ source, ICollection<SvnMergeRange^>^ mergeRange, SvnMergeArgs^ args);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Diff Client Command
		/// <overloads>Retrieves the differences between two revisions or paths (<c>svn diff</c>)</overloads>
		void Diff(SvnTarget^ from, SvnTarget^ to, [Out]FileStream^% result);
		bool Diff(SvnTarget^ from, SvnTarget^ to, SvnDiffArgs^ args, [Out]FileStream^% result);
		void Diff(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, [Out]FileStream^% result);
		bool Diff(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, SvnDiffArgs^ args, [Out]FileStream^% result);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Diff Summary Command
		/// <overloads>Streamingly retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</overloads>
		void DiffSummary(SvnTarget^ from, SvnTarget^ to, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler);
		bool DiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler);
		/// <overloads>Retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</overloads>
		void GetDiffSummary(SvnTarget^ from, SvnTarget^ to, [Out] IList<SvnDiffSummaryEventArgs^>^% list);
		bool GetDiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, [Out] IList<SvnDiffSummaryEventArgs^>^% list);
#pragma endregion


	public:
		/// <overloads>Associate the specified path(s) with the specified changelist (<c>svn changelist</c>)</overloads>
		void AddToChangeList(String^ path, String^ changeList);
		bool AddToChangeList(String^ path, String^ changeList, SvnAddToChangeListArgs^ args);
		void AddToChangeList(ICollection<String^>^ paths, String^ changeList);
		bool AddToChangeList(ICollection<String^>^ paths, String^ changeList, SvnAddToChangeListArgs^ args);
		/// <overloads>Deassociate the specified path(s) from the specified changelist (<c>svn changelist</c>)</overloads>
		void RemoveFromChangeList(String^ path);
		bool RemoveFromChangeList(String^ path, SvnRemoveFromChangeListArgs^ args);
		void RemoveFromChangeList(ICollection<String^>^ paths);
		bool RemoveFromChangeList(ICollection<String^>^ paths, SvnRemoveFromChangeListArgs^ args);
		/// <overloads>Streamingly gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</overloads>
		void ListChangeList(String^ rootPath, String^ changeList, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler);
		bool ListChangeList(String^ rootPath, String^ changeList, SvnListChangeListArgs^ args, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler);

		/// <overloads>Gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</overloads>
		void GetChangeList(String^ rootPath, String^ changeList, [Out]IList<String^>^% list);
		bool GetChangeList(String^ rootPath, String^ changeList, SvnListChangeListArgs^ args, [Out]IList<String^>^% list);
		void GetChangeList(String^ rootPath, String^ changeList, [Out]IList<SvnListChangeListEventArgs^>^% list);
		bool GetChangeList(String^ rootPath, String^ changeList, SvnListChangeListArgs^ args, [Out]IList<SvnListChangeListEventArgs^>^% list);

	public:
		/// <overloads>Gets a list of Uri's which might be valid merge sources (<c>svn mergeinfo</c>)</overloads>
		/// <summary>Gets a list of Uri's which might be valid merge sources</summary>
		/// <remarks>The list contains copy-from locations and previous merge locations</remarks>
		void GetSuggestedMergeSources(SvnTarget ^target, [Out]IList<Uri^>^% mergeSources);
		/// <summary>Gets a list of Uri's which might be valid merge sources</summary>
		/// <remarks>The list contains copy-from locations and previous merge locations</remarks>
		bool GetSuggestedMergeSources(SvnTarget ^target, SvnGetSuggestedMergeSourcesArgs^ args, [Out]IList<Uri^>^% mergeSources);

	public:
		/// <overloads>Gets the merges which are applied on the specified target (<c>svn mergeinfo</c>)</overloads>
		/// <summary>Gets the merges which are applied on the specified target</summary>
		void GetAppliedMergeInfo(SvnTarget ^target, [Out]SvnAppliedMergeInfo^% mergeInfo);
		/// <summary>Gets the merges which are applied on the specified target</summary>
		bool GetAppliedMergeInfo(SvnTarget ^target, SvnGetAppliedMergeInfoArgs^ args, [Out]SvnAppliedMergeInfo^% mergeInfo);

	public:
		/// <overloads>Gets a list of available merges which can be applied on the specified target (<c>svn mergeinfo</c>)</overloads>
		/// <summary>Gets a list of merges which can be applied on target</summary>
		void GetAvailableMergeInfo(SvnTarget ^target, Uri^ sourceUri, [Out]SvnAvailableMergeInfo^% mergeInfo);
		/// <summary>Gets a list of merges which can be applied on target</summary>
		bool GetAvailableMergeInfo(SvnTarget ^target, Uri^ sourceUri, SvnGetSuggestedMergeSourcesArgs^ args, [Out]SvnAvailableMergeInfo^% mergeInfo);

	public:
		/// <overloads>Streamingly retrieve the content of specified files or URLs with revision and author information per-line (<c>svn blame</c>)</overloads>
		void Blame(SvnTarget^ target, EventHandler<SvnBlameEventArgs^>^ blameHandler);
		bool Blame(SvnTarget^ target, SvnBlameArgs^ args, EventHandler<SvnBlameEventArgs^>^ blameHandler);
		/// <overloads>Retrieve the content of specified files or URLs with revision and author information per-line (<c>svn blame</c>)</overloads>
		void GetBlame(SvnTarget^ target, [Out] IList<SvnBlameEventArgs^>^% list);
		bool GetBlame(SvnTarget^ target, SvnBlameArgs^ args, [Out] IList<SvnBlameEventArgs^>^% list);

	public:
		/// <summary>Gets the repository Uri of a path, or <c>null</c> if path is not versioned</summary>
		Uri^ GetUriFromWorkingCopy(String^ path);

		/// <overloads>Gets the repository root from the specified uri or path</overloads>
		/// <summary>Gets the repository root from the specified uri</summary>
		/// <value>The repository root <see cref="Uri" /> or <c>null</c> if the uri is not a repository uri</value>
		/// <remarks>SharpSvn makes sure the uri ends in a '/'</remarks>
		Uri^ GetRepositoryRoot(Uri^ uri);
		/// <summary>Gets the repository root from the specified path</summary>
		/// <value>The repository root <see cref="Uri" /> or <c>null</c> if the uri is not a working copy path</value>
		/// <remarks>SharpSvn makes sure the uri ends in a '/'</remarks>
		Uri^ GetRepositoryRoot(String^ path);

		/// <summary>Gets the Uuid of a Uri, or <see cref="Guid::Empty" /> if path is not versioned</summary>
		/// <returns>true if successfull, otherwise false</returns>
		bool GetUuidFromUri(Uri^ uri, [Out] Guid% uuid);

	private:
		~SvnClient();

		// Used as auto-dispose class for setting the _currentArgs property
		ref class ArgsStore sealed
		{
			initonly SvnClient^ _client;
		public:
			ArgsStore(SvnClient^ client, SvnClientArgs^ args)
			{
				if(!args)
					throw gcnew ArgumentNullException("args");
				else if(client->_currentArgs)
					throw gcnew InvalidOperationException(SharpSvnStrings::SvnClientOperationInProgress);

				client->_currentArgs = args;
				_client = client;
			}

			~ArgsStore()
			{
				_client->_currentArgs = nullptr;
			}
		};
	};
}
#include "SvnClientArgs.h"
