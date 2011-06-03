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
#include "LookArgs/SvnLookClientArgs.h"

#include "AprBaton.h"


namespace SharpSvn {
	ref class SvnChangeInfoArgs;
	ref class SvnChangedArgs;

	ref class SvnLookRevisionPropertyArgs;
	ref class SvnLookGetPropertyArgs;
	ref class SvnLookPropertyListArgs;
	ref class SvnLookWriteArgs;
	ref class SvnLookListArgs;

	/// <summary>Container for the location to use with the <see cref="SvnLookClient" /> commands</summary>
	public ref class SvnLookOrigin sealed : public SvnBase
	{
		initonly String^ _path;
		initonly __int64 _revision;
		initonly String^ _transactionName;
	public:
		/// <overloads>Initializes a new SvnLookOrigin</overloads>
		/// <summary>Initializes a new SvnLookOrigin instance for the head revision of repositoryPath</summary>
		SvnLookOrigin(String^ repositoryPath);
		/// <summary>Initializes a new SvnLookOrigin instance for a specific revision of repositoryPath</summary>
		SvnLookOrigin(String^ repositoryPath, __int64 revision);
		/// <summary>Initializes a new SvnLookOrigin instance for a transaction in repositoryPath</summary>
		SvnLookOrigin(String^ repositoryPath, String^ transactionName);

	public:
		property String^ RepositoryPath
		{
			String^ get() { return _path; }
		}

		property bool HasTransaction
		{
			bool get() { return (_transactionName != nullptr); }
		}

		property String^ Transaction
		{
			String^ get() { return _transactionName; }
		}

		property __int64 Revision
		{
			__int64 get() { return _revision; }
		}

		property bool HasRevision
		{
			bool get() { return _revision >= 0; }
		}
	};

	/// <summary>
	/// Managed wrapper of some of the most common svnlook functions
	/// </summary>
	/// <remarks>
	/// At this time this class is a limited subset of svnlook functionality and
	/// is experimental
	/// </remarks>
	public ref class SvnLookClient : public SvnClientContext
	{
		initonly AprBaton<SvnLookClient^>^ _clientBaton;
		AprPool _pool;

	public:
		///<summary>Initializes a new <see cref="SvnLookClient" /> instance with default properties</summary>
		SvnLookClient();

	private:
		~SvnLookClient();

	public:
		/// <overloads>Gets the change information of a change directly from a repository</overloads>
		/// <summary>Gets the change information (author, date, log, paths, etc.) of the last change in a repository</summary>
		bool ChangeInfo(SvnLookOrigin^ lookOrigin, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler);

		/// <summary>Gets the change information (author, date, log, paths, etc.) of a change in a repository</summary>
		bool ChangeInfo(SvnLookOrigin^ lookOrigin, SvnChangeInfoArgs^ args, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler);

		/// <overloads>Gets the change information of a change directly from a repository</overloads>
		/// <summary>Gets the change information (author, date, log, paths, etc.) of the last change in a repository</summary>
		bool GetChangeInfo(SvnLookOrigin^ lookOrigin, [Out] SvnChangeInfoEventArgs^% changeInfo);

		/// <summary>Gets the change information (author, date, log, paths, etc.) of a change in a repository</summary>
		bool GetChangeInfo(SvnLookOrigin^ lookOrigin, SvnChangeInfoArgs^ args, [Out] SvnChangeInfoEventArgs^% changeInfo);

	public:
		/// <overloads>Gets the changed paths directly from a repository</overloads>
		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool Changed(SvnLookOrigin^ lookOrigin, EventHandler<SvnChangedEventArgs^>^ changedHandler);

		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool Changed(SvnLookOrigin^ lookOrigin, SvnChangedArgs^ args, EventHandler<SvnChangedEventArgs^>^ changedHandler);

		/// <overloads>Gets the changed paths directly from a repository</overloads>
		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool GetChanged(SvnLookOrigin^ lookOrigin, [Out] Collection<SvnChangedEventArgs^>^% changedItems);
		///<summary>Equivalent to <c>svnlook changed</c></summary>
		bool GetChanged(SvnLookOrigin^ lookOrigin, SvnChangedArgs^ args, [Out] Collection<SvnChangedEventArgs^>^% changedItems);

	public:
		/// <overloads>Gets a revision property directly from a repository</overloads>
		/// <summary>Gets the specified revision property directly from the repository</summary>
		bool GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, [Out] String^% value);
		/// <summary>Gets the specified revision property directly from the repository</summary>
		bool GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, [Out] SvnPropertyValue^% value);
		/// <summary>Gets the specified revision property directly from the repository</summary>
		bool GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] String^% value);
		/// <summary>Gets the specified revision property directly from the repository</summary>
		bool GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] SvnPropertyValue^% value);

	public:
		/// <overloads>Gets the property value of the specified path directly from a repository</overloads>
		/// <summary>Gets the property value of the specified path directly from a repository</summary>
		bool GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, [Out] String^% value);
		/// <summary>Gets the property value of the specified path directly from a repository</summary>
		bool GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, [Out] SvnPropertyValue^% value);
		/// <summary>Gets the property value of the specified path directly from a repository</summary>
		bool GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, SvnLookGetPropertyArgs^ args, [Out] String^% value);
		/// <summary>Gets the property value of the specified path directly from a repository</summary>
		bool GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, SvnLookGetPropertyArgs^ args, [Out] SvnPropertyValue^% value);

	public:
		/// <overloads>Gets all properties on files or dirs (<c>svn proplist</c>)</overloads>
		/// <summary>Gets all properties on files or dirs (<c>svn proplist</c>)</summary>
		bool GetPropertyList(SvnLookOrigin^ lookOrigin, String^ path, [Out] SvnPropertyCollection^% properties);
		/// <summary>Gets all properties on files or dirs (<c>svn proplist</c>)</summary>
		bool GetPropertyList(SvnLookOrigin^ lookOrigin, String^ path, SvnLookPropertyListArgs^ args, [Out] SvnPropertyCollection^% properties);

	public:
		/// <overloads>Writes the content of the specified path to a stream directly from a repository</overloads>
		/// <summary>Writes the content of the specified path to a stream directly from a repository</summary>
		bool Write(SvnLookOrigin^ lookOrigin, String^ path, Stream^ toStream);
		/// <summary>Writes the content of the specified path to a stream directly from a repository</summary>
		bool Write(SvnLookOrigin^ lookOrigin, String^ path, Stream^ toStream, SvnLookWriteArgs^ args);

	public:
		/// <overloads>Lists the content of a path directly from a repository</overloads>
		/// <summary>Lists the content of a path directly from a repository</summary>
		bool List(SvnLookOrigin^ origin, String^ path, EventHandler<SvnLookListEventArgs^>^ listHandler);

		/// <summary>Lists the content of a path directly from a repository</summary>
		bool List(SvnLookOrigin^ origin, String^ path, SvnLookListArgs^ args, EventHandler<SvnLookListEventArgs^>^ listHandler);

		/// <overloads>Gets the content of a path directly from a repository</overloads>
		/// <summary>Gets the content of a path directly from a repository</summary>
		bool GetList(SvnLookOrigin^ origin, String^ path, [Out] Collection<SvnLookListEventArgs^>^% list);
		/// <summary>Gets the content of a path directly from a repository</summary>
		bool GetList(SvnLookOrigin^ origin, String^ path, SvnLookListArgs^ args, [Out] Collection<SvnLookListEventArgs^>^% list);

	public:
		/// <summary>
		/// Raised to allow canceling operations. The event is first
		/// raised on the <see cref="SvnClientArgs" /> object and
		/// then on the <see cref="SvnLookClient" />
		/// </summary>
		DECLARE_EVENT(SvnCancelEventArgs^, Cancel)

	protected:
		virtual void OnCancel(SvnCancelEventArgs^ e);

	internal:
		void HandleClientCancel(SvnCancelEventArgs^ e);

	private:
		svn_error_t* ProcessTree(svn_repos_node_t *node, String^ basePath, SvnChangedArgs^ args);

		svn_error_t* open_origin(SvnLookOrigin^ lookOrigin, svn_fs_root_t **root, svn_fs_t **fs, svn_repos_t **repos, AprPool^ pool);
	};
}
