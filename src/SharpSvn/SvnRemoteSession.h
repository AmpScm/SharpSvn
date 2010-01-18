#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "LookArgs/SvnLookClientArgs.h"

#include "AprBaton.h"

namespace SharpSvn {

	ref class SvnRemoteCommonArgs;

	ref class SvnRemoteOpenArgs;
	ref class SvnRemoteReparentArgs;
	ref class SvnRemoteLatestRevisionArgs;
	ref class SvnRemoteLogArgs;
	ref class SvnRemoteStatArgs;

	ref class SvnRemoteStatEventArgs;

	public ref class SvnRemoteSession : public SvnClientContext
	{
		initonly AprBaton<SvnRemoteSession^>^ _clientBaton;
		AprPool _pool;
		svn_ra_session_t *_session;
		bool _cbInitialized;
		Uri^ _root;
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
		/// <overloads>Streamingly retrieve the log messages for a set of revision(s)</overloads>
		/// <summary>Streamingly retrieve the log messages for a set of revision(s).</summary>
		bool Log(ICollection<String^>^ paths, EventHandler<SvnRemoteLogEventArgs^>^ logHandler);
		/// <summary>Streamingly retrieve the log messages for a set of revision(s).</summary>
		bool Log(ICollection<String^>^ paths, SvnRemoteLogArgs^ args, EventHandler<SvnRemoteLogEventArgs^>^ logHandler);
	};

}