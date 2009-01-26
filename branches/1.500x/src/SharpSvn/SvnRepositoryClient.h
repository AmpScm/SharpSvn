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

#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"

#include "AprBaton.h"

namespace SharpSvn {
	ref class SvnCreateRepositoryArgs;
	ref class SvnDeleteRepositoryArgs;
	ref class SvnRecoverRepositoryArgs;
	ref class SvnLoadRepositoryArgs;
	ref class SvnDumpRepositoryArgs;
	ref class SvnUpgradeRepositoryArgs;


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
		///<summary>Initializes a new <see cref="SvnRepositoryClient" /> instance with default properties</summary>
		SvnRepositoryClient();

	public:
		/// <summary>Helper which allows splitting file:/// uri's in the repository root and the file within</summary>
		String^ FindRepositoryRoot(Uri^ repositoryUri);

		/// <summary>Creates a default repository at the specified local path</summary>
		bool CreateRepository(String^ repositoryPath);
		/// <summary>Creates a default repository at the specified local path</summary>
		bool CreateRepository(String^ repositoryPath, SvnCreateRepositoryArgs^ args);
		/// <summary>Deletes the repository at the specified local path</summary>
		bool DeleteRepository(String^ repositoryPath);
		/// <summary>Deletes the repository at the specified local path</summary>
		bool DeleteRepository(String^ repositoryPath, SvnDeleteRepositoryArgs^ args);

		/// <summary>Recovers the repository at the specified local path</summary>
		bool RecoverRepository(String^ repositoryPath);
		/// <summary>Recovers the repository at the specified local path</summary>
		bool RecoverRepository(String^ repositoryPath, SvnRecoverRepositoryArgs^ args);

		/// <overloads>Loads a repository from a file (svnadmin load)</overloads>
		/// <summary>Loads a repository from a file (svnadmin load)</summary>
		bool LoadRepository(String^ repositoryPath, Stream^ from);
		/// <summary>Loads a repository from a file (svnadmin load)</summary>
		bool LoadRepository(String^ repositoryPath, Stream^ from, SvnLoadRepositoryArgs^ args);

		/// <overloads>Dumps a repository to a file (svnadmin dump)</overloads>
		/// <summary>Dumps a repository to a file (svnadmin dump)</summary>
		bool DumpRepository(String^ repositoryPath, Stream^ to);
		/// <summary>Dumps a repository to a file (svnadmin dump)</summary>
		bool DumpRepository(String^ repositoryPath, Stream^ to, SvnDumpRepositoryArgs^ args);

		/// <overloads>In-Place Upgrades a repository to a later version</overloads>
		/// <summary>In-Place Upgrades a repository to a later version</summary>
		/// <remarks>
		///  NOTE: This functionality is provided as a convenience for
		/// administrators wishing to make use of new Subversion functionality
		/// without a potentially costly full repository dump/load.  As such,
		/// the operation performs only the minimum amount of work needed to
		/// accomplish this while maintaining the integrity of the repository.
		/// It does *not* guarantee the most optimized repository state as a
		/// dump and subsequent load would.
		/// </remarks>
		bool UpgradeRepository(String^ repositoryPath);
		/// <summary>In-Place Upgrades a repository to a later version</summary>
		/// <remarks>
		///  NOTE: This functionality is provided as a convenience for
		/// administrators wishing to make use of new Subversion functionality
		/// without a potentially costly full repository dump/load.  As such,
		/// the operation performs only the minimum amount of work needed to
		/// accomplish this while maintaining the integrity of the repository.
		/// It does *not* guarantee the most optimized repository state as a
		/// dump and subsequent load would.
		/// </remarks>
		bool UpgradeRepository(String^ repositoryPath, SvnUpgradeRepositoryArgs^ args);

	private:
		~SvnRepositoryClient();
	};
}
