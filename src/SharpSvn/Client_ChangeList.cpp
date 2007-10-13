#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangeList(System.String,System.String,SharpSvn.SvnListChangeListArgs,System.Collections.Generic.IList`1<System.String>&):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.AddToChangeList(System.Collections.Generic.ICollection`1<System.String>,System.String,SharpSvn.SvnAddToChangeListArgs):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.RemoveFromChangeList(System.Collections.Generic.ICollection`1<System.String>,System.String,SharpSvn.SvnRemoveFromChangeListArgs):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.RemoveFromChangeList(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnRemoveFromChangeListArgs):System.Boolean")];

void SvnClient::AddToChangeList(String^ path, String^ changeList)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	AddToChangeList(NewSingleItemCollection(path), changeList, gcnew SvnAddToChangeListArgs());
}

bool SvnClient::AddToChangeList(String^ path, String^ changeList, SvnAddToChangeListArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return AddToChangeList(NewSingleItemCollection(path), changeList, args);
}

void SvnClient::AddToChangeList(ICollection<String^>^ paths, String^ changeList)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	AddToChangeList(paths, changeList, gcnew SvnAddToChangeListArgs());
}

bool SvnClient::AddToChangeList(ICollection<String^>^ paths, String^ changeList, SvnAddToChangeListArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);


	svn_error_t *r = svn_client_add_to_changelist(
		AllocPathArray(paths, %pool),
		pool.AllocString(changeList),
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

void SvnClient::RemoveFromChangeList(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	RemoveFromChangeList(NewSingleItemCollection(path), gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(String^ path, SvnRemoveFromChangeListArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoveFromChangeList(NewSingleItemCollection(path), args);
}

void SvnClient::RemoveFromChangeList(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	RemoveFromChangeList(paths, gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(ICollection<String^>^ paths, SvnRemoveFromChangeListArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_remove_from_changelist(
		AllocPathArray(paths, %pool),
		nullptr,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

void SvnClient::ListChangeList(String^ rootPath, String^ changeList, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!changeListHandler)
		throw gcnew ArgumentNullException("changeListHandler");

	ListChangeList(rootPath, changeList, gcnew SvnListChangeListArgs(), changeListHandler);
}

static svn_error_t *svnclient_changelist_handler(void *baton, const char *path)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnListChangeListArgs^ args = dynamic_cast<SvnListChangeListArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnCommitArgs
	if(args)
	{
		SvnListChangeListEventArgs^ e = gcnew SvnListChangeListEventArgs(path);
		try
		{
			args->OnListChangeList(e);

			if(e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
		}
		catch(Exception^ e)
		{
			AprPool^ thePool = gcnew AprPool(); // We have no error pool handler; just create a GC-able one; it is just for copying into the error

			return svn_error_create(SVN_ERR_CANCELLED, nullptr, thePool->AllocString(String::Concat("List receiver throwed exception: ", e->ToString())));
		}
		finally
		{
			e->Detach(false);
		}
	}

	return nullptr;
}


bool SvnClient::ListChangeList(String^ rootPath, String^ changeList, SvnListChangeListArgs^ args, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if(changeListHandler)
		args->ListChangeList += changeListHandler;
	try
	{
		svn_error_t* r = svn_client_get_changelist_streamy(
			svnclient_changelist_handler,
			(void*)_clientBatton->Handle,
			pool.AllocString(changeList),
			pool.AllocPath(rootPath),
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if(changeListHandler)
			args->ListChangeList -= changeListHandler;
	}
}


void SvnClient::GetChangeList(String^ rootPath, String^ changeList, [Out]IList<String^>^% list)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");

	GetChangeList(rootPath, changeList, gcnew SvnListChangeListArgs(), list);
}

bool SvnClient::GetChangeList(String^ rootPath, String^ changeList, SvnListChangeListArgs^ args, [Out]IList<String^>^% list)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	list = nullptr;

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_array_header_t* aprResult = nullptr;

	svn_error_t* r = svn_client_get_changelist(
		&aprResult,
		pool.AllocString(changeList),
		pool.AllocPath(rootPath),
		CtxHandle,
		pool.Handle);

	if(!r && aprResult)
	{
		AprArray<String^,AprCStrPathMarshaller^> paths(aprResult, %pool);

		list = safe_cast<IList<String^>^>(paths.ToArray());
	}

	return args->HandleResult(this, r);
}

void SvnClient::GetChangeList(String^ rootPath, String^ changeList, [Out]IList<SvnListChangeListEventArgs^>^% list)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");

	GetChangeList(rootPath, changeList, gcnew SvnListChangeListArgs(), list);
}

bool SvnClient::GetChangeList(String^ changeList, String^ rootPath, SvnListChangeListArgs^ args, [Out]IList<SvnListChangeListEventArgs^>^% list)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");

	InfoItemCollection<SvnListChangeListEventArgs^>^ results = gcnew InfoItemCollection<SvnListChangeListEventArgs^>();

	try
	{
		return ListChangeList(rootPath, changeList, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}