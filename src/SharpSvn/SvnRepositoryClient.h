// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"

#include "AprBaton.h"

namespace SharpSvn {

	/// <summary>
	/// Managed wrapper of some of the most common repository management functions.
	/// </summary>
	/// <remarks>
	/// At this time this class is just included to enable a standalone test framework
	/// over the SvnClient api; without depending on external tools)
	/// </remarks>
	public ref class SvnRepositoryClient : public SvnClientContext
	{
		AprPool _pool;

	public:
		///<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
		SvnRepositoryClient();

	public:
		/// <summary>Helper which allows splitting file:/// uri's in the repository root and the file within</summary>
		String^ FindRepositoryRoot(Uri^ repositoryUri);

		/// <summary>Creates a default repository at the specified local path</summary>
		void CreateRepository(String^ repositoryPath);
		/// <summary>Creates a default repository at the specified local path</summary>
		bool CreateRepository(String^ repositoryPath, SvnCreateRepositoryArgs^ args);
		/// <summary>Deletes the repository at the specified local path</summary>
		void DeleteRepository(String^ repositoryPath);
		/// <summary>Deletes the repository at the specified local path</summary>
		bool DeleteRepository(String^ repositoryPath, SvnDeleteRepositoryArgs^ args);

		/// <summary>Recovers the repository at the specified local path</summary>
		void RecoverRepository(String^ repositoryPath);
		/// <summary>Recovers the repository at the specified local path</summary>
		bool RecoverRepository(String^ repositoryPath, SvnRecoverRepositoryArgs^ args);

	private:
		~SvnRepositoryClient();
	};
}
