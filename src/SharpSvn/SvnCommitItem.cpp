// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;

SvnCommitItem::SvnCommitItem(const svn_client_commit_item3_t *commitItemInfo)
{
	if(!commitItemInfo)
		throw gcnew ArgumentNullException("commitItemInfo");

	_info = commitItemInfo;
}

void SvnCommitItem::Detach(bool keepProperties)
{
	try
	{
		if(keepProperties)
		{
			GC::KeepAlive(Path);
			GC::KeepAlive(Uri);
			GC::KeepAlive(CopyFromUri);
		}
	}
	finally
	{
		_info = nullptr;
	}
}

