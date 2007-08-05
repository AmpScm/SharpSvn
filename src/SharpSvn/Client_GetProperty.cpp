#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::GetProperty(SvnTarget^ target, String^ propertyName, String^% value)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	IDictionary<SvnTarget^, String^>^ result = nullptr;
	value = nullptr;

	GetProperty(target, propertyName, gcnew SvnGetPropertyArgs(), result);

	if(result && (result->Count >= 0))
	{
		for each(String^ v in result->Values)
		{
			value = v;
			return;
		}
	}
}

void SvnClient::GetProperty(SvnTarget^ target, String^ propertyName, IList<char>^% bytes)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	IDictionary<SvnTarget^, String^>^ result = nullptr;
	bytes = nullptr;

	GetProperty(target, propertyName, gcnew SvnGetPropertyArgs(), result);

	if(result && (result->Count >= 0))
	{
		for each(IList<char>^ v in result->Values)
		{
			bytes = v;
			return;
		}
	}
}

bool SvnClient::GetProperty(SvnTarget^ target, String^ propertyName, SvnGetPropertyArgs^ args, IDictionary<SvnTarget^, String^>^% properties)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	properties = nullptr;
	EnsureState(SvnContextState::AuthorizationInitialized);	

	AprPool pool(_pool);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	_currentArgs = args;

	try
	{
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();
		svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
		svn_revnum_t actualRev = 0;

		apr_hash_t* pHash = nullptr;

		svn_error_t *r = svn_client_propget3(
			&pHash,			
			pool.AllocString(propertyName),
			pool.AllocString(target->TargetName),
			&pegRev,
			&rev,
			&actualRev,
			IsRecursive(args->Depth),
			CtxHandle,
			pool.Handle);

		if(pHash)
		{
			int count = apr_hash_count(pHash);

			Dictionary<SvnTarget^, String^>^ rd = gcnew Dictionary<SvnTarget^, String^>(count);

			for(apr_hash_index_t* hi = apr_hash_first(pool.Handle, pHash); hi ; hi = apr_hash_next(hi))
			{
				const char* pKey;
				apr_ssize_t keyLen;
				const svn_string_t *propVal;

				apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&propVal);

				rd->Add(SvnTarget::FromString(Utf8_PtrToString(pKey, keyLen)), Utf8_PtrToString(propVal->data, propVal->len));
			}

			properties = safe_cast<IDictionary<SvnTarget^, String^>^>(rd);
		}

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

bool SvnClient::GetProperty(SvnTarget^ target, String^ propertyName, SvnGetPropertyArgs^ args, IDictionary<SvnTarget^, IList<char>^>^% properties)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	properties = nullptr;
	EnsureState(SvnContextState::AuthorizationInitialized);	

	AprPool pool(_pool);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	_currentArgs = args;

	try
	{
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();
		svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();

		apr_hash_t* pHash = nullptr;
		svn_revnum_t actualRev;

		svn_error_t *r = svn_client_propget3(
			&pHash,			
			pool.AllocString(propertyName),
			pool.AllocString(target->TargetName),
			&pegRev,
			&rev,
			&actualRev,
			IsRecursive(args->Depth),
			CtxHandle,
			pool.Handle);

		if(pHash)
		{
			int count = apr_hash_count(pHash);

			Dictionary<SvnTarget^, IList<char>^>^ rd = gcnew Dictionary<SvnTarget^, IList<char>^>(count);

			for(apr_hash_index_t* hi = apr_hash_first(pool.Handle, pHash); hi ; hi = apr_hash_next(hi))
			{
				const char* pKey;
				apr_ssize_t keyLen;
				const svn_string_t *propVal;

				apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&propVal);

				IList<char>^ list = safe_cast<IList<char>^>(PtrToByteArray(propVal->data, propVal->len));
				rd->Add(SvnTarget::FromString(Utf8_PtrToString(pKey, keyLen)), list);
			}

			properties = safe_cast<IDictionary<SvnTarget^, IList<char>^>^>(rd);
		}

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

bool SvnClient::TryGetProperty(SvnTarget^ target, String^ propertyName, String^% value)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	IDictionary<SvnTarget^, String^>^ result = nullptr;
	value = nullptr;

	SvnGetPropertyArgs^ args = gcnew SvnGetPropertyArgs();
	args->ThrowOnError = false;

	if(!GetProperty(target, propertyName, args, result))
		return false;

	if(result && (result->Count >= 0))
	{
		for each(String^ v in result->Values)
		{
			value = v;
			return true;
		}
	}

	return false;
}