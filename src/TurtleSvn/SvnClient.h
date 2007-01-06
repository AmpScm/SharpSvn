
#pragma once

#include "SvnClientContext.h"
#include "SvnUriTarget.h"
#include "SvnPathTarget.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "AprBaton.h"

namespace TurtleSvn {

	ref class SvnCancelEventArgs;
	ref class SvnProgressEventArgs;
	ref class SvnLogEventArgs;
	ref class SvnNotifyEventArgs;

	using System::Runtime::InteropServices::GCHandle;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;

	struct SvnClientCallBacks
	{
		static svn_error_t *svn_cancel_func(void *cancel_baton);
		static svn_error_t *svn_client_get_commit_log2(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool);
		static void svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool);
		static void svn_ra_progress_notify_func(apr_off_t progress, apr_off_t total, void *baton, apr_pool_t *pool);
	};

	/// <summary>Subversion client instance; main entrance to Subversion api</summary>
	public ref class SvnClient : public SvnClientContext
	{
		initonly TurtleSvn::Apr::AprBaton<SvnClient^>^ _clientBatton;
		AprPool^ _pool;
		SvnClientArgs^ _currentArgs;
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

	protected:
		virtual void OnClientCancel(SvnClientCancelEventArgs^ e);
		virtual void OnClientProgress(SvnClientProgressEventArgs^ e);
		virtual void OnClientGetCommitLog(SvnClientCommitLogEventArgs^ e);
		virtual void OnClientNotify(SvnClientNotifyEventArgs^ e);

	internal:

		void HandleClientCancel(SvnClientCancelEventArgs^ e);
		void HandleClientProgress(SvnClientProgressEventArgs^ e);
		void HandleClientGetCommitLog(SvnClientCommitLogEventArgs^ e);
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

	private:
		~SvnClient();
	};
}
