// $Id: GetWorkingCopyState.cpp 271 2008-02-14 17:44:58Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/WorkingCopyEntries.h"
#include "EventArgs/SvnWorkingCopyEntryEventArgs.h"

#include "UnmanagedStructs.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnWorkingCopyClient::ListEntries(String^ targetPath, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler)
{
	if (!targetPath)
		throw gcnew ArgumentNullException("targetPath");
	else if(!entryHandler)
		throw gcnew ArgumentNullException("entryHandler");
	
	return ListEntries(targetPath, gcnew SvnWorkingCopyEntriesArgs(), entryHandler);
}

bool SvnWorkingCopyClient::ListEntries(String^ targetPath, SvnWorkingCopyEntriesArgs^ args, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler)
{
	return false;
}

bool SvnWorkingCopyClient::GetEntries(String^ targetPath, [Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list)
{
	if (!targetPath)
		throw gcnew ArgumentNullException("targetPath");

	InfoItemCollection<SvnWorkingCopyEntryEventArgs^>^ results = gcnew InfoItemCollection<SvnWorkingCopyEntryEventArgs^>();

	try
	{
		return ListEntries(targetPath, gcnew SvnWorkingCopyEntriesArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}

bool SvnWorkingCopyClient::GetEntries(String^ targetPath, SvnWorkingCopyEntriesArgs^ args,[Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list)
{
	if (!targetPath)
		throw gcnew ArgumentNullException("targetPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnWorkingCopyEntryEventArgs^>^ results = gcnew InfoItemCollection<SvnWorkingCopyEntryEventArgs^>();

	try
	{
		return ListEntries(targetPath, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}