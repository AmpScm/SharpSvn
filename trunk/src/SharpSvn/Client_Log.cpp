// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnTarget,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnUriTarget,System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnUriTarget,System.Collections.Generic.ICollection`1<System.Uri>,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
bool SvnClient::Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	SvnLogArgs^ args = gcnew SvnLogArgs();
	args->OriginRevision = target->Revision;

	SvnUriTarget^ uriTarget = dynamic_cast<SvnUriTarget^>(target);	

	if(uriTarget)
		return Log(uriTarget->Uri, args, logHandler);
	else
		return Log(static_cast<SvnPathTarget^>(target)->Path, args, logHandler);
}

bool SvnClient::Log(Uri^ target, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return InternalLog(NewSingleItemCollection(CanonicalizeUri(target)->ToString()), nullptr, args, logHandler);
}

bool SvnClient::Log(String^ targetPath, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!IsNotUri(targetPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPath");

	return InternalLog(NewSingleItemCollection(CanonicalizePath(targetPath)), nullptr, args, logHandler);
}

bool SvnClient::Log(ICollection<Uri^>^ targets, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	Uri^ first = nullptr;
	String^ root = nullptr;
	bool moreThanOne = false;
	for each(Uri^ uri in targets)
	{
		if(!uri)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");

		if(!uri->IsAbsoluteUri)
		{
			if(args->BaseUri)
				uri = gcnew Uri(args->BaseUri, uri);

			if(!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "targets");
		}

		if(!first)
		{
			first = uri;
			root = uri->AbsolutePath->TrimEnd('/');
			continue;
		}

		if(Uri::Compare(uri, first, UriComponents::HostAndPort | UriComponents::Scheme | UriComponents::StrongAuthority, UriFormat::UriEscaped, StringComparison::Ordinal))
			throw gcnew ArgumentException(SharpSvnStrings::AllUrisMustBeOnTheSameServer, "targets");

		String^ itemPath = uri->AbsolutePath->TrimEnd('/');

		int nEnd = Math::Min(root->Length, itemPath->Length)-1;

		while(nEnd >= 0 && String::Compare(root, 0, itemPath, 0, nEnd))
		{
			nEnd = root->LastIndexOf('/', nEnd)-1;
		}

		if(nEnd >= root->Length - 1)
		{}
		else if(nEnd > 1)
			root = root->Substring(0, nEnd);
		else
			root = "/";
	}

	System::Collections::Generic::List<String^>^ rawTargets = gcnew System::Collections::Generic::List<String^>();
	Uri^ rootUri = gcnew Uri(first, root); 
	if(moreThanOne)
	{		
		// Invoke with primary url followed by relative subpaths
		rawTargets->Add(CanonicalizeUri(rootUri)->ToString());

		for each(Uri^ uri in targets)
		{
			if(!uri->IsAbsoluteUri && args->BaseUri) // Allow relative Uri's relative from the first
				uri = gcnew Uri(args->BaseUri, uri);

			uri = CanonicalizeUri(uri);

			rawTargets->Add(rootUri->MakeRelativeUri(uri)->ToString());
		}
	}
	else
		rawTargets->Add(CanonicalizeUri(first)->ToString());

	return InternalLog(safe_cast<ICollection<String^>^>(targets), rootUri, args, logHandler);
}

bool SvnClient::Log(ICollection<String^>^ targetPaths, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!targetPaths)
		throw gcnew ArgumentNullException("targetPaths");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!targetPaths->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "targets");

	// This overload is not supported by the Subversion runtime at this time. We fake it useing the client api
	Uri^ first = nullptr;

	System::Collections::Generic::List<Uri^>^ targetsUris = gcnew System::Collections::Generic::List<Uri^>();

	for each(String^ path in targetPaths)
	{
		if(!path)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targetPaths");
		else if(!IsNotUri(path))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPaths");

		Uri^ uri = GetUriFromWorkingCopy(path);

		if(!uri)
		{
			ArgsStore store(this, args);

			// HACK: We must provide some kind of svn error, to eventually replace this method with real svn client code
			return args->HandleResult(this, svn_error_create(SVN_ERR_WC_NOT_DIRECTORY, nullptr, nullptr));
		}

		if(!first)
			first = uri;
		else if(Uri::Compare(uri, first, UriComponents::HostAndPort | UriComponents::Scheme | UriComponents::StrongAuthority, UriFormat::UriEscaped, StringComparison::Ordinal))
		{
			// TODO: Give some kind of meaningfull error. We just ignore other repository paths in logging now
			continue;
		}

		targetsUris->Add(uri);
	}

	return Log(targetsUris, args, logHandler);
}

static svn_error_t *svnclient_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnLogArgs^ args = dynamic_cast<SvnLogArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnLogArgs
	AprPool aprPool(pool, false);
	if(!args)
		return nullptr;

	if(log_entry->revision == SVN_INVALID_REVNUM)
	{
		// This marks the end of logs at this level,
		args->_mergeLogLevel--;
		return nullptr;
	}

	SvnLogEventArgs^ e = gcnew SvnLogEventArgs(log_entry, args->_mergeLogLevel, %aprPool);

	if(log_entry->has_children)
		args->_mergeLogLevel++;

	try
	{
		args->OnLog(e);

		if(e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Log receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Log receiver", e);
	}
	finally
	{
		e->Detach(false);
	}
}

bool SvnClient::InternalLog(ICollection<String^>^ targets, Uri^ searchRoot, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	args->_mergeLogLevel = 0; // Clear log level
	args->_searchRoot = searchRoot;
	if(logHandler)
		args->Log += logHandler;
	try
	{
		apr_array_header_t* retrieveProperties;

		if(args->RetrievePropertiesUsed)
			retrieveProperties = AllocArray(args->RetrieveProperties, %pool);
		else
			retrieveProperties = svn_compat_log_revprops_in(pool.Handle);

		svn_opt_revision_t pegRev = args->OriginRevision->ToSvnRevision();
		svn_opt_revision_t start = args->Start->ToSvnRevision(SvnRevision::Head);
		svn_opt_revision_t end = args->End->ToSvnRevision(SvnRevision::Zero);

		svn_error_t *r = svn_client_log4(
			AllocArray(targets, %pool),
			&pegRev,
			&start,
			&end,
			args->Limit,
			args->LogChangedPaths,
			args->StrictNodeHistory,
			args->IncludeMergedRevisions,
			retrieveProperties,
			svnclient_log_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if(logHandler)
			args->Log -= logHandler;

		args->_searchRoot = nullptr;
	}
}

bool SvnClient::GetLog(SvnTarget^ target, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(target, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

bool SvnClient::GetLog(Uri^ target, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(target, args, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

bool SvnClient::GetLog(String^ targetPath, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!targetPath)
		throw gcnew ArgumentNullException("targetPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(targetPath, args, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}


bool SvnClient::GetLog(ICollection<Uri^>^ targets, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(targets, args, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

bool SvnClient::GetLog(ICollection<String^>^ targetPaths, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!targetPaths)
		throw gcnew ArgumentNullException("targetPaths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(targetPaths, args, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}