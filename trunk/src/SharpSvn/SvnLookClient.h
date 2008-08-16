// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"

#include "AprBaton.h"

namespace SharpSvn {
	ref class SvnChangedArgs;

	/// <summary>
	/// Managed wrapper of some of the most common svnlook functions
	/// </summary>
	/// <remarks>
	/// At this time this class is a limited subset of svnlook functionality and
	/// is experimental
	/// </remarks>
	public ref class SvnLookClient : public SvnClientContext
	{
		AprPool _pool;

	public:
		///<summary>Initializes a new <see cref="SvnLookClient" /> instance with default properties</summary>
		SvnLookClient();

	public:
		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool Changed(String^ repositoryPath, SvnChangedArgs^ args);

		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool Changed(String^ repositoryPath, SvnChangedArgs^ args, EventHandler<SvnChangedEventArgs^>^ changedHandler);

		bool GetChanged(String^ repositoryPath, SvnChangedArgs^ args, [Out] Collection<SvnChangedEventArgs^>^% changedItems);

	private:
		~SvnLookClient();
		svn_error_t* ProcessTree(svn_repos_node_t *node, String^ path, String^ relativePath, SvnChangedArgs^ args);
		String^ PreparePath(String^ path);
		String^ SvnPathJoin(String^ base, String^ component);
	};
}
