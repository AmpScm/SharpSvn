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
		bool ListEntries(String^ targetPath, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler);
		/// <summary>Enumerates through the list of entries contained in the specified working copy path</summary>
		bool ListEntries(String^ targetPath, SvnWorkingCopyEntriesArgs^ args, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler);

		/// <summary>Gets a list of entries contained in the specified working copy path</summary>
		/// <remarks>Helper method used by AnkhSVN to detect nested working copies</remarks>
		bool GetEntries(String^ targetPath, [Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list);

		/// <summary>Gets a list of entries contained in the specified working copy path</summary>
		/// <remarks>Helper method used by AnkhSVN to detect nested working copies</remarks>
		bool GetEntries(String^ targetPath, SvnWorkingCopyEntriesArgs^ args,[Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list);

	public:
		/// <summary>Gets the working copy version (<c>svnversion</c>)</summary>
		bool GetVersion(String^ targetPath, [Out] SvnWorkingCopyVersion^% version);
		/// <summary>Gets the working copy version (<c>svnversion</c>)</summary>
		bool GetVersion(String^ targetPath, SvnGetWorkingCopyVersionArgs^ args, [Out] SvnWorkingCopyVersion^% version);

	};
};