// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include <malloc.h>
#include "SvnAll.h"
#include "SvnTools.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using System::Text::StringBuilder;
using namespace System::IO;

Uri^ SvnTools::GetUriFromWorkingCopy(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	path = SvnTools::GetNormalizedFullPath(path);

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

	if(root && path->Length > root->Length && path[path->Length-1] == '\\')
	{
		path = path->TrimEnd('\\');

		if(path->Length <= root->Length)
			return root;
	}

	return FindTruePath(path, root);
}

String^ SvnTools::GetFullTruePath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	path = Path::GetFullPath(path);
	String^ root = Path::GetPathRoot(path);

	if(root->StartsWith("\\\\", StringComparison::OrdinalIgnoreCase))
		root = root->ToLowerInvariant();
	else if(wchar_t::IsLower(root, 0))
		root = root->ToUpperInvariant();

	return FindTruePath(path, root);
}

String^ SvnTools::FindTruePath(String^ path, String^ root)
{
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

	while(0 < (i = path->IndexOf('\\', nStart)))
	{
		if(IsDirectory(Path::Combine(path->Substring(0, i), SvnClient::AdministrativeDirectoryName)))
			return true;

		nStart = i+1;
	}

	if(nStart >= (path->Length-1))
		return false; // Path ends with a \, probably disk root

	return IsDirectory(Path::Combine(path, SvnClient::AdministrativeDirectoryName));
}

static String^ LongGetFullPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	// Subversion does not have a problem with paths over MAX_PATH, as long as they are absolute
	if (!path->StartsWith("\\\\?\\"))
		path = "\\\\?\\" + path;

	pin_ptr<const wchar_t> pPath = PtrToStringChars(path);
	wchar_t rPath[1024];

	ZeroMemory(rPath, sizeof(rPath));
	const int sz = (sizeof(rPath) / sizeof(rPath[0]))-1;

	unsigned c = GetFullPathNameW((LPCWSTR)pPath, sz, rPath, nullptr);

	if(c == 0 || c >= sz)
		throw gcnew PathTooLongException("GetFullPath for long paths failed");

	path = gcnew String(rPath, 0, c);

	if (path->StartsWith("\\\\?\\"))
		path = path->Substring(4);

	return path;
}

String^ SvnTools::GetNormalizedFullPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	try
	{
		path = Path::GetFullPath(path);
	}
	catch(PathTooLongException^)
	{
		// Path::GetFullPath() checked for strange characters for us!
		// It would have throwed an ArgumentException for them

		if (IsNormalizedFullPath(path))
			return path; // Just pass through

		try
		{
			path = LongGetFullPath(path);
		}
		catch(Exception^)
		{
			path = nullptr;
		}

		if(!path)
			throw;
	}

	if(path->Length >= 2 && path[1] == ':')
	{
		wchar_t c = path[0];

		if((c >= 'a') && (c <= 'z'))
			path = wchar_t::ToUpperInvariant(c) + path->Substring(1);

		String^ r = path->TrimEnd('\\');

		if(r->Length > 3)
			return r;
		else
			return path->Substring(0, 3);
	}
	else if(path->StartsWith("\\\\", StringComparison::OrdinalIgnoreCase))
	{
		String^ root = Path::GetPathRoot(path)->ToLowerInvariant();
	
		if(!path->StartsWith(root, StringComparison::Ordinal))
			path = root + path->Substring(root->Length)->TrimEnd('\\');
	}
	else
		path = path->TrimEnd('\\');

	return path;
}

static bool IsSeparator(String^ v, int index)
{
	if (index < 0 || (index >= v->Length))
		return false;

	wchar_t c = v[index];

	return (c == Path::DirectorySeparatorChar) || (c == Path::AltDirectorySeparatorChar);
}

static bool IsDirSeparator(String^ v, int index)
{
	if (index < 0 || (index >= v->Length))
		return false;

	return (v[index] == Path::DirectorySeparatorChar);
}

static bool IsInvalid(String^ v, int index)
{
	if (index < 0 || (index >= v->Length))
		return false;

	return 0 <= Array::IndexOf(Path::InvalidPathChars, v[index]);
}

bool SvnTools::IsAbsolutePath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentException("path");

	int c = path->Length;

	if(path->Length < 3)
		return false;

	int i, n;
	if (IsSeparator(path, 0))
	{
		if(!IsSeparator(path, 1))
			return false;

		for(i = 2; i < path->Length; i++)
		{
			if (!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if (i == 2 || !IsSeparator(path, i))
			return false;

		i++;

		n = i;

		for(; i < path->Length; i++)
		{
			if(!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if(i == n || !IsSeparator(path, i))
			return false;

		i++;
	}
	else if ((path[1] != ':') || !IsSeparator(path, 2))
		return false;
	else if (!(((path[0] >= 'A') && (path[0] <= 'Z')) || ((path[0] >= 'a') && (path[0] <= 'z'))))
		return false;
	else
		i = 3;	

	while(i < c)
	{
		if (i >= c && IsSeparator(path, i))
			return false; // '\'-s behind each other
		
		if(i < c && path[i] == '.')
		{
			int n = i;
			n++;

			if(n < c && path[n] == '.')
				n++;

			if (IsSeparator(path, n))
				return false; // '\'-s behind each other
		}

		n = i;

		while (i < c && !IsInvalid(path, i) && !IsSeparator(path, i))
			i++;

		if (n == i)
			return false;
		else if (i == c)
			return true;
		else if (!IsSeparator(path, i++))
			return false;

		if(i == c)
			return false; // We don't like paths with a '\' at the end
	}	

	return true;
}

bool SvnTools::IsNormalizedFullPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentException("path");

	int c = path->Length;

	if(path->Length < 3)
		return false;

	int i, n;
	if (IsDirSeparator(path, 0))
	{
		if(!IsDirSeparator(path, 1))
			return false;

		for(i = 2; i < path->Length; i++)
		{
			if (!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if (i == 2 || !IsDirSeparator(path, i))
			return false;

		i++;

		n = i;

		for(; i < path->Length; i++)
		{
			if(!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if(i == n || !IsDirSeparator(path, i))
			return false;

		i++;
	}
	else if ((path[1] != ':') || !IsDirSeparator(path, 2))
		return false;
	else if (!((path[0] >= 'A') && (path[0] <= 'Z')))
		return false;
	else
		i = 3;	

	while(i < c)
	{
		if (i >= c && IsDirSeparator(path, i))
			return false; // '\'-s behind each other
		
		if(i < c && path[i] == '.')
		{
			int n = i;

			while(n < c && path[n] == '.')
				n++;
			
			if (IsSeparator(path, n) || n >= c)
				return false; // Relative path
		}

		n = i;

		while (i < c && !IsInvalid(path, i) && !IsDirSeparator(path, i) && path[i] != Path::AltDirectorySeparatorChar)
			i++;

		if (n == i)
			return false;
		else if (i == c)
			return true;
		else if (!IsDirSeparator(path, i++))
			return false;

		if(i == c)
			return false; // We don't like paths with a '\' at the end
	}	

	return true;
}

Uri^ SvnTools::AppendPathSuffix(Uri^ uri, String^ pathSuffix)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (String::IsNullOrEmpty(pathSuffix))
		throw gcnew ArgumentNullException("pathSuffix");

	Uri^ suffixUri = SvnBase::PathToUri(pathSuffix);

	return gcnew Uri(uri, suffixUri);
}

String^ SvnTools::UriPartToPath(String^ stringToUnescape)
{
	if (!stringToUnescape)
		throw gcnew ArgumentNullException("stringToUnescape");

	return Uri::UnescapeDataString(stringToUnescape)->Replace('/', Path::DirectorySeparatorChar);
}

Uri^ SvnTools::GetNormalizedUri(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!uri->IsAbsoluteUri)
		throw gcnew ArgumentOutOfRangeException("uri");

	return SvnBase::CanonicalizeUri(uri);
}

Uri^ SvnTools::PathToRelativeUri(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return SvnBase::PathToUri(path);
}