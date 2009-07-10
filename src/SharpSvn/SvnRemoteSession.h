#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "LookArgs/SvnLookClientArgs.h"

#include "AprBaton.h"

namespace SharpSvn {

	ref class SvnRemoteSessionOpenArgs;
	ref class SvnRemoteSessionReparentArgs;
	ref class SvnRemoteSessionLatestRevisionArgs;
	ref class SvnRemoteSessionLogArgs;

	public ref class SvnRemoteSession : public SvnClientContext
	{
		initonly AprBaton<SvnRemoteSession^>^ _clientBaton;
		AprPool _pool;
		svn_ra_session_t *_session;
		bool _cbInitialized;
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
		bool Open(Uri^ sessionUri, SvnRemoteSessionOpenArgs^ args);

	public:
		/// <overloads>Reparents the session to the specified Uri</overloads>
		/// <summary>Reparents the session to the specified Uri in the same repository</summary>
		bool Reparent(Uri^ newSessionUri);
		/// <summary>Reparents the session to the specified Uri in the same repository</summary>
		bool Reparent(Uri^ newSessionUri, SvnRemoteSessionReparentArgs^ args);

	public:
		/// <overloads>Get the latest revision in the repository</overloads>
		/// <summary>Get the latest revision in the repository</summary>
		bool GetLatestRevision([Out] __int64% revision);
		/// <summary>Get the latest revision in the repository</summary>
		bool GetLatestRevision(SvnRemoteSessionLatestRevisionArgs^ args, [Out] __int64% revision);

	public:
		/// <overloads>Streamingly retrieve the log messages for a set of revision(s)</overloads>
		/// <summary>Streamingly retrieve the log messages for a set of revision(s).</summary>
		bool Log(ICollection<String^>^ paths, EventHandler<SvnRemoteSessionLogEventArgs^>^ logHandler);
		/// <summary>Streamingly retrieve the log messages for a set of revision(s).</summary>
		bool Log(ICollection<String^>^ paths, SvnRemoteSessionLogArgs^ args, EventHandler<SvnRemoteSessionLogEventArgs^>^ logHandler);
	};

}