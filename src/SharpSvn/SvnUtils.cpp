// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include <malloc.h>
#include "SvnAll.h"
#include "SvnUtils.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using System::Text::StringBuilder;
using namespace System::IO;

Uri^ SvnTools::GetUriFromWorkingCopy(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	path = System::IO::Path::GetFullPath(path);

	AprPool pool(SmallThreadPool);

	const char* url = nullptr;

	svn_error_t* err = svn_client_url_from_path(&url, pool.AllocPath(path), pool.Handle);

	if (!err && url)
		return Utf8_PtrToUri(url, System::IO::Directory::Exists(path) ? SvnNodeKind::Directory : SvnNodeKind::File);
	else if(err)
		svn_error_clear(err);

	return nullptr;
}

String^ SvnTools::GetTruePath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	int n = path->IndexOf('/');
	if (n >= 0)
		path = path->Replace('/', '\\');

	String^ root = nullptr;
	wchar_t c = path[0];

	if (c == '\\' && path->StartsWith("\\\\?\\", StringComparison::Ordinal))
		path = path->Substring(4); // We use this trick ourselves

	if (path->Length > 2 && (path[1] == ':') && ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
	{
		if ((path[2] == '\\') && !path->Contains("\\."))
			root = Path::GetPathRoot(path)->ToUpperInvariant(); // 'a:\' -> 'A:\'
	}
	else if (path->StartsWith("\\\\", StringComparison::Ordinal))
	{		
		int next = path->IndexOf('\\', 2);

		if (next > 0)
			next = path->IndexOf('\\', next+1);

		if ((next > 0) && (0 > path->IndexOf("\\.", next+1, StringComparison::Ordinal)))
			root = Path::GetPathRoot(path);
	}

	if (!root)
	{
		if(path[0] == '\\' || path->Contains("\\."))
		{
			path = Path::GetFullPath(path);
			root = Path::GetPathRoot(path); // UNC paths are not case sensitive, but we keep the casing
		}
		else
			root = "";
	}

	// Okay, now we have a normalized path and it's root in normal form. Now we need to find the exact casing of the next parts
	StringBuilder^ result = gcnew StringBuilder(root, path->Length + 16);

	pin_ptr<const wchar_t> pChars = PtrToStringChars(path);
	size_t len = sizeof(wchar_t)* (path->Length+1+4);
	wchar_t* pSec = (wchar_t*)_alloca(len);

	if(wcscpy_s(pSec, len, L"\\\\?\\") || wcscat_s(pSec, len, pChars))
		return nullptr;

	wchar_t *pTxt = &pSec[4]; // The point after '\\?\'

	int nStart = root->Length;
	bool isFirst = true;

	while(nStart < path->Length)
	{
		WIN32_FIND_DATAW filedata;

		int nNext = path->IndexOf('\\', nStart);

		if(nNext > 0)
			pTxt[nNext] = 0; // Temporarily replace '\' with 0

		HANDLE hSearch = FindFirstFileW(pSec, &filedata);

		if(hSearch == INVALID_HANDLE_VALUE)
			return nullptr;

		if(!isFirst)
			result->Append((wchar_t)'\\');

		result->Append(gcnew String(filedata.cFileName));		

		GC::KeepAlive(::FindClose(hSearch)); // Close search request

		if(nNext < 0)
			break;
		else
			pTxt[nNext] = '\\'; // Revert 0 to '\'

		nStart = nNext+1;
		isFirst= false;
	}

	return result->ToString();
}

// Optimized version of Directory::Exists() with several security checks removed
inline bool IsDirectory(String^ path)
{
	pin_ptr<const wchar_t> p = PtrToStringChars(path);
	DWORD r = ::GetFileAttributesW(p);

	if(r == INVALID_FILE_ATTRIBUTES)
		return false;
	
	return (r & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool SvnTools::IsManagedPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	path = Path::Combine(path, SvnClient::AdministrativeDirectoryName);

	return IsDirectory(path);
}

bool SvnTools::IsBelowManagedPath(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	// We search from the root, instead of the other way around to optimize the disk cache
	// (We probably never need to access the disk when called a few times in a row)

	path = Path::GetFullPath(path);

	int nStart = Path::GetPathRoot(path)->Length;
	int i;

	while(0 >= (i = path->IndexOf('\\', nStart)))
	{
		if(IsDirectory(Path::Combine(path->Substring(0, i), SvnClient::AdministrativeDirectoryName)))
			return true;

		nStart = i+1;
	}

	if(nStart >= (path->Length-1))
		return false; // Path ends with a \, probably disk root

	return IsDirectory(Path::Combine(path, SvnClient::AdministrativeDirectoryName));
}