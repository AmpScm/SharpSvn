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


[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Unlock(System.String):System.Void")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Unlock(System.String,SharpSvn.SvnUnlockArgs):System.Boolean")];

void SvnClient::Unlock(String^ target)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	Unlock(NewSingleItemCollection(target), gcnew SvnUnlockArgs());
}

void SvnClient::Unlock(Uri^ target)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	Unlock(NewSingleItemCollection(target), gcnew SvnUnlockArgs());
}

void SvnClient::Unlock(ICollection<String^>^ targets)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");

	Unlock(targets, gcnew SvnUnlockArgs());
}

void SvnClient::Unlock(ICollection<Uri^>^ targets)
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

	return Unlock(NewSingleItemCollection(target), args);
}

bool SvnClient::Unlock(Uri^ target, SvnUnlockArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return Unlock(NewSingleItemCollection(target), args);
}

bool SvnClient::Unlock(ICollection<String^>^ targets, SvnUnlockArgs^ args)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targetStrings = gcnew array<String^>(targets->Count);

	int i = 0;
	bool checked = false;
	bool allUri = false;
	for each(String^ target in targets)
	{
		if(String::IsNullOrEmpty(target))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");

		bool isUri = IsNotUri(target);
		if(checked && (isUri != allUri))
			throw gcnew ArgumentException(SharpSvnStrings::AllTargetsMustBeUriOrPath, "targets");
		else
		{
			checked = true;
			allUri = isUri;
		}

		if(isUri)
		{
			Uri^ uri;

			if(!Uri::TryCreate(target, UriKind::Absolute, uri))
				throw gcnew ArgumentException(SharpSvnStrings::InvalidUri, "targets");
			else if(!SvnBase::IsValidReposUri(uri))
				throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "targets");

			targetStrings[i++] = uri->ToString();
		}
		else
			targetStrings[i++] = target;
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCanonicalMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCanonicalMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);


	svn_error_t* r = svn_client_unlock(
		aprTargets->Handle,
		args->BreakLock,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

bool SvnClient::Unlock(ICollection<Uri^>^ targets, SvnUnlockArgs^ args)
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
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
		else if(!SvnBase::IsValidReposUri(uri))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "targets");

		targetStrings[i++] = uri->ToString();
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);


	svn_error_t* r = svn_client_unlock(
		aprTargets->Handle,
		args->BreakLock,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}