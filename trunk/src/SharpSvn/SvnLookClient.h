// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientContext.h"
#include "SvnClientEventArgs.h"
#include "SvnClientArgs.h"
#include "LookArgs/SvnLookClientArgs.h"

#include "AprBaton.h"


namespace SharpSvn {
	ref class SvnChangeInfoArgs;
	ref class SvnChangedArgs;
	
	ref class SvnLookPropertyArgs;

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
		/// <overloads>Gets the change information of a change in a repository</overloads>
		/// <summary>Gets the change information (author, date, log, paths, etc.) of the last change in a repository</summary>
		bool ChangeInfo(String^ repositoryPath, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler);

		/// <summary>Gets the change information (author, date, log, paths, etc.) of a change in a repository</summary>
		/// <remarks>Use <see cref="SvnLookClientArgs::Transaction" /> or <see cref="SvnLookClientArgs::Revision" /> on
		/// the args object to specify a specific version</remarks>
		bool ChangeInfo(String^ repositoryPath, SvnChangeInfoArgs^ args, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler);

		/// <summary>Gets the change information (author, date, log, paths, etc.) of the last change in a repository</summary>
		bool GetChangeInfo(String^ repositoryPath, [Out] SvnChangeInfoEventArgs^% changeInfo);

		/// <summary>Gets the change information (author, date, log, paths, etc.) of a change in a repository</summary>
		/// <remarks>Use <see cref="SvnLookClientArgs::Transaction" /> or <see cref="SvnLookClientArgs::Revision" /> on
		/// the args object to specify a specific version</remarks>
		bool GetChangeInfo(String^ repositoryPath, SvnChangeInfoArgs^ args, [Out] SvnChangeInfoEventArgs^% changeInfo);

	public:
		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool Changed(String^ repositoryPath, EventHandler<SvnChangedEventArgs^>^ changedHandler);

		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool Changed(String^ repositoryPath, SvnChangedArgs^ args, EventHandler<SvnChangedEventArgs^>^ changedHandler);

		bool GetChanged(String^ repositoryPath, [Out] Collection<SvnChangedEventArgs^>^% changedItems);
		bool GetChanged(String^ repositoryPath, SvnChangedArgs^ args, [Out] Collection<SvnChangedEventArgs^>^% changedItems);

		bool Property(String^ repositoryPath, String^ propertyName, SvnLookPropertyArgs^ args);
		bool GetProperty(String^ repositoryPath, String^ propertyName, SvnLookPropertyArgs^ args, [Out] String^% result);

		bool GetAuthor(String^ repositoryPath, SvnLookPropertyArgs^ args, [Out] String^% result);
		bool GetLog(String^ repositoryPath, SvnLookPropertyArgs^ args, [Out] String^% result);
		bool GetDate(String^ repositoryPath, SvnLookPropertyArgs^ args, [Out] DateTime^% result);

	private:
		~SvnLookClient();
		svn_error_t* ProcessTree(svn_repos_node_t *node, String^ path, String^ relativePath, SvnChangedArgs^ args);
	};
}
