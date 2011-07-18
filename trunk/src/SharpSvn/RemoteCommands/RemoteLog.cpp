#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteLogArgs.h"
#include "Args/Log.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::Log(String^ relPath, SvnRevisionRange^ range, EventHandler<SvnRemoteLogEventArgs^>^ logHandler)
{
	if (!relPath)
		throw gcnew ArgumentNullException("relPath");
	else if (!range)
		throw gcnew ArgumentNullException("range");
	else if (!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	return Log(relPath, range, gcnew SvnRemoteLogArgs(), logHandler);
}

bool SvnRemoteSession::Log(String^ relPath, SvnRevisionRange^ range, SvnRemoteLogArgs^ args, EventHandler<SvnRemoteLogEventArgs^>^ logHandler)
{
	if (!relPath)
		throw gcnew ArgumentNullException("relPath");
	else if (!range)
		throw gcnew ArgumentNullException("range");

	return Log(NewSingleItemCollection(relPath), range, args, logHandler);
}


bool SvnRemoteSession::Log(ICollection<String^>^ relPaths, SvnRevisionRange^ range, EventHandler<SvnRemoteLogEventArgs^>^ logHandler)
{
	if (!relPaths)
		throw gcnew ArgumentNullException("relPaths");
	else if (!range)
		throw gcnew ArgumentNullException("range");
	else if (!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	return Log(relPaths, range, gcnew SvnRemoteLogArgs(), logHandler);
}

static svn_error_t *remote_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
	SvnRemoteSession^ client = AprBaton<SvnRemoteSession^>::Get((IntPtr)baton);

	SvnRemoteLogArgs^ args = dynamic_cast<SvnRemoteLogArgs^>(client->CurrentCommandArgs);
	AprPool aprPool(pool, false);
	if (!args)
		return nullptr;

	if (log_entry->revision == SVN_INVALID_REVNUM)
	{
		// This marks the end of logs at this level,
		args->_mergeLogLevel--;
		return nullptr;
	}

	SvnRemoteLogEventArgs^ e = gcnew SvnRemoteLogEventArgs(log_entry, args->_mergeLogLevel, %aprPool);

	if (log_entry->has_children)
		args->_mergeLogLevel++;

	try
	{
		args->OnLog(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Log receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ ex)
	{
		return SvnException::CreateExceptionSvnError("Log receiver", ex);
	}
	finally
	{
		e->Detach(false);
	}
}

bool SvnRemoteSession::Log(ICollection<String^>^ relPaths, SvnRevisionRange^ range, SvnRemoteLogArgs^ args, EventHandler<SvnRemoteLogEventArgs^>^ logHandler)
{
	if (!relPaths)
		throw gcnew ArgumentNullException("relPaths");
	else if (!range)
		throw gcnew ArgumentNullException("range");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!relPaths->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "relPaths");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	args->_mergeLogLevel = 0; // Clear log level

	if (logHandler)
		args->Log += logHandler;

	try
	{
		__int64 start_rev, end_rev;

		if (!InternalResolveRevision(range->StartRevision, args, start_rev) || !InternalResolveRevision(range->EndRevision, args, end_rev))
			return false;

		apr_array_header_t* retrieveProperties;

		if (args->RetrieveAllProperties)
			retrieveProperties = nullptr;
		else if (args->RetrievePropertiesUsed)
			retrieveProperties = AllocArray(args->RetrieveProperties, %pool);
		else
			retrieveProperties = svn_compat_log_revprops_in(pool.Handle);

		svn_error_t *r = svn_ra_get_log2(_session,
										 AllocRelpathArray(relPaths, %pool),
										 (svn_revnum_t)start_rev,
										 (svn_revnum_t)end_rev,
										 args->Limit,
										 args->RetrieveChangedPaths,
										 args->StrictNodeHistory,
										 args->RetrieveMergedRevisions,
										 retrieveProperties,
										 remote_log_handler,
										 (void*)_clientBaton->Handle,
										  pool.Handle);

		return args->HandleResult(this, r, relPaths);
	}
	finally
	{
		if (logHandler)
			args->Log -= logHandler;
	}
}

SvnRevisionPropertyNameCollection^ SvnRemoteLogArgs::RetrieveProperties::get()
{
	if (!_retrieveProperties)
	{
		_retrieveProperties = gcnew SvnRevisionPropertyNameCollection(false);
	}

	return _retrieveProperties;
}
