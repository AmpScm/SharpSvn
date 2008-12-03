// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnChangeList.h"
#include "SvnCommandLineArgumentCollection.h"

namespace SharpSvn {
	using namespace SharpSvn::Implementation;

	public ref class SvnTools sealed : SvnBase
	{
	private:
		SvnTools()
		{} // Static class

		static initonly array<wchar_t>^ g_DirSeparators = gcnew array<wchar_t>{ System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar };

		static String^ FindTruePath(String^ path, String^ root);

	public:
		/// <summary>Gets the repository Uri of a path, or <c>null</c> if path is not versioned</summary>
		static Uri^ GetUriFromWorkingCopy(String^ path);

		/// <summary>Gets the pathname exactly like it is on disk</summary>
		static String^ GetTruePath(String^ path);

		/// <summary>Gets the pathname exactly like it is on disk</summary>
		static String^ GetFullTruePath(String^ path);

		/// <summary>Gets a boolean indicating whether the path contains a subversion Working Copy</summary>
		/// <remarks>Assumes path is a directory</remarks>
		static bool IsManagedPath(String^ path);

		/// <summary>Gets a boolean indicating whether at least one of the
		/// parent paths or the path (file/directory) itself is a working copy.
		/// </summary>
		static bool IsBelowManagedPath(String^ path);

		/// <summary>
		/// Normalizes the path to a full path
		/// </summary>
		/// <summary>This normalizes drive letters to upper case and hostnames to lowercase to match Subversion 1.6 behavior</summary>
		static String^ GetNormalizedFullPath(String^ path);

		/// <summary>
		/// Checks whether normalization is required
		/// </summary>
		/// <remarks>This method does not verify the casing of invariant parts</remarks>
		static bool IsNormalizedFullPath(String^ path);

		/// <summary>
		/// Normalizes the Uri to a full absolute Uri
		/// </summary>
		static Uri^ GetNormalizedUri(Uri^ uri);

		/// <summary>
		/// Checks whether the specified path is an absolute path
		/// </summary>
		static bool IsAbsolutePath(String^ path);

		/// <summary>
		/// Appends the specified path suffix to the given Uri
		/// </summary>
		static Uri^ AppendPathSuffix(Uri^ uri, String^ pathSuffix);


		/// <summary>
		/// Converts a string from a Uri path to a local path name; unescaping when necessary
		/// </summary>
		static String^ UriPartToPath(String^ uriPath);


		/// <summary>
		/// Converts a local relative path to a valid relative Uri
		/// </summary>
		static Uri^ PathToRelativeUri(String^ path);

		/// <summary>Gets the filename of the specified target</summary>
		static String^ GetFileName(Uri^ target);

	internal:
		/// <summary>Long path capable version of <see cref="System::IO::Path::Combine" /></summary>
		static String^ PathCombine(String^ path1, String^ path2);
	};
}
