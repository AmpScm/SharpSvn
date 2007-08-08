#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Lock(System.String,SharpSvn.SvnLockArgs):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Lock(System.String,System.String):System.Void")];

void SvnClient::Lock(String^ target, String^ comment)
{
	if(String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	Lock(NewSingleItemCollection(target), args);
}

void SvnClient::Lock(Uri^ target, String^ comment)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	Lock(NewSingleItemCollection(target), args);
}

void SvnClient::Lock(ICollection<String^>^ targets, String^ comment)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	Lock(targets, args);
}

void SvnClient::Lock(ICollection<Uri^>^ targets, String^ comment)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	Lock(targets, args);
}

bool SvnClient::Lock(String^ target, SvnLockArgs^ args)
{
	if(String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targets = gcnew array<String^>(1);
	targets[0] = target;

	return Lock(safe_cast<ICollection<String^>^>(targets), args);
}

bool SvnClient::Lock(Uri^ target, SvnLockArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targets = gcnew array<String^>(1);
	targets[0] = target->ToString();

	return Lock(safe_cast<ICollection<String^>^>(targets), args);
}

bool SvnClient::Lock(ICollection<String^>^ targets, SvnLockArgs^ args)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");


	array<String^>^ targetStrings = gcnew array<String^>(targets->Count);

	int i = 0;
	for each(String^ target in targets)
	{
		if(String::IsNullOrEmpty(target))
			throw gcnew ArgumentException("Target is null within targets", "targets");

		if(IsNotUri(target))
			targetStrings[i++] = GetSvnPath(target);
		else
		{
			Uri^ uri;

			if(!Uri::TryCreate(target, UriKind::Absolute, uri))
				throw gcnew ArgumentException("Invalid target Uri", "targets");

			targetStrings[i++] = uri->ToString();
		}
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(%_pool);
	_currentArgs = args;

	try
	{
		AprArray<String^, AprCStrMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);


		svn_error_t* err = svn_client_lock(
			aprTargets->Handle,
			pool.AllocString(args->Comment),
			args->StealLock,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

bool SvnClient::Lock(ICollection<Uri^>^ targets, SvnLockArgs^ args)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");


	array<String^>^ targetStrings = gcnew array<String^>(targets->Count);

	int i = 0;
	for each(Uri^ uri in targets)
	{
		if(!uri)
			throw gcnew ArgumentException("Uri is null within targets", "targets");

		targetStrings[i++] = uri->ToString();
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(%_pool);
	_currentArgs = args;

	try
	{
		AprArray<String^, AprCStrMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);


		svn_error_t* err = svn_client_lock(
			aprTargets->Handle,
			pool.AllocString(args->Comment),
			args->StealLock,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}