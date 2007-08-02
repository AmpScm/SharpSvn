#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Unlock(String^ target)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	array<String^>^ targets = gcnew array<String^>(1);
	targets[0] = target;

	Unlock(safe_cast<ICollection<String^>^>(targets), gcnew SvnUnlockArgs());
}

void SvnClient::Unlock(Uri^ target)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	array<String^>^ targets = gcnew array<String^>(1);
	targets[0] = target->ToString();

	Unlock(safe_cast<ICollection<String^>^>(targets), gcnew SvnUnlockArgs());
}

void SvnClient::Unlock(ICollection<String^>^ targets)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");

	Unlock(targets, gcnew SvnUnlockArgs());
}

bool SvnClient::Unlock(String^ target, SvnUnlockArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targets = gcnew array<String^>(1);
	targets[0] = target;

	return Unlock(safe_cast<ICollection<String^>^>(targets), args);
}

bool SvnClient::Unlock(Uri^ target, SvnUnlockArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targets = gcnew array<String^>(1);
	targets[0] = target->ToString();

	return Unlock(safe_cast<ICollection<String^>^>(targets), args);
}

bool SvnClient::Unlock(ICollection<String^>^ targets, SvnUnlockArgs^ args)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");


	array<String^>^ targetStrings = gcnew array<String^>(targets->Count);

	int i = 0;
	for each(String^ target in targets)
	{
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

	AprPool pool(_pool);
	_currentArgs = args;

	try
	{
		AprArray<String^, AprCStrMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);


		svn_error_t* err = svn_client_unlock(
			aprTargets->Handle,
			args->BreakLock,			
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}