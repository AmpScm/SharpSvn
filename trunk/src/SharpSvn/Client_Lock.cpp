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
	bool allUri = false;
	bool checked = false;
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

			targetStrings[i++] = uri->ToString();
		}
		else
			targetStrings[i++] = target;
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCanonicalMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCanonicalMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);


	svn_error_t* r = svn_client_lock(
		aprTargets->Handle,
		pool.AllocUnixString(args->Comment),
		args->StealLock,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
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
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");

		targetStrings[i++] = uri->ToString();
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);


	svn_error_t* r = svn_client_lock(
		aprTargets->Handle,
		pool.AllocString(args->Comment),
		args->StealLock,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}