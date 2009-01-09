// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using System::Collections::Generic::List;

bool SvnClientArgs::HandleResult(SvnClientContext^ client, svn_error_t *error)
{
	// Note: client can be null if called from a not SvnClient command
	if (!error)
	{
		_exception = nullptr;
		return true;
	}

	apr_status_t err = error->apr_err;

	_exception = SvnException::Create(error);

	if (err == SVN_ERR_CEASE_INVOCATION)
		return false;

	return HandleResult(client, _exception); // Releases error
}

bool SvnClientArgs::HandleResult(SvnClientContext^ client, SvnException^ exception)
{
	_exception = exception;

	if (!_exception)
		return true;

	if (_exception->SubversionErrorCode == SVN_ERR_CEASE_INVOCATION)
		return false;

	SvnClient^ svnClient = dynamic_cast<SvnClient^>(client);

	SvnErrorEventArgs^ ea = gcnew SvnErrorEventArgs(_exception);

	if (svnClient)
	{
		svnClient->HandleClientError(ea);
	}
	else
		OnSvnError(ea);

	if (ea->Cancel)
		return false;

	if (_expectedErrors && 0 <= Array::IndexOf(_expectedErrors, _exception->SvnErrorCode))
		return false;
	else if (_expectedErrorCategories && 0 <= Array::IndexOf(_expectedErrorCategories, _exception->SvnErrorCategory))
		return false;
	if (!ThrowOnCancel && _exception->SubversionErrorCode == SVN_ERR_CANCELLED)
		return false;
	else if (ThrowOnError)
		throw _exception;
	else
		return false;
}

void SvnClientArgs::Prepare()
{
	LastException = nullptr;
	_hooked = true;
}

bool SvnClientArgs::IsLastInvocationCanceled::get()
{
	return _exception && dynamic_cast<SvnOperationCompletedException^>(_exception);
}

void SvnClientArgs::AddExpectedError(SvnErrorCode errorCode)
{
	List<SvnErrorCode>^ items = gcnew List<SvnErrorCode>();

	if(_expectedErrors)
		items->AddRange(_expectedErrors);

	items->Add(errorCode);

	_expectedErrors = items->ToArray();
}

void SvnClientArgs::AddExpectedError(... array<SvnErrorCode>^ errorCodes)
{
	if (!errorCodes || !errorCodes->Length)
		return;

	List<SvnErrorCode>^ items = gcnew List<SvnErrorCode>();

	if(_expectedErrors)
		items->AddRange(_expectedErrors);

	items->AddRange(errorCodes);

	_expectedErrors = items->ToArray();
}

void SvnClientArgs::AddExpectedError(SvnErrorCategory errorCategory)
{
	List<SvnErrorCategory>^ items = gcnew List<SvnErrorCategory>();

	if(_expectedErrorCategories)
		items->AddRange(_expectedErrorCategories);

	items->Add(errorCategory);

	_expectedErrorCategories = items->ToArray();
}

void SvnClientArgs::AddExpectedError(... array<SvnErrorCategory>^ errorCategories)
{
	if (!errorCategories || !errorCategories->Length)
		return;

	List<SvnErrorCategory>^ items = gcnew List<SvnErrorCategory>();

	if(_expectedErrorCategories)
		items->AddRange(_expectedErrorCategories);

	items->AddRange(errorCategories);

	_expectedErrorCategories = items->ToArray();
}
