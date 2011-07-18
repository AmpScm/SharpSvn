#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "LookArgs/SvnLookClientArgs.h"

#include "AprBaton.h"

namespace SharpSvn {
	namespace Remote {

	ref class SvnRemoteSessionArgs;
	ref class SvnRemoteCommonArgs;

	ref class SvnRemoteOpenArgs;
	ref class SvnRemoteReparentArgs;
	ref class SvnRemoteLatestRevisionArgs;
	ref class SvnRemoteNodeKindArgs;
	ref class SvnRemoteLogArgs;
	ref class SvnRemoteStatArgs;
	ref class SvnRemoteListArgs;
	ref class SvnRemoteListLocksArgs;
	ref class SvnRemoteLocationSegmentsArgs;
	ref class SvnRemotePropertiesArgs;

	ref class SvnRemoteStatEventArgs;

	public ref class SvnRevisionLocationMap sealed : KeyedCollection<__int64, SvnUriTarget^>
	{
	protected:
		virtual __int64 GetKeyForItem(SvnUriTarget^ item) override
		{
			if (!item)
				throw gcnew ArgumentNullException("item");

			return item->Revision->Revision;
		}
	};

	/// <summary>This class gives access to the Subversion network protocol. Make sure all
	/// passed paths use '/' as path separator. Refer to svn_ra.h for details about the
	/// functions on this class, or use the more polished wrappers on the SvnClient class
	/// </summary>
	public ref class SvnRemoteSession : public SvnClientContext
	{
		initonly AprBaton<SvnRemoteSession^>^ _clientBaton;
		AprPool _pool;
		svn_ra_session_t *_session;
		bool _cbInitialized;
		Uri^ _sessionRoot;
		Uri^ _reposRoot;

	public:
		/// <summary>Initializes a new SvnRemoteSession instance</summary>
		SvnRemoteSession();
		/// <summary>Initializes a new SvnRemoteSession instance and then <see cref="Open(Uri^)" /> a session to the specified url</summary>
		SvnRemoteSession(Uri^ sessionUri);

	private:
		~SvnRemoteSession();

	internal:
		svn_error_t *Init();
		void Ensure();

	public:
		/// <overloads>Opens the session to the specified Uri</overloads>
		/// <summary>Opens the session to the specified Uri</summary>
		bool Open(Uri^ sessionUri);
		/// <summary>Opens the session to the specified Uri</summary>
		bool Open(Uri^ sessionUri, SvnRemoteOpenArgs^ args);

	public:
		property Uri^ SessionUri
		{
			Uri^ get()
			{
				return _sessionRoot;
			}
		}

	public:
		/// <overloads>Reparents the session to the specified Uri</overloads>
		/// <summary>Reparents the session to the specified Uri in the same repository</summary>
		bool Reparent(Uri^ newSessionUri);
		/// <summary>Reparents the session to the specified Uri in the same repository</summary>
		bool Reparent(Uri^ newSessionUri, SvnRemoteCommonArgs^ args);

	public:
		/// <overloads>Get the latest revision in the repository</overloads>
		/// <summary>Get the latest revision in the repository</summary>
		bool GetLatestRevision([Out] __int64% revision);
		/// <summary>Get the latest revision in the repository</summary>
		bool GetLatestRevision(SvnRemoteCommonArgs^ args, [Out] __int64% revision);

	private:
		bool InternalGetLatestRevision(SvnRemoteSessionArgs^ args, [Out] __int64% revno);

	public:
		/// <overloads>Resolves a dated or head revision to an actual revision number</overloads>
		/// <summary>Resolves a dated or head revision to an actual revision number</summary>
		bool ResolveRevision(SvnRevision^ revision, [Out] __int64% revno);
		/// <summary>Resolves a dated or head revision to an actual revision number</summary>
		bool ResolveRevision(SvnRevision^ revision, SvnRemoteCommonArgs^ args, [Out] __int64% revno);

	private:
		bool InternalResolveRevision(SvnRevision^ revision, SvnRemoteSessionArgs^ args, [Out] __int64% revno);

	public:
		bool GetRepositoryRoot([Out] Uri^% uri);
		bool GetRepositoryRoot(SvnRemoteCommonArgs^ args, [Out] Uri^% uri);

	public:
		bool GetRepositoryId([Out] Guid% uuid);
		bool GetRepositoryId(SvnRemoteCommonArgs^ args, [Out] Guid% uuid);

	public:
		bool GetStat(String^ relPath, [Out] SvnRemoteStatEventArgs^% result);
		bool GetStat(String^ relPath, SvnRemoteStatArgs^ args, [Out] SvnRemoteStatEventArgs^% result);

	public:
		bool GetNodeKind(String^ relPath, [Out] SvnNodeKind% result);
		bool GetNodeKind(String^ relPath, SvnRemoteNodeKindArgs^ args, [Out] SvnNodeKind% result);

	public:
		bool List(String^ relPath, EventHandler<SvnRemoteListEventArgs^>^ listHandler);
		bool List(String^ relPath, SvnRemoteListArgs^ args, EventHandler<SvnRemoteListEventArgs^>^ listHandler);

	public:
		bool ListLocks(String^ relPath, EventHandler<SvnRemoteListLockEventArgs^>^ listHandler);
		bool ListLocks(String^ relPath, SvnRemoteListLocksArgs^ args, EventHandler<SvnRemoteListLockEventArgs^>^ listHandler);

	public:
		bool LocationSegments(String^ relPath, EventHandler<SvnRemoteLocationSegmentEventArgs^>^ segmentHandler);
		bool LocationSegments(String^ relPath, SvnRemoteLocationSegmentsArgs^ args, EventHandler<SvnRemoteLocationSegmentEventArgs^>^ segmentHandler);

		bool GetLocationSegments(String^ relPath, [Out] Collection<SvnRemoteLocationSegmentEventArgs^>^% list);
		bool GetLocationSegments(String^ relPath, SvnRemoteLocationSegmentsArgs^ args, [Out] Collection<SvnRemoteLocationSegmentEventArgs^>^% list);

	public:
		bool GetLocations(String^ relpath, __int64 revision, ICollection<__int64>^ resolveRevisions, [Out] SvnRevisionLocationMap^% locations);
		bool GetLocations(String^ relpath, __int64 revision, ICollection<__int64>^ resolveRevisions, SvnRemoteCommonArgs^ args, [Out] SvnRevisionLocationMap^% locations);

	public:
		bool Log(String^ relPath, SvnRevisionRange^ range, EventHandler<SvnRemoteLogEventArgs^>^ logHandler);
		/// <summary>Streamingly retrieve the log messages for a set of revision(s).</summary>
		bool Log(String^ relPath, SvnRevisionRange^ range, SvnRemoteLogArgs^ args, EventHandler<SvnRemoteLogEventArgs^>^ logHandler);

		/// <overloads>Streamingly retrieve the log messages for a set of revision(s)</overloads>
		/// <summary>Streamingly retrieve the log messages for a set of revision(s).</summary>
		bool Log(ICollection<String^>^ relPaths, SvnRevisionRange^ range, EventHandler<SvnRemoteLogEventArgs^>^ logHandler);
		/// <summary>Streamingly retrieve the log messages for a set of revision(s).</summary>
		bool Log(ICollection<String^>^ relPaths, SvnRevisionRange^ range, SvnRemoteLogArgs^ args, EventHandler<SvnRemoteLogEventArgs^>^ logHandler);

	public:
		bool GetProperties(String^ relpath, [Out] SvnPropertyCollection^% properties);
		bool GetProperties(String^ relpath, SvnRemotePropertiesArgs^ args, [Out] SvnPropertyCollection^% properties);

	public:
		static bool IsConnectionlessRepository(Uri^ uri);
		static bool RequiresExternalAuthorization(Uri^ uri);

	public:
		String^ MakeRelativePath(Uri^ uri);
		String^ MakeRepositoryRootRelativePath(Uri^ uri);

	public:
		/// <summary>
		/// Raised to allow canceling operations. The event is first
		/// raised on the <see cref="SvnClientArgs" /> object and
		/// then on the <see cref="SvnLookClient" />
		/// </summary>
		DECLARE_EVENT(SvnCancelEventArgs^, Cancel)

		/// <summary>
		/// Raised when a subversion exception occurs.
		/// Set <see cref="SvnErrorEventArgs::Cancel" /> to true to cancel
		/// throwing the exception
		/// </summary>
		DECLARE_EVENT(SvnErrorEventArgs^, SvnError)

		/// <summary>
		/// Raised on progress. The event is first
		/// raised on the <see cref="SvnClientArgs" /> object and
		/// then on the <see cref="SvnClient" />
		/// </summary>
		DECLARE_EVENT(SvnProgressEventArgs^, Progress)

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
		/// <summary>Raises the <see cref="Exception" /> event.</summary>
		virtual void OnSvnError(SvnErrorEventArgs^ e);
		/// <summary>Raises the <see cref="Processing" /> event.</summary>
		virtual void OnProcessing(SvnProcessingEventArgs^ e);

	internal:
		void HandleClientCancel(SvnCancelEventArgs^ e);
		void HandleClientProgress(SvnProgressEventArgs^ e);

		virtual void HandleClientError(SvnErrorEventArgs^ e) override sealed;
		virtual void HandleProcessing(SvnProcessingEventArgs^ e) override sealed;

	};
	}
}
