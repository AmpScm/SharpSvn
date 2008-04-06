// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientContext.h"
#include "SvnUriTarget.h"
#include "SvnPathTarget.h"
#include "SvnClientEventArgs.h"
#include "SvnCommitItem.h"
#include "SvnMergeInfo.h"
#include "AprBaton.h"

#include "SvnClientConfiguration.h"
#include "SvnPropertyValue.h"
#include "SvnMergeSourcesCollection.h"

#include "SvnTools.h"

namespace SharpSvn {

	ref class SvnCommittingEventArgs;
	ref class SvnCancelEventArgs;
	ref class SvnProgressEventArgs;
	ref class SvnLogEventArgs;
	ref class SvnNotifyEventArgs;
	ref class SvnProcessingEventArgs;

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
	ref class SvnResolveArgs;
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
	ref class SvnDiffMergeArgs;
	ref class SvnReintegrationMergeArgs;
	ref class SvnDiffArgs;
	ref class SvnDiffSummaryArgs;
	ref class SvnBlameArgs;
	ref class SvnAddToChangeListArgs;
	ref class SvnRemoveFromChangeListArgs;
	ref class SvnListChangeListArgs;
	ref class SvnGetSuggestedMergeSourcesArgs;
	ref class SvnGetAppliedMergeInfoArgs;
	ref class SvnGetEligableMergeInfoArgs;
	ref class SvnCreateRepositoryArgs;
	ref class SvnDeleteRepositoryArgs;
	ref class SvnRecoverRepositoryArgs;
	ref class SvnGetWorkingCopyStateArgs;

	ref class SvnCommitResult;
	ref class SvnUpdateResult;
	ref class SvnClientReporter;
	ref class SvnWorkingCopyState;

	using System::Runtime::InteropServices::GCHandle;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;
	using System::Collections::ObjectModel::Collection;
	using System::IO::Stream;

	/// <summary>Subversion client instance; main entrance to the Subversion Client api</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClient : public SvnClientContext
	{
		initonly AprBaton<SvnClient^>^ _clientBatton;
		AprPool _pool;
	internal:
		bool _noLogMessageRequired;

	public:
		///<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
		SvnClient();

	private:
		static String^ _administrativeDirName = Utf8_PtrToString(_admDir);
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

		/// <summary>Adds the specified client name to web requests' UserAgent string</summary>
		/// <remarks>The name is filtered to be unique and conformant for the webrequest. Clients should use only alphanumerical ascii characters</remarks>
		static void AddClientName(String^ name, System::Version^ version);


		/// <summary>Enables the subversion localization engine. (Costs +- 400 KByte ram/language used and slows error handling within the subversion engine)</summary>
		/// <remarks>By default the subversion layer uses english messages for everything. After this method is called all messages are routed via
		/// the managed resource handler</remarks>
		static void EnableSubversionLocalization();


		/// <summary>
		/// Gets the name of the subversion administrative directories. Most commonly ".svn"
		/// </summary>
		/// <remarks>
		/// An internal implementation of a setter is available on this property. Users should read
		/// the subversion implementation of svn_wc_set_adm_dir before invoking the setter of this property
		/// via reflection
		/// </remarks>
		property static String^ AdministrativeDirectoryName
		{
			String^ get()
			{
				return _administrativeDirName;
			}
		internal:
			void set(String^ value);
		}

	private:
		SvnClientConfiguration^ _config;

	public:
		/// <summary>Gets the <see cref="SvnClientConfiguration" /> instance of this <see cref="SvnClient"/></summary>
		property SvnClientConfiguration^ Configuration
		{
			SvnClientConfiguration^ get()
			{
				if (!_config)
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
		event EventHandler<SvnCancelEventArgs^>^ Cancel;
		/// <summary>
		/// Raised on progress. The event is first
		/// raised on the <see cref="SvnClientArgs" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnProgressEventArgs^>^ Progress;
		/// <summary>
		/// Raised on notifications. The event is first
		/// raised on the <see cref="SvnClientArgs" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnNotifyEventArgs^>^ Notify;
		/// <summary>
		/// Raised on progress. The event is first
		/// raised on the <see cref="SvnClientArgsWithCommit" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnCommittingEventArgs^>^ Committing;
		/// <summary>
		/// Raised on conflict. The event is first
		/// raised on the <see cref="SvnClientArgsWithConflict" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		event EventHandler<SvnConflictEventArgs^>^ Conflict;

		/// <summary>
		/// Raised when a subversion exception occurs.
		/// Set <see cref="SvnErrorEventArgs::Cancel" /> to true to cancel
		/// throwing the exception
		/// </summary>
		event EventHandler<SvnErrorEventArgs^>^ SvnError;

		/// <summary>
		/// Raised just before a command is executed. This allows a listener
		/// to cleanup before a new command is started
		/// </summary>
		event EventHandler<SvnProcessingEventArgs^>^	Processing;

	protected:
		/// <summary>Raises the <see cref="Cancel" /> event.</summary>
		virtual void OnCancel(SvnCancelEventArgs^ e);
		/// <summary>Raises the <see cref="Progress" /> event.</summary>
		virtual void OnProgress(SvnProgressEventArgs^ e);
		/// <summary>Raises the <see cref="Committing" /> event.</summary>
		virtual void OnCommitting(SvnCommittingEventArgs^ e);
		/// <summary>Raises the <see cref="Notify" /> event.</summary>
		virtual void OnNotify(SvnNotifyEventArgs^ e);
		/// <summary>Raises the <see cref="Conflict" /> event.</summary>
		virtual void OnConflict(SvnConflictEventArgs^ e);
		/// <summary>Raises the <see cref="Exception" /> event.</summary>
		virtual void OnSvnError(SvnErrorEventArgs^ e);
		/// <summary>Raises the <see cref="Processing" /> event.</summary>
		virtual void OnProcessing(SvnProcessingEventArgs^ e);

	internal:
		void HandleClientCancel(SvnCancelEventArgs^ e);
		void HandleClientProgress(SvnProgressEventArgs^ e);
		void HandleClientGetCommitLog(SvnCommittingEventArgs^ e);
		void HandleClientNotify(SvnNotifyEventArgs^ e);
		void HandleClientConflictResolver(SvnConflictEventArgs^ e);
		virtual void HandleClientError(SvnErrorEventArgs^ e) override sealed;
		virtual void HandleProcessing(SvnProcessingEventArgs^ e) override sealed;

		static const char* GetEolPtr(SvnLineStyle style);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Checkout Client Command

		/// <overloads>Check out a working copy from a repository. (<c>svn checkout</c>)</overloads>
		/// <summary>Performs a recursive checkout of <paramref name="url" /> to <paramref name="path" /></summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool CheckOut(SvnUriTarget^ url, String^ path);
		/// <summary>Performs a recursive checkout of <paramref name="url" /> to <paramref name="path" /></summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool CheckOut(SvnUriTarget^ url, String^ path, [Out] SvnUpdateResult^% result);

		/// <summary>Performs a checkout of <paramref name="url" /> to <paramref name="path" /> to the specified param</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args);
		/// <summary>Performs a checkout of <paramref name="url" /> to <paramref name="path" /> to the specified param</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args, [Out] SvnUpdateResult^% result);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Update Client Command
		/// <overloads>Bring changes from the repository into the working copy (<c>svn update</c>)</overloads>
		/// <summary>Recursively updates the specified path to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(String^ path);
		/// <summary>Recursively updates the specified path to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(String^ path, [Out] SvnUpdateResult^% result);
		/// <summary>Recursively updates the specified path</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(String^ path, SvnUpdateArgs^ args);
		/// <summary>Recursively updates the specified path to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(String^ path, SvnUpdateArgs^ args, [Out] SvnUpdateResult^% result);

		/// <summary>Recursively updates the specified paths to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(ICollection<String^>^ paths);
		/// <summary>Recursively updates the specified paths to the latest (HEAD) revision</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Update(ICollection<String^>^ paths, [Out] SvnUpdateResult^% result);
		/// <summary>Updates the specified paths to the specified revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool Update(ICollection<String^>^ paths, SvnUpdateArgs^ args);
		/// <summary>Updates the specified paths to the specified revision</summary>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		bool Update(ICollection<String^>^ paths, SvnUpdateArgs^ args, [Out] SvnUpdateResult^% result);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Export Client Command
		/// <overloads>Create an unversioned copy of a tree (<c>svn export</c>)</overloads>
		/// <summary>Recursively exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		bool Export(SvnTarget^ from, String^ toPath);
		/// <summary>Recursively exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		bool Export(SvnTarget^ from, String^ toPath, [Out] SvnUpdateResult^% result);
		/// <summary>Exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		bool Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args);
		/// <summary>Exports the specified target to the specified path</summary>
		/// <remarks>Subversion optimizes this call if you specify a workingcopy file instead of an url</remarks>
		bool Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args, [Out] SvnUpdateResult^% result);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Switch Client Command
		/// <overloads>Update the working copy to a different URL (<c>svn switch</c>)</overloads>
		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Switch(String^ path, SvnUriTarget^ target);

		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Switch(String^ path, SvnUriTarget^ target, [Out] SvnUpdateResult^% result);

		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args);

		/// <summary>Switches a path recursively to the specified target</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args, [Out] SvnUpdateResult^% result);
#pragma endregion

		/////////////////////////////////////////
#pragma region // Relocate Client Command
		/// <overloads>Update the working copy to a different repository (<c>svn switch --relocate</c>)</overloads>
		/// <summary>Update the working copy to a different repository (<c>svn switch --relocate</c>)</summary>
		bool Relocate(String^ path, Uri^ from, Uri^ to);

		/// <summary>Update the working copy to a different repository (<c>svn switch --relocate</c>)</summary>
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
		bool Add(String^ path);

		/// <summary>Adds the specified path</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Add(String^ path, SvnDepth depth);

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
				e->Detach();
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
		bool Status(String^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler);
		/// <summary>Gets status data for the specified path</summary>
		bool Status(String^ path, SvnStatusArgs^ args, EventHandler<SvnStatusEventArgs^>^ statusHandler);

		/// <summary>Recursively gets a list of 'interesting' status data for the specified path</summary>
		bool GetStatus(String^ path, [Out] Collection<SvnStatusEventArgs^>^% statuses);
		/// <summary>Gets a list of status data for the specified path</summary>
		bool GetStatus(String^ path, SvnStatusArgs^ args, [Out] Collection<SvnStatusEventArgs^>^% statuses);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Log Client Command
		/// <overloads>Streamingly retrieve the log messages for a set of revision(s) and/or file(s). (<c>svn log</c>)</overloads>
		/// <summary>Gets log messages of the specified target</summary>
		bool Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <summary>Gets log messages of the specified target</summary>
		/// <remarks>A pegrevision applied on the target overrides one set on the args object</remarks>
		bool Log(Uri^ target, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <summary>Gets log messages of the specified target</summary>
		/// <remarks>A pegrevision applied on the target overrides one set on the args object</remarks>
		bool Log(String^ targetPath, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <summary>Gets log messages of the specified target path</summary>
		bool Log(ICollection<Uri^>^ targets, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <summary>Gets log messages of the specified target path</summary>
		bool Log(ICollection<String^>^ targetPaths, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);

		/// <overloads>Retrieve the log messages for a set of revision(s) and/or file(s). (<c>svn log</c>)</overloads>
		/// <summary>Gets log messages of the specified target</summary>
		bool GetLog(SvnTarget^ target, [Out] Collection<SvnLogEventArgs^>^% logItems);
		/// <summary>Gets log messages of the specified target</summary>
		bool GetLog(Uri^ target, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems);
		/// <summary>Gets log messages of the specified target</summary>
		bool GetLog(String^ targetPath, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems);
		/// <summary>Gets log messages of the specified target</summary>
		bool GetLog(ICollection<Uri^>^ targets, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems);
		/// <summary>Gets log messages of the specified target</summary>
		bool GetLog(ICollection<String^>^ targetPaths, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems);

	private:
		bool InternalLog(ICollection<String^>^ targets, Uri^ logRoot, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Write Command
		/// <overloads>Writes the content of specified files or URLs to a stream. (<c>svn cat</c>)</overloads>
		/// <summary>Writes the content of specified files or URLs to a stream. (<c>svn cat</c>)</summary>
		bool Write(SvnTarget^ target, Stream^ toStream);
		/// <summary>Writes the content of specified files or URLs to a stream. (<c>svn cat</c>)</summary>
		bool Write(SvnTarget^ target, Stream^ toStream, SvnWriteArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // List Client Command
		/// <overloads>Streamingly lists directory entries in the repository. (<c>svn list</c>)</overloads>
		/// <summary>Streamingly lists directory entries in the repository. (<c>svn list</c>)</summary>
		bool List(SvnTarget^ target, EventHandler<SvnListEventArgs^>^ listHandler);
		/// <summary>Streamingly lists directory entries in the repository. (<c>svn list</c>)</summary>
		bool List(SvnTarget^ target, SvnListArgs^ args, EventHandler<SvnListEventArgs^>^ listHandler);

		/// <overloads>Gets a list of directory entries in the repository. (<c>svn list</c>)</overloads>
		/// <summary>Streamingly lists directory entries in the repository. (<c>svn list</c>)</summary>
		bool GetList(SvnTarget^ target, [Out] Collection<SvnListEventArgs^>^% list);
		/// <summary>Streamingly lists directory entries in the repository. (<c>svn list</c>)</summary>
		bool GetList(SvnTarget^ target, SvnListArgs^ args, [Out] Collection<SvnListEventArgs^>^% list);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Info Client Command
		/// <overloads>Streamingly retrieves information about a local or remote item (<c>svn info</c>)</overloads>
		/// <summary>Gets information about the specified target</summary>
		bool Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler);
		/// <summary>Gets information about the specified target</summary>
		bool Info(SvnTarget^ target, SvnInfoArgs^ args, EventHandler<SvnInfoEventArgs^>^ infoHandler);

		/// <summary>Gets information about the specified target</summary>
		bool GetInfo(SvnTarget^ target, [Out] SvnInfoEventArgs^% info);
		/// <summary>Gets information about the specified target</summary>
		bool GetInfo(SvnTarget^ target, SvnInfoArgs^ args, [Out] Collection<SvnInfoEventArgs^>^% info);

#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // CreateDirectory Client Command

		/// <overloads>Create a new directory under version control, scheduling (<c>svn mkdir</c>)</overloads>
		/// <summary>Create a new directory under version control, scheduling (<c>svn mkdir</c>)</summary>
		bool CreateDirectory(String^ path);
		/// <summary>Create a new directory under version control, scheduling (<c>svn mkdir</c>)</summary>
		bool CreateDirectory(String^ path, SvnCreateDirectoryArgs^ args);

		/// <summary>Create a new directory under version control, scheduling (<c>svn mkdir</c>)</summary>
		bool CreateDirectories(ICollection<String^>^ paths);
		/// <summary>Create a new directory under version control, scheduling (<c>svn mkdir</c>)</summary>
		bool CreateDirectories(ICollection<String^>^ paths, SvnCreateDirectoryArgs^ args);

		/// <overloads>Create a new directory under version control at the repository (<c>svn mkdir</c>)</overloads>
		/// <summary>Create a new directory under version control at the repository (<c>svn mkdir</c>)</summary>
		bool RemoteCreateDirectory(Uri^ uri);
		/// <summary>Create a new directory under version control at the repository (<c>svn mkdir</c>)</summary>
		bool RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args);
		/// <summary>Create a new directory under version control at the repository (<c>svn mkdir</c>)</summary>
		bool RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args, [Out] SvnCommitResult^% result);
		/// <summary>Create a new directory under version control at the repository (<c>svn mkdir</c>)</summary>
		bool RemoteCreateDirectories(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args);
		/// <summary>Create a new directory under version control at the repository (<c>svn mkdir</c>)</summary>
		bool RemoteCreateDirectories(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args, [Out] SvnCommitResult^% result);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // CreateDirectory Client Command

		/// <overloads>Remove files and directories from version control, scheduling (<c>svn delete|remove</c>)</overloads>
		/// <summary>Remove files and directories from version control, scheduling (<c>svn delete|remove</c>)</summary>
		bool Delete(String^ path);
		/// <summary>Remove files and directories from version control, scheduling (<c>svn delete|remove</c>)</summary>
		bool Delete(String^ path, SvnDeleteArgs^ args);

		/// <summary>Remove files and directories from version control, scheduling (<c>svn delete|remove</c>)</summary>
		bool Delete(ICollection<String^>^ paths);
		/// <summary>Remove files and directories from version control, scheduling (<c>svn delete|remove</c>)</summary>
		bool Delete(ICollection<String^>^ paths, SvnDeleteArgs^ args);

		/// <overloads>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</overloads>
		/// <summary>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</summary>
		bool RemoteDelete(Uri^ uri);
		/// <summary>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</summary>
		bool RemoteDelete(Uri^ uri, SvnDeleteArgs^ args);
		/// <summary>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</summary>
		bool RemoteDelete(Uri^ uri, SvnDeleteArgs^ args, [Out] SvnCommitResult^% result);
		/// <summary>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</summary>
		bool RemoteDelete(ICollection<Uri^>^ uris);
		/// <summary>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</summary>
		bool RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args);
		/// <summary>Remove files and directories from version control at the repository (<c>svn delete|remove</c>)</summary>
		bool RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args, [Out] SvnCommitResult^% result);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Import Client Command

		/// <overloads>Commit an unversioned file or tree into the repository (<c>svn import</c> followed by <c>svn checkout</c>)</overloads>
		/// <summary>Performs a working copy import to the specified Uri,
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		bool Import(String^ path, Uri^ target);
		/// <summary>Performs a working copy import to the specified Uri,
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		bool Import(String^ path, Uri^ target, [Out] SvnCommitResult^% result);
		/// <summary>Performs a working copy import to the specified Uri,
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		bool Import(String^ path, Uri^ target, SvnImportArgs^ args);
		/// <summary>Performs a working copy import to the specified Uri,
		/// by importing the root remotely, checking that out and then adding the files locally</summary>
		/// <remarks>Implemented as an Add follwed by an obstructed checkout</remarks>
		bool Import(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitResult^% result);

		/// <overloads>Commit an unversioned file or tree into the repository (<c>svn import</c>)</overloads>
		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		bool RemoteImport(String^ path, Uri^ target);
		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		bool RemoteImport(String^ path, Uri^ target, [Out] SvnCommitResult^% result);
		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		bool RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args);
		/// <summary>Importing as the subversion api does; without creating a working directory</summary>
		bool RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitResult^% result);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Commit Client Command
		/// <overloads>Send changes from your working copy to the repository (<c>svn commit</c>)</overloads>
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(String^ path);
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(String^ path, [Out] SvnCommitResult^% result);
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(ICollection<String^>^ paths);
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(ICollection<String^>^ paths, [Out] SvnCommitResult^% result);
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(String^ path, SvnCommitArgs^ args);
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(String^ path, SvnCommitArgs^ args, [Out] SvnCommitResult^% result);
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(ICollection<String^>^ paths, SvnCommitArgs^ args);
		/// <summary>Send changes from your working copy to the repository (<c>svn commit</c>)</summary>
		bool Commit(ICollection<String^>^ paths, SvnCommitArgs^ args, [Out] SvnCommitResult^% result);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // Revert Client Command
		/// <overloads>Restore pristine working copy file (undo most local edits) (<c>svn revert</c>)</overloads>
		/// <summary>Restore pristine working copy file (undo most local edits) (<c>svn revert</c>)</summary>
		bool Revert(String^ path);
		/// <summary>Restore pristine working copy file (undo most local edits) (<c>svn revert</c>)</summary>
		bool Revert(String^ path, SvnRevertArgs^ args);
		/// <summary>Restore pristine working copy file (undo most local edits) (<c>svn revert</c>)</summary>
		bool Revert(ICollection<String^>^ paths);
		/// <summary>Restore pristine working copy file (undo most local edits) (<c>svn revert</c>)</summary>
		bool Revert(ICollection<String^>^ paths, SvnRevertArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Resolved Client Command
		/// <overloads>Remove 'conflicted' state on a working copy file or directory using the specified choice</overloads>
		/// <summary>Remove 'conflicted' state on a working copy file or directory using the specified choice</summary>
		/// <remarks>Calls <see cref="Resolve(String,SvnAccept)" /> with SvnAccept.Merged</remarks>
		bool Resolved(String^ path);
		/// <summary>Remove 'conflicted' state on a working copy file or directory using the specified choice</summary>
		bool Resolve(String^ path, SvnAccept choice);
		/// <summary>Remove 'conflicted' state on a working copy file or directory using the specified choice</summary>
		bool Resolve(String^ path, SvnAccept choice, SvnResolveArgs^ args);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // Cleanup Client Command
		/// <overloads> Recursively clean up the working copy, removing locks, resuming unfinished operations, etc.(<c>svn cleanup</c>)</overloads>
		/// <summary>Cleans up the specified path, removing all workingcopy locks left behind by crashed clients</summary>
		/// <exception type="SvnException">Operation failed</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool CleanUp(String^ path);
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
		/// <summary>Duplicate something in working copy, remembering history (<c>svn copy</c>)</summary>
		bool Copy(SvnTarget^ sourceTarget, String^ toPath);
		
		/// <summary>Duplicate something in working copy, remembering history (<c>svn copy</c>)</summary>
		/// <remarks>Can be called with either a list of <see cref="SvnTarget" />, <see cref="SvnUriTarget" /> or <see cref="SvnPathTarget" />.
		/// All members must be of the same type.</remarks>
		generic<typename TSvnTarget> where TSvnTarget : SvnTarget
		bool Copy(ICollection<TSvnTarget>^ sourceTargets, String^ toPath);

		/// <summary>Duplicate something in working copy, remembering history (<c>svn copy</c>)</summary>
		/// <remarks>Can be called with either a list of <see cref="SvnTarget" />, <see cref="SvnUriTarget" /> or <see cref="SvnPathTarget" />.
		/// All members must be of the same type.</remarks>
		bool Copy(SvnTarget^ sourceTarget, String^ toPath, SvnCopyArgs^ args);

		/// <summary>Duplicate something in working copy, remembering history (<c>svn copy</c>)</summary>
		/// <remarks>Can be called with either a list of <see cref="SvnTarget" />, <see cref="SvnUriTarget" /> or <see cref="SvnPathTarget" />.
		/// All members must be of the same type.</remarks>
		generic<typename TSvnTarget> where TSvnTarget : SvnTarget
		bool Copy(ICollection<TSvnTarget>^ sourceTargets, String^ toPath, SvnCopyArgs^ args);

		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		bool RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri);
		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		/// <remarks>Can be called with either a list of <see cref="SvnTarget" />, <see cref="SvnUriTarget" /> or <see cref="SvnPathTarget" />.
		/// All members must be of the same type.</remarks>
		generic<typename TSvnTarget> where TSvnTarget : SvnTarget
		bool RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri);
		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		bool RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri, [Out] SvnCommitResult^% result);

		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		/// <remarks>Can be called with either a list of <see cref="SvnTarget" />, <see cref="SvnUriTarget" /> or <see cref="SvnPathTarget" />.
		/// All members must be of the same type.</remarks>
		generic<typename TSvnTarget> where TSvnTarget : SvnTarget
		bool RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri, [Out] SvnCommitResult^% result);

		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		bool RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args);

		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		/// <remarks>Can be called with either a list of <see cref="SvnTarget" />, <see cref="SvnUriTarget" /> or <see cref="SvnPathTarget" />.
		/// All members must be of the same type.</remarks>
		generic<typename TSvnTarget> where TSvnTarget : SvnTarget
		bool RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args);
		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		bool RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitResult^% result);

		/// <summary>Duplicate something in repository, remembering history (<c>svn copy</c>)</summary>
		/// <remarks>Can be called with either a list of <see cref="SvnTarget" />, <see cref="SvnUriTarget" /> or <see cref="SvnPathTarget" />.
		/// All members must be of the same type.</remarks>
		generic<typename TSvnTarget> where TSvnTarget : SvnTarget
		bool RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitResult^% result);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Move Client Command
		/// <overloads>Move and/or rename something in working copy, remembering history (<c>svn move</c>)</overloads>
		/// <summary>Move and/or rename something in working copy, remembering history (<c>svn move</c>)</summary>
		bool Move(String^ sourcePath, String^ toPath);
		/// <summary>Move and/or rename something in working copy, remembering history (<c>svn move</c>)</summary>
		bool Move(ICollection<String^>^ sourcePaths, String^ toPath);
		/// <summary>Move and/or rename something in working copy, remembering history (<c>svn move</c>)</summary>
		bool Move(String^ sourcePath, String^ toPath, SvnMoveArgs^ args);
		/// <summary>Move and/or rename something in working copy, remembering history (<c>svn move</c>)</summary>
		bool Move(ICollection<String^>^ sourcePaths, String^ toPath, SvnMoveArgs^ args);
		/// <overloads>Move and/or rename something in repository, remembering history (<c>svn move</c>)</overloads>
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(Uri^ sourceUri, Uri^ toUri);
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri);
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(Uri^ sourceUri, Uri^ toUri, [Out] SvnCommitResult^% result);
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, [Out] SvnCommitResult^% result);
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args);
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args);
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitResult^% result);
		/// <summary>Move and/or rename something in repository, remembering history (<c>svn move</c>)</summary>
		bool RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitResult^% result);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Lock Client Command
		/// <overloads>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</overloads>
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(String^ target, String^ comment);
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(Uri^ target, String^ comment);
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(ICollection<String^>^ targets, String^ comment);
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(ICollection<Uri^>^ targets, String^ comment);
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(String^ target, SvnLockArgs^ args);
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(Uri^ target, SvnLockArgs^ args);
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(ICollection<String^>^ targets, SvnLockArgs^ args);
		/// <summary>Lock working copy paths or URLs in the repository, so that no other user can commit changes to them (<c>svn lock</c>)</summary>
		bool Lock(ICollection<Uri^>^ targets, SvnLockArgs^ args);
		
	private:
		generic<typename TMarshaller> where TMarshaller : IItemMarshaller<String^>
		bool LockInternal(AprArray<String^, TMarshaller>^ items, SvnLockArgs^ args, AprPool^ pool);
			
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Lock Client Command
		/// <overloads>Unlock working copy paths or URLs (<c>svn unlock</c>)</overloads>
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(String^ target);
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(Uri^ target);
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(ICollection<String^>^ targets);
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(ICollection<Uri^>^ targets);
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(String^ target, SvnUnlockArgs^ args);
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(Uri^ target, SvnUnlockArgs^ args);
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(ICollection<Uri^>^ targets, SvnUnlockArgs^ args);
		/// <summary>Unlock working copy paths or URLs (<c>svn unlock</c>)</summary>
		bool Unlock(ICollection<String^>^ targets, SvnUnlockArgs^ args);

	private:
		generic<typename TMarshaller> where TMarshaller : IItemMarshaller<String^>
		bool UnlockInternal(AprArray<String^, TMarshaller>^ items, SvnUnlockArgs^ args, AprPool^ pool);			
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // SetProperty Client Command
		/// <overloads>Set the value of a property on files, dirs (<c>svn propset</c>)</overloads>
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(String^ target, String^ propertyName, String^ value);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(String^ target, String^ propertyName, ICollection<Byte>^ bytes);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(String^ target, String^ propertyName, String^ value, SvnSetPropertyArgs^ args);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(String^ target, String^ propertyName, ICollection<Byte>^ bytes, SvnSetPropertyArgs^ args);

		/// <overloads>Set the value of a property on files, dirs (<c>svn propset</c>)</overloads>
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(Uri^ target, String^ propertyName, String^ value);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(Uri^ target, String^ propertyName, ICollection<Byte>^ bytes);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(Uri^ target, String^ propertyName, String^ value, SvnSetPropertyArgs^ args);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool SetProperty(Uri^ target, String^ propertyName, ICollection<Byte>^ bytes, SvnSetPropertyArgs^ args);


		/// <overloads>Deletes a property from files, dirs (<c>svn propdel</c>)</overloads>
		/// <summary>Removes the specified property from the specfied path</summary>
		bool DeleteProperty(String^ target, String^ propertyName);
		/// <summary>Removes the specified property from the specfied path</summary>
		bool DeleteProperty(String^ target, String^ propertyName, SvnSetPropertyArgs^ args);
		/// <summary>Removes the specified property from the specfied path</summary>
		bool DeleteProperty(Uri^ target, String^ propertyName);
		/// <summary>Removes the specified property from the specfied path</summary>
		bool DeleteProperty(Uri^ target, String^ propertyName, SvnSetPropertyArgs^ args);

	internal:
		bool InternalSetProperty(SvnTarget^ target, String^ propertyName, const svn_string_t* value, SvnSetPropertyArgs^ args, AprPool^ pool);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // GetProperty Client Command
		/// <overloads>Retrieves the value of a property on files, dirs, or revisions (<c>svn propget</c>)</overloads>
		/// <summary>Gets the specified property from the specfied path</summary>
		/// <returns>true if property is set, otherwise false</returns>
		/// <exception type="SvnException">path is not a valid workingcopy path</exception>
		bool GetProperty(SvnTarget^ target, String^ propertyName, [Out] String^% value);
		/// <summary>Gets the specified property from the specfied path</summary>
		/// <returns>true if property is set, otherwise false</returns>
		/// <exception type="SvnException">path is not a valid workingcopy path</exception>
		bool GetProperty(SvnTarget^ target, String^ propertyName, [Out] SvnPropertyValue^% bytes);
		/// <summary>Sets the specified property on the specfied path to value</summary>
		/// <remarks>Use <see cref="DeleteProperty(String^,String^, SvnSetPropertyArgs^)" /> to remove an existing property</remarks>
		bool GetProperty(SvnTarget^ target, String^ propertyName, SvnGetPropertyArgs^ args, [Out] SvnTargetPropertyCollection^% properties);
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
		/// <summary>Streamingly lists all properties on files or dirs (<c>svn proplist</c>)</summary>
		bool PropertyList(SvnTarget^ target, EventHandler<SvnPropertyListEventArgs^>^ listHandler);
		/// <summary>Streamingly lists all properties on files or dirs (<c>svn proplist</c>)</summary>
		bool PropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, EventHandler<SvnPropertyListEventArgs^>^ listHandler);

		/// <overloads>Gets all properties on files or dirs (<c>svn proplist</c>)</overloads>
		/// <summary>Gets all properties on files or dirs (<c>svn proplist</c>)</summary>
		bool GetPropertyList(SvnTarget^ target, [Out] Collection<SvnPropertyListEventArgs^>^% list);
		/// <summary>Gets all properties on files or dirs (<c>svn proplist</c>)</summary>
		bool GetPropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, [Out] Collection<SvnPropertyListEventArgs^>^% list);
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // SetRevisionProperty Client Command
		/// <overloads>Sets the value of a revision property on files, dirs in a specific revision (<c>svn propset --revision</c>)</overloads>
		/// <summary>Sets the value of a revision property on files, dirs in a specific revision (<c>svn propset --revision</c>)</summary>
		bool SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, String^ value);
		/// <summary>Sets the value of a revision property on files, dirs in a specific revision (<c>svn propset --revision</c>)</summary>
		bool SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, ICollection<Byte>^ bytes);
		/// <summary>Sets the value of a revision property on files, dirs in a specific revision (<c>svn propset --revision</c>)</summary>
		bool SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args, String^ value);
		/// <summary>Sets the value of a revision property on files, dirs in a specific revision (<c>svn propset --revision</c>)</summary>
		bool SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args, ICollection<Byte>^ bytes);

		/// <overloads>Deletes the value of a revision property on files, dirs in a specific revision(<c>svn propdel --revision</c>)</overloads>
		/// <summary>Deletes the value of a revision property on files, dirs in a specific revision(<c>svn propdel --revision</c>)</summary>
		bool DeleteRevisionProperty(SvnUriTarget^ target, String^ propertyName);
		/// <summary>Deletes the value of a revision property on files, dirs in a specific revision(<c>svn propdel --revision</c>)</summary>
		bool DeleteRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args);

	internal:
		bool InternalSetRevisionProperty(SvnUriTarget^ target, String^ propertyName, const svn_string_t* value, SvnSetRevisionPropertyArgs^ args, AprPool^ pool);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // GetRevisionProperty Client Command
		/// <overloads>Gets the value of a revision property on files or dirs in a specific revision (<c>svn propget --revision</c>)</overloads>
		/// <summary>Gets the value of a revision property on files or dirs in a specific revision (<c>svn propget --revision</c>)</summary>
		bool GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] String^% value);
		/// <summary>Gets the value of a revision property on files or dirs in a specific revision (<c>svn propget --revision</c>)</summary>
		bool GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] SvnPropertyValue^% value);
		/// <summary>Gets the value of a revision property on files or dirs in a specific revision (<c>svn propget --revision</c>)</summary>
		bool GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] String^% value);
		/// <summary>Gets the value of a revision property on files or dirs in a specific revision (<c>svn propget --revision</c>)</summary>
		bool GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] SvnPropertyValue^% value);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Properties List Client Command
		/// <overloads>Gets all revision properties on a a specific revision (<c>svn proplist --revision</c>)</overloads>
		/// <summary>Gets all revision properties on a a specific revision (<c>svn proplist --revision</c>)</summary>
		bool GetRevisionPropertyList(SvnUriTarget^ target, [Out] SvnPropertyCollection^% list);
		/// <summary>Gets all revision properties on a a specific revision (<c>svn proplist --revision</c>)</summary>
		bool GetRevisionPropertyList(SvnUriTarget^ target, SvnRevisionPropertyListArgs^ args, [Out] SvnPropertyCollection^% list);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // DiffMerge Client Command
		/// <overloads>Merges the differences between two sources to a working copy path (<c>svn merge</c>)</overloads>
		/// <summary>
		/// Merges the changes from <paramref name="mergeFrom" /> to <paramref name="mergeTo" /> into <paramRef name="targetPath" />
		/// </summary>
		bool DiffMerge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo);
		/// <summary>
		/// Merges the changes from <paramref name="mergeFrom" /> to <paramref name="mergeTo" /> into <paramRef name="targetPath" />
		/// </summary>
		bool DiffMerge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo, SvnDiffMergeArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Merge Client Command
		/// <overloads>Merges the changes in the specified revisions from source to targetPath (<c>svn merge</c>)</overloads>
		/// <summary>Merges the changes in the specified revisions from source to targetPath (<c>svn merge</c>)</summary>
		bool Merge(String^ targetPath, SvnTarget^ source, SvnRevisionRange^ mergeRange);

		/// <summary>Merges the changes in the specified revisions from source to targetPath (<c>svn merge</c>)</summary>
		generic<typename TRevisionRange> where TRevisionRange : SvnRevisionRange
		bool Merge(String^ targetPath, SvnTarget^ source, ICollection<TRevisionRange>^ mergeRange);

		/// <summary>Merges the changes in the specified revisions from source to targetPath (<c>svn merge</c>)</summary>
		bool Merge(String^ targetPath, SvnTarget^ source, SvnRevisionRange^ mergeRange, SvnMergeArgs^ args);

		/// <summary>Merges the changes in the specified revisions from source to targetPath (<c>svn merge</c>)</summary>
		generic<typename TRevisionRange> where TRevisionRange : SvnRevisionRange
		bool Merge(String^ targetPath, SvnTarget^ source, ICollection<TRevisionRange>^ mergeRange, SvnMergeArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // ReintegrationMerge Client Command
		/// <overloads>Perform a reintegration merge of source into targetPath</overloads>
		/// <summary>Perform a reintegration merge of source into targetPath</summary>
		bool ReintegrationMerge(String^ targetPath, SvnTarget^ source);
		/// <summary>Perform a reintegration merge of source into targetPath</summary>
		bool ReintegrationMerge(String^ targetPath, SvnTarget^ source, SvnReintegrationMergeArgs^ args);
#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Diff Client Command
		/// <overloads>Retrieves the differences between two revisions or paths (<c>svn diff</c>)</overloads>
		/// <summary>Retrieves the differences between two revisions or paths (<c>svn diff</c>)</summary>
		bool Diff(SvnTarget^ from, SvnTarget^ to, Stream^ result);
		/// <summary>Retrieves the differences between two revisions or paths (<c>svn diff</c>)</summary>
		bool Diff(SvnTarget^ from, SvnTarget^ to, SvnDiffArgs^ args, Stream^ result);
		/// <summary>Retrieves the differences between two revisions or paths (<c>svn diff</c>)</summary>
		bool Diff(SvnTarget^ source, SvnRevisionRange^ range, Stream^ result);
		/// <summary>Retrieves the differences between two revisions or paths (<c>svn diff</c>)</summary>
		bool Diff(SvnTarget^ source, SvnRevisionRange^ range, SvnDiffArgs^ args, Stream^ result);

#pragma endregion

	public:
		/////////////////////////////////////////
#pragma region // Diff Summary Command
		/// <overloads>Streamingly retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</overloads>
		/// <summary>Streamingly retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</summary>
		bool DiffSummary(SvnTarget^ from, SvnTarget^ to, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler);
		/// <summary>Streamingly retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</summary>
		bool DiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler);
		/// <overloads>Retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</overloads>
		/// <summary>Streamingly retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</summary>
		bool GetDiffSummary(SvnTarget^ from, SvnTarget^ to, [Out] Collection<SvnDiffSummaryEventArgs^>^% list);
		/// <summary>Streamingly retrieves the differences between two revisions or paths (<c>svn diff --summarize</c>)</summary>
		bool GetDiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, [Out] Collection<SvnDiffSummaryEventArgs^>^% list);
#pragma endregion


	public:
		/// <overloads>Associate the specified path(s) with the specified changelist (<c>svn changelist</c>)</overloads>
		/// <summary>Associate the specified path(s) with the specified changelist (<c>svn changelist</c>)</summary>
		bool AddToChangeList(String^ target, String^ changeList);
		/// <summary>Associate the specified path(s) with the specified changelist (<c>svn changelist</c>)</summary>
		bool AddToChangeList(String^ target, String^ changeList, SvnAddToChangeListArgs^ args);
		/// <summary>Associate the specified path(s) with the specified changelist (<c>svn changelist</c>)</summary>
		bool AddToChangeList(ICollection<String^>^ targets, String^ changeList);
		/// <summary>Associate the specified path(s) with the specified changelist (<c>svn changelist</c>)</summary>
		bool AddToChangeList(ICollection<String^>^ targets, String^ changeList, SvnAddToChangeListArgs^ args);
		/// <overloads>Deassociate the specified path(s) from the specified changelist (<c>svn changelist</c>)</overloads>
		/// <summary>Deassociate the specified path(s) from the specified changelist (<c>svn changelist</c>)</summary>
		bool RemoveFromChangeList(String^ target);
		/// <summary>Deassociate the specified path(s) from the specified changelist (<c>svn changelist</c>)</summary>
		bool RemoveFromChangeList(String^ target, SvnRemoveFromChangeListArgs^ args);
		/// <summary>Deassociate the specified path(s) from the specified changelist (<c>svn changelist</c>)</summary>
		bool RemoveFromChangeList(ICollection<String^>^ targets);
		/// <summary>Deassociate the specified path(s) from the specified changelist (<c>svn changelist</c>)</summary>
		bool RemoveFromChangeList(ICollection<String^>^ targets, SvnRemoveFromChangeListArgs^ args);
		/// <overloads>Streamingly gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</overloads>
		/// <summary>Streamingly gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</summary>
		bool ListChangeList(String^ rootPath, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler);
		/// <summary>Streamingly gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</summary>
		bool ListChangeList(String^ rootPath, SvnListChangeListArgs^ args, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler);

		/// <overloads>Gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</overloads>
		/// <summary>Gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</summary>
		bool GetChangeList(String^ rootPath, [Out]Collection<SvnListChangeListEventArgs^>^% list);
		/// <summary>Gets the contents of the specified changelist below a specified directory (<c>svn changelist</c>)</summary>
		bool GetChangeList(String^ rootPath, SvnListChangeListArgs^ args, [Out]Collection<SvnListChangeListEventArgs^>^% list);

	public:
		/// <overloads>Gets a list of Uri's which might be valid merge sources (<c>svn mergeinfo</c>)</overloads>
		/// <summary>Gets a list of Uri's which might be valid merge sources</summary>
		/// <remarks>The list contains copy-from locations and previous merge locations</remarks>
		bool GetSuggestedMergeSources(SvnTarget ^target, [Out]SvnMergeSourcesCollection^% mergeSources);
		/// <summary>Gets a list of Uri's which might be valid merge sources</summary>
		/// <remarks>The list contains copy-from locations and previous merge locations</remarks>
		bool GetSuggestedMergeSources(SvnTarget ^target, SvnGetSuggestedMergeSourcesArgs^ args, [Out]SvnMergeSourcesCollection^% mergeSources);

	public:
		/// <overloads>Gets the merges which are applied on the specified target (<c>svn mergeinfo</c>)</overloads>
		/// <summary>Gets the merges which are applied on the specified target</summary>
		bool GetAppliedMergeInfo(SvnTarget ^target, [Out]SvnAppliedMergeInfo^% mergeInfo);
		/// <summary>Gets the merges which are applied on the specified target</summary>
		bool GetAppliedMergeInfo(SvnTarget ^target, SvnGetAppliedMergeInfoArgs^ args, [Out]SvnAppliedMergeInfo^% mergeInfo);

	public:
		/// <overloads>Gets a list of available merges which can be applied on the specified target (<c>svn mergeinfo</c>)</overloads>
		/// <summary>Gets a list of merges which can be applied on target</summary>
		bool GetEligableMergeInfo(SvnTarget ^target, SvnUriTarget^ sourceUri, [Out]SvnEligableMergeInfo^% mergeInfo);
		/// <summary>Gets a list of merges which can be applied on target</summary>
		bool GetEligableMergeInfo(SvnTarget ^target, SvnUriTarget^ sourceUri, SvnGetEligableMergeInfoArgs^ args, [Out]SvnEligableMergeInfo^% mergeInfo);

	public:
		/// <overloads>Streamingly retrieve the content of specified files or URLs with revision and author information per-line (<c>svn blame</c>)</overloads>
		/// <summary>Streamingly retrieve the content of specified files or URLs with revision and author information per-line (<c>svn blame</c>)</summary>
		bool Blame(SvnTarget^ target, EventHandler<SvnBlameEventArgs^>^ blameHandler);
		/// <summary>Streamingly retrieve the content of specified files or URLs with revision and author information per-line (<c>svn blame</c>)</summary>
		bool Blame(SvnTarget^ target, SvnBlameArgs^ args, EventHandler<SvnBlameEventArgs^>^ blameHandler);
		/// <summary>Retrieve the content of specified files or URLs with revision and author information per-line (<c>svn blame</c>)</summary>
		bool GetBlame(SvnTarget^ target, [Out] Collection<SvnBlameEventArgs^>^% list);
		/// <summary>Retrieve the content of specified files or URLs with revision and author information per-line (<c>svn blame</c>)</summary>
		bool GetBlame(SvnTarget^ target, SvnBlameArgs^ args, [Out] Collection<SvnBlameEventArgs^>^% list);

	public:
		/// <summary>Gets the repository Uri of a path, or <c>null</c> if path is not versioned</summary>
		/// <remarks>See also <see cref="SvnTools::GetUriFromWorkingCopy" /></remarks>
		Uri^ GetUriFromWorkingCopy(String^ path);

		/// <overloads>Gets the repository root from the specified uri or path</overloads>
		/// <summary>Gets the repository root from the specified uri</summary>
		/// <value>The repository root <see cref="Uri" /> or <c>null</c> if the uri is not a repository uri</value>
		/// <remarks>SharpSvn makes sure the uri ends in a '/'</remarks>
		Uri^ GetRepositoryRoot(Uri^ uri);

		/// <summary>Gets the repository root from the specified path</summary>
		/// <value>The repository root <see cref="Uri" /> or <c>null</c> if the uri is not a working copy path</value>
		/// <remarks>SharpSvn makes sure the uri ends in a '/'</remarks>
		Uri^ GetRepositoryRoot(String^ target);

		/// <summary>Gets the Uuid of a Uri, or <see cref="Guid::Empty" /> if path is not versioned</summary>
		/// <returns>true if successfull, otherwise false</returns>
		bool GetRepositoryIdFromUri(Uri^ uri, [Out] Guid% id);

	public:
		/// <summary>Gets the Pristine file and/or text file status of a working copy path</summary>
		bool GetWorkingCopyState(String^ targetPath, [Out] SvnWorkingCopyState^% result);
		/// <summary>Gets the Pristine file and/or text file status of a working copy path</summary>
		bool GetWorkingCopyState(String^ targetPath, SvnGetWorkingCopyStateArgs^ args, [Out] SvnWorkingCopyState^% result);

	private:
		~SvnClient();
	};
}
#include "SvnClientArgs.h"
