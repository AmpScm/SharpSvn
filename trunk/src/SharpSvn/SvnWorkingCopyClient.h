// $Id: SvnClient.h 594 2008-06-20 01:43:34Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	ref class SvnWorkingCopyStateArgs;
	ref class SvnWorkingCopyEntriesArgs;
	ref class SvnWorkingCopyEntryEventArgs;
	ref class SvnWorkingCopyEntryCollection;

	public ref class SvnWorkingCopyClient : public SvnClientContext
	{
		AprPool _pool;

	public:
		SvnWorkingCopyClient();

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

	};
};