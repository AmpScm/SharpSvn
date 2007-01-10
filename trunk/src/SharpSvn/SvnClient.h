
#pragma once

#include "SvnClientContext.h"
#include "SvnUriTarget.h"
#include "SvnPathTarget.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "SvnCommitArgs.h"
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

	/// <summary>Subversion client instance; main entrance to Subversion api</summary>
	public ref class SvnClient : public SvnClientContext
	{
		initonly SharpSvn::Apr::AprBaton<SvnClient^>^ _clientBatton;
		AprPool^ _pool;
		SvnClientArgs^ _currentArgs;
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

/////////////////////////////////////////
#pragma region // Client events
	private:
		EventHandler<SvnClientCancelEventArgs^>^	_clientCancel;
		EventHandler<SvnClientProgressEventArgs^>^	_clientProgress;
		EventHandler<SvnClientNotifyEventArgs^>^	_clientNotify;
		EventHandler<SvnClientBeforeCommitEventArgs^>^	_clientBeforeCommit;
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

	protected:
		virtual void OnClientCancel(SvnClientCancelEventArgs^ e);
		virtual void OnClientProgress(SvnClientProgressEventArgs^ e);
		virtual void OnClientBeforeCommit(SvnClientBeforeCommitEventArgs^ e);
		virtual void OnClientNotify(SvnClientNotifyEventArgs^ e);

	internal:

		void HandleClientCancel(SvnClientCancelEventArgs^ e);
		void HandleClientProgress(SvnClientProgressEventArgs^ e);
		void HandleClientGetCommitLog(SvnClientBeforeCommitEventArgs^ e);
		void HandleClientNotify(SvnClientNotifyEventArgs^ e);

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

	private:
		bool UpdateInternal(ICollection<String^>^ paths, SvnUpdateArgs^ args, array<__int64>^ revisions);
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
		void Add(String^ path, bool notRecursive);

		/// <summary>Cleans up the specified path, removing all workingcopy locks left behind by crashed clients</summary>
		/// <returns>true if the operation succeeded; false if it did not</returns>
		/// <exception type="SvnException">Operation failed and args.ThrowOnError = true</exception>
		/// <exception type="ArgumentException">Parameters invalid</exception>
		bool Add(String^ path, SvnAddArgs^ args);
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

	private:
		bool CommitInternal(ICollection<String^>^ paths, SvnCommitArgs^ args, bool requireInfo, [Out] SvnCommitInfo^% commitInfo);
#pragma endregion

	internal:
		generic<typename T>
		where T : SvnClientEventArgs
		ref class InfoItemList : public System::Collections::Generic::List<T>
		{
		public:
			InfoItemList()
			{}

		internal:
			void HandleItem(Object^ sender, T e)
			{
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
		void Status(SvnPathTarget^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler);
		/// <summary>Gets status data for the specified path</summary>
		bool Status(SvnPathTarget^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler, SvnStatusArgs^ args);

		/// <summary>Recursively gets a list of 'interesting' status data for the specified path</summary>
		void GetStatus(SvnPathTarget^ path, [Out] IList<SvnStatusEventArgs^>^% statuses);
		/// <summary>Gets the status data for the specified path</summary>
		void GetStatus(SvnPathTarget^ path, [Out] SvnStatusEventArgs^% status);
		/// <summary>Gets a list of status data for the specified path</summary>
		bool GetStatus(SvnPathTarget^ path, SvnStatusArgs^ args, [Out] IList<SvnStatusEventArgs^>^% statuses);
		
#pragma endregion


	public:
		/////////////////////////////////////////
#pragma region // Info Client Command
		/// <summary>Gets information about the specified target</summary>
		void Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler);
		/// <summary>Gets information about the specified target</summary>
		bool Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler, SvnInfoArgs^ args);

		/// <summary>Gets information about the specified target</summary>
		void GetInfo(SvnTarget^ target, [Out] SvnInfoEventArgs^% status);
		/// <summary>Gets information about the specified target</summary>
		bool GetInfo(SvnTarget^ target, SvnInfoArgs^ args, [Out] IList<SvnInfoEventArgs^>^ statuses);
		
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
		/// <summary>Gets the repository Uri of a path, or <c>null</c> if path is not versioned</summary>
		Uri^ GetUriFromPath(String^ path);

		/// <summary>Gets the Uuid of a Uri, or <see cref="Guid::Empty" /> if path is not versioned</summary>
		/// <returns>true if successfull, otherwise false</returns>
		bool GetUuidFromUri(Uri^ uri, [Out] Guid% uuid);

	private:
		~SvnClient();
	};
}
