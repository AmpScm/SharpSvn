#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "LookArgs/SvnLookClientArgs.h"

#include "AprBaton.h"

namespace SharpSvn {

	ref class SvnRemoteSessionOpenArgs;
	ref class SvnRemoteSessionReparentArgs;

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
		bool Open(Uri^ sessionUri);
		bool Open(Uri^ sessionUri, SvnRemoteSessionOpenArgs^ args);

	public:
		bool Reparent(Uri^ newSessionUri);
		bool Reparent(Uri^ newSessionUri, SvnRemoteSessionReparentArgs^ args);
	};

}