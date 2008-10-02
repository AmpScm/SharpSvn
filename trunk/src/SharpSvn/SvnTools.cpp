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
	else if (err)
		svn_error_clear(err);

	return nullptr;
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

	if (c == 0 || c >= sz)
		throw gcnew PathTooLongException("GetFullPath for long paths failed");

	path = gcnew String(rPath, 0, c);

	if (path->StartsWith("\\\\?\\"))
		path = path->Substring(4);

	return path;
}

static bool ContainsRelative(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	int nxt = 0;
	int n;

	while (0 <= (n = path->IndexOf("\\.", nxt)))
	{
		nxt+=2;

		if (nxt >= path->Length || !Char::IsLetterOrDigit(path[nxt]))
			return true;
	}

	return false;
}

static String^ GetPathRootPart(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	if (path->Length >= 3 && path[1] == ':' && path[2] == '\\')
	{
		if (path[0] >= 'a' && path[0] <= 'z')
			return Char::ToUpper(path[0]) + ":\\";
		else
			return path->Substring(0, 3);
	}
	
	if (!path->StartsWith("\\\\"))
		return nullptr;

	int nSlash = path->IndexOf('\\', 2);

	if (nSlash <= 2)
		return nullptr; // No hostname

	int nEnd = path->IndexOf('\\', nSlash+1);

	if (nEnd < 0)
		nEnd = path->Length;

	return "\\\\" + path->Substring(2, nSlash-2)->ToLowerInvariant() + path->Substring(nSlash, nEnd-nSlash);
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

	array<Char>^ invalidChars = SvnBase::_invalidChars;

	if (!invalidChars)
		SvnBase::_invalidChars = invalidChars = Path::GetInvalidPathChars();

	return 0 <= Array::IndexOf(invalidChars, v[index]);
}

bool SvnBase::PathContainsInvalidChars(String^ path)
{
	array<Char>^ invalidChars = _invalidChars;

	if (!invalidChars)
		_invalidChars = invalidChars = Path::GetInvalidPathChars();

	if (0 <= path->IndexOfAny(invalidChars))
		return true;

	return false;
}

String^ SvnTools::GetTruePath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	int n = path->IndexOf('/');
	if (n >= 0)
		path = path->Replace('/', '\\');

	String^ root = nullptr;
	bool normalized = false;;
	wchar_t c = path[0];

	if (c == '\\' && path->StartsWith("\\\\?\\", StringComparison::Ordinal))
		path = path->Substring(4); // We use this trick ourselves

    if (ContainsRelative(path))
	{
		normalized = true;
        path = GetNormalizedFullPath(path);
	}

	root = GetPathRootPart(path);

	if (!root && !normalized)
	{
		normalized = true;
        path = GetNormalizedFullPath(path);

		root = GetPathRootPart(path);
	}

	if (!root)
		throw gcnew InvalidOperationException("Didn't get an absolute path after normalization");

	if (path->Length > root->Length && path[path->Length-1] == '\\')
	{
		path = path->TrimEnd('\\');

		if (path->Length <= root->Length)
			return root;
	}

	return FindTruePath(path, root);
}

String^ SvnTools::GetFullTruePath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	path = GetNormalizedFullPath(path);
	String^ root = GetPathRootPart(path);

	return FindTruePath(path, root);
}

String^ SvnTools::FindTruePath(String^ path, String^ root)
{
	// Okay, now we have a normalized path and it's root in normal form. Now we need to find the exact casing of the next parts
	StringBuilder^ result = gcnew StringBuilder(root, path->Length + 16);

	pin_ptr<const wchar_t> pChars = PtrToStringChars(path);
	size_t len = sizeof(wchar_t)* (path->Length+1+4);
	wchar_t* pSec = (wchar_t*)_alloca(len);

	if (wcscpy_s(pSec, len, L"\\\\?\\") || wcscat_s(pSec, len, pChars))
		return nullptr;

	wchar_t *pTxt = &pSec[4]; // The point after '\\?\'

	int nStart = root->Length;
	bool isFirst = true;

	while (nStart < path->Length)
	{
		WIN32_FIND_DATAW filedata;

		int nNext = path->IndexOf('\\', nStart);

		if (nNext > 0)
			pTxt[nNext] = 0; // Temporarily replace '\' with 0

		HANDLE hSearch = FindFirstFileW(pSec, &filedata);

		if (hSearch == INVALID_HANDLE_VALUE)
			return nullptr;

		if (!isFirst)
			result->Append((wchar_t)'\\');

		result->Append(gcnew String(filedata.cFileName));

		GC::KeepAlive(::FindClose(hSearch)); // Close search request

		if (nNext < 0)
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
	if (path->Length >= (MAX_PATH-4))
		path = "\\\\?\\" + SvnTools::GetNormalizedFullPath(path);

	pin_ptr<const wchar_t> p = PtrToStringChars(path);
	DWORD r = ::GetFileAttributesW(p);

	if (r == INVALID_FILE_ATTRIBUTES)
		return false;

	return (r & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool SvnTools::IsManagedPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	if (IsSeparator(path, path->Length-1))
		path += SvnClient::AdministrativeDirectoryName;
	else
		path += "\\" + SvnClient::AdministrativeDirectoryName;

	return IsDirectory(path->Replace('/', '\\'));
}

bool SvnTools::IsBelowManagedPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	// We search from the root, instead of the other way around to optimize the disk cache
	// (We probably never need to access the disk when called a few times in a row)

	path = GetNormalizedFullPath(path);

	String^ root = GetPathRootPart(path);

	if (!root)
		return false;

	int nStart = root->Length;
	int i;

	while (0 < (i = path->IndexOf('\\', nStart)))
	{
		if (IsManagedPath(path->Substring(0, i)))
			return true;

		nStart = i+1;
	}

	if (nStart >= (path->Length-1))
		return false; // Path ends with a \, probably disk root

	return IsManagedPath(path);
}

String^ SvnTools::GetNormalizedFullPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	if (PathContainsInvalidChars(path))
		throw gcnew ArgumentException("Path contains invalid characters", "path");
	else if (IsNormalizedFullPath(path))
		return path; // Just pass through; no allocations

	bool retry = true;
	
	if(path->Length < MAX_PATH)
	{
		try
		{	
			path = Path::GetFullPath(path);
			retry = false;
		}
		catch(PathTooLongException^) // Path grew by getting full path
		{
			// Use the retry
		}
	}

	if (retry)
	{
		path = LongGetFullPath(path);

		if (!GetPathRootPart(path))
			throw gcnew PathTooLongException("Paths with a length above MAX_PATH must be rooted");
	}

	if (path->Length >= 2 && path[1] == ':')
	{
		wchar_t c = path[0];

		if ((c >= 'a') && (c <= 'z'))
			path = wchar_t::ToUpperInvariant(c) + path->Substring(1);

		String^ r = path->TrimEnd('\\');

		if (r->Length > 3)
			return r;
		else
			return path->Substring(0, 3);
	}
	else if (path->StartsWith("\\\\", StringComparison::OrdinalIgnoreCase))
	{
		String^ root = GetPathRootPart(path);

		if (root && !path->StartsWith(root, StringComparison::Ordinal))
			path = root + path->Substring(root->Length)->TrimEnd('\\');
	}
	else
		path = path->TrimEnd('\\');

	return path;
}

bool SvnTools::IsAbsolutePath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentException("path");

	int c = path->Length;

	if (path->Length < 3)
		return false;

	int i, n;
	if (IsSeparator(path, 0))
	{
		if (!IsSeparator(path, 1))
			return false;

		for (i = 2; i < path->Length; i++)
		{
			if (!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if (i == 2 || !IsSeparator(path, i))
			return false;

		i++;

		n = i;

		for (; i < path->Length; i++)
		{
			if (!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if (i == n || !IsSeparator(path, i))
			return false;

		i++;
	}
	else if ((path[1] != ':') || !IsSeparator(path, 2))
		return false;
	else if (!(((path[0] >= 'A') && (path[0] <= 'Z')) || ((path[0] >= 'a') && (path[0] <= 'z'))))
		return false;
	else
		i = 3;

	while (i < c)
	{
		if (i >= c && IsSeparator(path, i))
			return false; // '\'-s behind each other

		if (i < c && path[i] == '.')
		{
			int n = i;
			n++;

			if (n < c && path[n] == '.')
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

		if (i == c)
			return false; // We don't like paths with a '\' at the end
	}

	return true;
}

bool SvnTools::IsNormalizedFullPath(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentException("path");

	int c = path->Length;

	if (path->Length < 3)
		return false;

	int i, n;
	if (IsDirSeparator(path, 0))
	{
		if (!IsDirSeparator(path, 1))
			return false;

		for (i = 2; i < path->Length; i++)
		{
			// Check hostname rules
			if (!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if (i == 2 || !IsDirSeparator(path, i))
			return false;

		i++;

		n = i;

		for (; i < path->Length; i++)
		{
			// Check share name rules
			if (!Char::IsLetterOrDigit(path, i) && 0 > static_cast<String^>("._-")->IndexOf(path[i]))
				break;
		}

		if (i == n)
			return false; // "\\server\"
		else if (i == c)
			return true; // "\\server\path"
		else if (c > i && !IsDirSeparator(path, i))
			return false;
		else if (c == i+1)
			return false; // "\\server\path\"

		i++;
	}
	else if ((path[1] != ':') || !IsDirSeparator(path, 2))
		return false;
	else if (!((path[0] >= 'A') && (path[0] <= 'Z')))
		return false;
	else
		i = 3;

	while (i < c)
	{
		if (i >= c && IsDirSeparator(path, i))
			return false; // '\'-s behind each other

		if (i < c && path[i] == '.')
		{
			int n = i;

			while (n < c && path[n] == '.')
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

		if (i == c)
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

String^ SvnTools::GetFileName(Uri^ target)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	target = SvnBase::CanonicalizeUri(target);
	
	String^ path = target->AbsolutePath;

	int nEnd = path->LastIndexOf('?');

	if (nEnd < 0)
		nEnd = path->Length-1;

	if (path->Length && nEnd <= path->Length && path[nEnd] == '/')
		nEnd--;

	if (nEnd <= 0)
		return "";

	int nStart = path->LastIndexOf('/', nEnd);

	if (nStart >= 0)
		nStart++;
	else
		nStart = 0;

	path = path->Substring(nStart, nEnd-nStart+1);

	return UriPartToPath(path);
}

String^ SvnTools::PathCombine(String^ path1, String^ path2)
{
	if (!path1)
		throw gcnew ArgumentNullException("path1");
	else if (!path2)
		throw gcnew ArgumentNullException("path2");
	
	try
	{
		return Path::Combine(path1, path2);
	}
	catch (PathTooLongException^)
	{
		if (GetPathRootPart(path2))
		{
			// Handle large absolute paths in path2
			return path2;
		}

		// The next code is fall back code that is only really supported
		// for appending simple relative filenames after a parent path

		// This all to avoid exceptions when creating repository local paths that
		// trip the MAX_PATH limit

		path1 = SvnTools::GetNormalizedFullPath(path1);

		if (IsSeparator(path2, 0)) // "\d\e\f"
		{
			// BH: This is what Path::Combine does in .Net 2.0!!!

			String^ root = GetPathRootPart(Environment::CurrentDirectory); 

			if(root)
				path1 = root;
		}

		if (!IsSeparator(path1, path1->Length-1))
			path1 += '\\';			

		return SvnTools::GetNormalizedFullPath(path1 + 
			path2->Replace(Path::AltDirectorySeparatorChar, Path::DirectorySeparatorChar)->TrimStart(Path::DirectorySeparatorChar));
	}
}
