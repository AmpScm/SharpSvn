// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Log.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.String,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.Uri,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#Log(System.String,SharpSvn.SvnLogArgs,System.EventHandler`1<SharpSvn.SvnLogEventArgs>)")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#GetLog(System.String,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)")];
[module: SuppressMessage("Microsoft.Usage", "CA2234:PassSystemUriObjectsInsteadOfStrings", Scope="member", Target="SharpSvn.SvnClient.#Log(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnLogArgs,System.EventHandler`1<SharpSvn.SvnLogEventArgs>)")];

bool SvnClient::Log(String^ targetPath, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	return Log(targetPath, gcnew SvnLogArgs(), logHandler);
}

bool SvnClient::Log(Uri^ target, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	return Log(target, gcnew SvnLogArgs(), logHandler);
}


bool SvnClient::Log(Uri^ target, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return InternalLog(NewSingleItemCollection(UriToString(target)), nullptr, args, logHandler);
}

bool SvnClient::Log(String^ targetPath, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!IsNotUri(targetPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPath");

	return InternalLog(NewSingleItemCollection(
		SvnPathTarget::GetTargetPath(targetPath)->Replace(System::IO::Path::DirectorySeparatorChar, '/')),
		nullptr, args, logHandler);
}

bool SvnClient::Log(ICollection<Uri^>^ targets, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	Uri^ first = nullptr;
	String^ root = nullptr;
	bool moreThanOne = false;
	for each (Uri^ uri in targets)
	{
		if (!uri)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");

		if (!uri->IsAbsoluteUri)
		{
			if (args->BaseUri)
				uri = gcnew Uri(args->BaseUri, uri);

			if (!uri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "targets");
		}

		if (!first)
		{
			first = uri;
			root = uri->AbsolutePath->TrimEnd('/');
			continue;
		}
		else
			moreThanOne = true;

		if (Uri::Compare(uri, first, UriComponents::HostAndPort | UriComponents::Scheme | UriComponents::StrongAuthority, UriFormat::UriEscaped, StringComparison::Ordinal))
			throw gcnew ArgumentException(SharpSvnStrings::AllUrisMustBeOnTheSameServer, "targets");

		String^ itemPath = uri->AbsolutePath->TrimEnd('/');

		int nEnd = Math::Min(root->Length, itemPath->Length);

		while (nEnd >= 0 && String::Compare(root, 0, itemPath, 0, nEnd, StringComparison::Ordinal))
		{
			nEnd = root->LastIndexOf('/', nEnd-1);
		}

		if (nEnd == root->Length && itemPath->Length > root->Length && itemPath[nEnd] != '/')
		{	// root can't end in a '/', as that would have been trimmed !

			nEnd = root->LastIndexOf('/', nEnd-1);
		}

		if (nEnd >= root->Length - 1)
		{}
		else
		{
			while (nEnd > 1 && nEnd < root->Length && root[nEnd-1] == '/')
				nEnd--;

			if (nEnd > 1)
				root = root->Substring(0, nEnd);
			else
				root = "/";
		}
	}

	if (moreThanOne && !root->EndsWith("/", StringComparison::Ordinal))
		root += "/";

	System::Collections::Generic::List<String^>^ rawTargets = gcnew System::Collections::Generic::List<String^>();
	Uri^ rootUri = gcnew Uri(first, Uri::UnescapeDataString(root)); // URI .ctor escapes the 2nd parameter
	if (moreThanOne)
	{
		// Invoke with primary url followed by relative subpaths
		rawTargets->Add(UriToString(rootUri));

		for each (Uri^ uri in targets)
		{
			if (!uri->IsAbsoluteUri && args->BaseUri) // Allow relative Uri's relative from the first
				uri = gcnew Uri(args->BaseUri, uri);

			uri = rootUri->MakeRelativeUri(uri);

			if (uri->IsAbsoluteUri) // Should have happened before				
				throw gcnew ArgumentException(SharpSvnStrings::AllUrisMustBeOnTheSameServer, "targets");

			rawTargets->Add(UriToString(uri));
		}
	}
	else
	{
		rootUri = nullptr;
		rawTargets->Add(UriToString(first));
	}

	return InternalLog(static_cast<ICollection<String^>^>(rawTargets), rootUri, args, logHandler);
}

bool SvnClient::Log(ICollection<String^>^ targetPaths, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if (!targetPaths)
		throw gcnew ArgumentNullException("targetPaths");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!targetPaths->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "targetPaths");

	// This overload is not supported by the Subversion runtime at this time. We fake it useing the client api
	Uri^ first = nullptr;

	System::Collections::Generic::List<Uri^>^ targetsUris = gcnew System::Collections::Generic::List<Uri^>();

	for each (String^ path in targetPaths)
	{
		if (!path)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targetPaths");
		else if (!IsNotUri(path))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPaths");

		Uri^ uri = GetUriFromWorkingCopy(path);

		if (!uri)
		{
			ArgsStore store(this, args);

			// We must provide some kind of svn error, to eventually replace this method with real svn client code
			return args->HandleResult(this, svn_error_create(SVN_ERR_WC_NOT_DIRECTORY, nullptr, nullptr));
		}

		if (!first)
			first = uri;
		else if (Uri::Compare(uri, first, UriComponents::HostAndPort | UriComponents::Scheme | UriComponents::StrongAuthority, UriFormat::UriEscaped, StringComparison::Ordinal))
		{
			ArgsStore store(this, args);

			// We must provide some kind of svn error, to eventually replace this method with real svn client code
			return args->HandleResult(this, svn_error_create(SVN_ERR_WC_BAD_PATH, nullptr, "Working copy paths must be in the same repository"));

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
	if (!args)
		return nullptr;

	if (log_entry->revision == SVN_INVALID_REVNUM)
	{
		// This marks the end of logs at this level,
		args->_mergeLogLevel--;
		return nullptr;
	}

	SvnLogEventArgs^ e = gcnew SvnLogEventArgs(log_entry, args->_mergeLogLevel, args->_searchRoot, %aprPool);

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
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	args->_mergeLogLevel = 0; // Clear log level
	args->_searchRoot = searchRoot;
	if (logHandler)
		args->Log += logHandler;
	try
	{
		apr_array_header_t* retrieveProperties;

		if (args->RetrievePropertiesUsed)
			retrieveProperties = args->RetrieveProperties->Count ? AllocArray(args->RetrieveProperties, %pool) : nullptr;
		else
			retrieveProperties = svn_compat_log_revprops_in(pool.Handle);

		svn_opt_revision_t pegRev = args->OriginRevision->ToSvnRevision();
		svn_opt_revision_t start = args->Start->Or(args->OriginRevision)->Or(SvnRevision::Head)->ToSvnRevision();
		svn_opt_revision_t end = args->End->Or(SvnRevision::Zero)->ToSvnRevision();

		svn_error_t *r = svn_client_log4(
			AllocCanonicalArray(targets, %pool),
			&pegRev,
			&start,
			&end,
			args->Limit,
			args->RetrieveChangedPaths,
			args->StrictNodeHistory,
			args->RetrieveMergedRevisions,
			retrieveProperties,
			svnclient_log_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (logHandler)
			args->Log -= logHandler;

		args->_searchRoot = nullptr;
	}
}

bool SvnClient::GetLog(String^ targetPath, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(targetPath, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

bool SvnClient::GetLog(Uri^ target, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if (!target)
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
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
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
	if (!targetPath)
		throw gcnew ArgumentNullException("targetPath");
	else if (!args)
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
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
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
	if (!targetPaths)
		throw gcnew ArgumentNullException("targetPaths");
	else if (!args)
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

SvnRevisionPropertyNameCollection::SvnRevisionPropertyNameCollection(bool initialEmpty)
	: KeyedCollection<String^,String^>(StringComparer::Ordinal, 10) // Start using hashtable at 10 items
{
	if (!initialEmpty)
	{
		Add(SVN_PROP_REVISION_AUTHOR);
		Add(SVN_PROP_REVISION_DATE);
		Add(SVN_PROP_REVISION_LOG);
	}
}

void SvnRevisionPropertyNameCollection::AddDefaultProperties()
{
	if (!Contains(SVN_PROP_REVISION_AUTHOR))
		Add(SVN_PROP_REVISION_AUTHOR);

	if (!Contains(SVN_PROP_REVISION_DATE))
		Add(SVN_PROP_REVISION_DATE);

	if (!Contains(SVN_PROP_REVISION_LOG))
		Add(SVN_PROP_REVISION_LOG);
}

SvnRevisionPropertyNameCollection^ SvnLogArgs::RetrieveProperties::get()
{
	if (!_retrieveProperties)
	{
		_retrieveProperties = gcnew SvnRevisionPropertyNameCollection(false);
	}

	return _retrieveProperties;
}
