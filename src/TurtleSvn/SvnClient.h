
#pragma once

#include "SvnClientContext.h"
#include "SvnUriTarget.h"
#include "SvnPathTarget.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "SvnBatton.h"

namespace QQn {
	namespace Svn {
		public enum class SvnClientState
		{
			Initial,
			ConfigLoaded,
			AuthorizationInitialized
		};

		ref class SvnCancelEventArgs;
		ref class SvnProgressEventArgs;
		ref class SvnLogEventArgs;
		ref class SvnNotifyEventArgs;

		using System::Runtime::InteropServices::GCHandle;

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
			initonly SvnBatton<SvnClient^>^ _clientBatton;
			AprPool^ _pool;
			SvnClientState _clientState;
			SvnClientArgs^ _currentArgs;
		public:
			///<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
			SvnClient();
			///<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
			SvnClient(AprPool^ pool);

		private:
			void Initialize();

		public:
			property static Version^ Version
			{
				System::Version^ get()
				{
					const svn_version_t* version = svn_client_version();

					return gcnew System::Version(version->major, version->minor, version->patch);
				}
			}

			property static System::Version^ WrapperVersion
			{
				System::Version^ get()
				{
					return (gcnew System::Reflection::AssemblyName(SvnClient::typeid->Assembly->FullName))->Version;
				}
			}

			property SvnClientState ClientState
			{
				SvnClientState get()
				{
					return _clientState;
				}
			}

		protected:
			void EnsureState(SvnClientState state);

		public:
			/// <summary>Loads the subversion configuration from the specified path</summary>
			void LoadConfiguration(String^ path);
			/// <summary>Loads the subversion configuration from the specified path and optionally ensures the path is a subversion config dir by creating default files</summary>
			void LoadConfiguration(String^ path, bool ensurePath);
			/// <summary>Loads the standard subversion configuration and ensures the subversion config dir by creating default files</summary>
			void LoadConfigurationDefault();
			/// <summary>Merges configuration from the specified path into the existing configuration</summary>
			void MergeConfiguration(String^ path);

		public:
			event EventHandler<SvnClientCancelEventArgs^>^		ClientCancel;
			event EventHandler<SvnClientProgressEventArgs^>^	ClientProgress;
			event EventHandler<SvnClientCommitLogEventArgs^>^	ClientGetCommitLog;
			event EventHandler<SvnClientNotifyEventArgs^>^		ClientNotify;

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


		public:
			/// <summary>Performs a recursive checkout of <paramref name="url" /> to <paramref name="path" /></summary>
			void CheckOut(SvnUriTarget^ url, String^ path, [Out] __int64% revision)
			{
				CheckOut(url, path, revision, gcnew SvnCheckOutArgs());
			}

			bool CheckOut(SvnUriTarget^ url, String^ path, [Out] __int64% revision, SvnCheckOutArgs^ args);



		private:
			~SvnClient();
		};
	}
}