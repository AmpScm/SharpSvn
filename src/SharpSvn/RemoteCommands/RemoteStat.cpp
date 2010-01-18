#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteStatArgs.h"
#include "EventArgs/SvnRemoteStatEventArgs.h"


using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetStat(String^ relPath, [Out] SvnRemoteStatEventArgs^% result)
{
	return GetStat(relPath, gcnew SvnRemoteStatArgs(), result);
}

bool SvnRemoteSession::GetStat(String^ relPath, SvnRemoteStatArgs^ args, [Out] SvnRemoteStatEventArgs^% result)
{
	Uri^ uri;
	if (!args)
		throw gcnew ArgumentNullException("args");

	if (String::IsNullOrEmpty(relPath))
		relPath = "";
	else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
		throw gcnew ArgumentException("Not a valid relative path", "relPath");

	Ensure();
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	result = nullptr;

	const char *path = pool.AllocCanonical(relPath);
	svn_dirent_t *dirent = nullptr;
	svn_error_t *err = svn_ra_stat(_session,
								   path,
								   (svn_revnum_t)args->Revision,
								   &dirent,
								   pool.Handle);

	if (err && err->apr_err == SVN_ERR_RA_NOT_IMPLEMENTED)
	{
		// TODO: Handle SvnServe 1.0 and 1.1:
	}

	SVN_HANDLE(err);

	if (dirent)
	{
		result = gcnew SvnRemoteStatEventArgs(dirent);
		return true;
	}

	return false;
}