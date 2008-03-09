// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnChangeList.h"
#include "SvnCommandLineArgumentCollection.h"

namespace SharpSvn {

	public ref class SvnTools sealed : SvnBase
	{
	private:
		SvnTools()
		{} // Static class

	public:
		/// <summary>Gets the repository Uri of a path, or <c>null</c> if path is not versioned</summary>
		static Uri^ GetUriFromWorkingCopy(String^ path);

		/// <summary>Gets the pathname exactly like it is on disk</summary>
		static String^ GetTruePath(String^ path);


		/// <summary>Gets a boolean indicating whether the path contains a subversion Working Copy</summary>
		/// <remarks>Assumes path is a directory</remarks>
		static bool IsManagedPath(String^ path);

		/// <summary>Gets a boolean indicating whether at least one of the 
		/// parent paths or the path (file/directory) itself is a working copy.
		/// </summary>
		static bool IsBelowManagedPath(String^ path);
	};
}