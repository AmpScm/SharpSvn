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

namespace SharpSvn {

	ref class SvnWorkingCopyStateArgs;
	ref class SvnWorkingCopyEntriesArgs;
	ref class SvnWorkingCopyEntryEventArgs;
	ref class SvnWorkingCopyEntryCollection;
	ref class SvnWorkingCopyInstallConflictArgs;
	ref class SvnWorkingCopyMoveArgs;
	ref class SvnWorkingCopyCopyArgs;

	public ref class SvnWorkingCopyClient : public SvnClientContext
	{
		initonly AprBaton<SvnWorkingCopyClient^>^ _clientBaton;
		AprPool _pool;

	public:
		SvnWorkingCopyClient();

	private:
		~SvnWorkingCopyClient();

	public:
		/// <summary>Gets the text file status of a working copy path</summary>
		bool GetState(String^ targetPath, [Out] SvnWorkingCopyState^% result);
		/// <summary>Gets the text file status of a working copy path</summary>
		bool GetState(String^ targetPath, SvnWorkingCopyStateArgs^ args, [Out] SvnWorkingCopyState^% result);

	public:
		/// <summary>Enumerates through the list of entries contained in the specified working copy path</summary>
		/// <remarks>Only the entry structures representing files and SVN_WC_ENTRY_THIS_DIR contain complete information.  The entry
		/// structures representing subdirs have only the `kind' and `state' fields filled in. If you want info on a subdir, you must use this
		/// routine to open its @a path and read the @c SVN_WC_ENTRY_THIS_DIR structure, or call svn_wc_entry() on its @a path.
		///</remarks>
		bool ListEntries(String^ directory, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler);
		/// <summary>Enumerates through the list of entries contained in the specified working copy path</summary>
		/// <remarks>Only the entry structures representing files and SVN_WC_ENTRY_THIS_DIR contain complete information.  The entry
		/// structures representing subdirs have only the `kind' and `state' fields filled in. If you want info on a subdir, you must use this
		/// routine to open its @a path and read the @c SVN_WC_ENTRY_THIS_DIR structure, or call svn_wc_entry() on its @a path.
		///</remarks>
		bool ListEntries(String^ directory, SvnWorkingCopyEntriesArgs^ args, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler);

		/// <summary>Gets a list of entries contained in the specified working copy path</summary>
		/// <remarks>Helper method used by AnkhSVN to detect nested working copies</remarks>
		/// <remarks>Only the entry structures representing files and SVN_WC_ENTRY_THIS_DIR contain complete information.  The entry
		/// structures representing subdirs have only the `kind' and `state' fields filled in. If you want info on a subdir, you must use this
		/// routine to open its @a path and read the @c SVN_WC_ENTRY_THIS_DIR structure, or call svn_wc_entry() on its @a path.
		///</remarks>
		bool GetEntries(String^ directory, [Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list);

		/// <summary>Gets a list of entries contained in the specified working copy path</summary>
		/// <remarks>Helper method used by AnkhSVN to detect nested working copies</remarks>
		/// <remarks>Only the entry structures representing files and SVN_WC_ENTRY_THIS_DIR contain complete information.  The entry
		/// structures representing subdirs have only the `kind' and `state' fields filled in. If you want info on a subdir, you must use this
		/// routine to open its @a path and read the @c SVN_WC_ENTRY_THIS_DIR structure, or call svn_wc_entry() on its @a path.
		///</remarks>
		bool GetEntries(String^ directory, SvnWorkingCopyEntriesArgs^ args,[Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list);

	public:
		/// <summary>Gets the working copy version (<c>svnversion</c>)</summary>
		bool GetVersion(String^ targetPath, [Out] SvnWorkingCopyVersion^% version);
		/// <summary>Gets the working copy version (<c>svnversion</c>)</summary>
		bool GetVersion(String^ targetPath, SvnGetWorkingCopyVersionArgs^ args, [Out] SvnWorkingCopyVersion^% version);

	public:
		/// <summary>Moves a node to a different path</summary>
		bool Move(String^ sourcePath, String^ toPath);
		/// <summary>Moves a node to a different path</summary>
		bool Move(String^ sourcePath, String^ toPath, SvnWorkingCopyMoveArgs^ args);

	public:
		/// <summary>Copies a node to a different path</summary>
		bool Copy(String^ sourcePath, String^ toPath);
		/// <summary>Copies a node to a different path</summary>
		bool Copy(String^ sourcePath, String^ toPath, SvnWorkingCopyCopyArgs^ args);

	public:
		bool InstallConflict(String^ targetPath, SvnUriOrigin^ leftSource, SvnUriOrigin^ rightSource, SvnWorkingCopyInstallConflictArgs^ args);
	};
};