#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

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

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(%_pool);
	_currentArgs = args;
	try
	{
		svn_error_t *r = svn_client_add_to_changelist(
			AllocPathArray(paths, %pool),
			pool.AllocString(changeList),
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

void SvnClient::RemoveFromChangeList(String^ path, String^ changeList)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	RemoveFromChangeList(NewSingleItemCollection(path), changeList, gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(String^ path, String^ changeList, SvnRemoveFromChangeListArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoveFromChangeList(NewSingleItemCollection(path), changeList, args);
}

void SvnClient::RemoveFromChangeList(ICollection<String^>^ paths, String^ changeList)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	RemoveFromChangeList(paths, changeList, gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(ICollection<String^>^ paths, String^ changeList, SvnRemoveFromChangeListArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(%_pool);
	_currentArgs = args;
	try
	{
		svn_error_t *r = svn_client_remove_from_changelist(
			AllocPathArray(paths, %pool),
			pool.AllocString(changeList),
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

void SvnClient::ListChangeList(String^ changeList, String^ rootPath, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!changeListHandler)
		throw gcnew ArgumentNullException("changeListHandler");

	ListChangeList(changeList, rootPath, gcnew SvnListChangeListArgs(), changeListHandler);
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
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, NULL, "List receiver canceled operation");
		}
		catch(Exception^ e)
		{
			AprPool^ thePool = gcnew AprPool(); // We have no error pool handler; just create a GC-able one; it is just for copying into the error

			return svn_error_create(SVN_ERR_CANCELLED, NULL, thePool->AllocString(String::Concat("List receiver throwed exception: ", e->ToString())));
		}
		finally
		{
			e->Detach(false);
		}
	}

	return nullptr;
}


bool SvnClient::ListChangeList(String^ changeList, String^ rootPath, SvnListChangeListArgs^ args, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(%_pool);
	_currentArgs = args;
	if(changeListHandler)
		args->ListChangeList += changeListHandler;
	try
	{
		svn_error_t* err = svn_client_get_changelist_streamy(
			svnclient_changelist_handler,
			(void*)_clientBatton->Handle,
			pool.AllocString(changeList),
			pool.AllocPath(rootPath),
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;

		if(changeListHandler)
			args->ListChangeList -= changeListHandler;
	}
}


void SvnClient::GetChangeList(String^ changeList, String^ rootPath, [Out]IList<String^>^% list)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");

	GetChangeList(changeList, rootPath, gcnew SvnListChangeListArgs(), list);
}

bool SvnClient::GetChangeList(String^ changeList, String^ rootPath, SvnListChangeListArgs^ args, [Out]IList<String^>^% list)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	list = nullptr;

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(%_pool);
	_currentArgs = args;
	try
	{
		apr_array_header_t* aprResult = nullptr;

		svn_error_t* err = svn_client_get_changelist(
			&aprResult,
			pool.AllocString(changeList),
			pool.AllocPath(rootPath),
			CtxHandle,
			pool.Handle);

		if(!err && aprResult)
		{
			AprArray<String^,AprCStrPathMarshaller^> paths(aprResult, %pool);

			list = safe_cast<IList<String^>^>(paths.ToArray());
		}

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

void SvnClient::GetChangeList(String^ changeList, String^ rootPath, [Out]IList<SvnListChangeListEventArgs^>^% list)
{
	if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");

	GetChangeList(changeList, rootPath, gcnew SvnListChangeListArgs(), list);
}

bool SvnClient::GetChangeList(String^ changeList, String^ rootPath, SvnListChangeListArgs^ args, [Out]IList<SvnListChangeListEventArgs^>^% list)
{
	InfoItemList<SvnListChangeListEventArgs^>^ results = gcnew InfoItemList<SvnListChangeListEventArgs^>();

	try
	{
		return ListChangeList(changeList, rootPath, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}