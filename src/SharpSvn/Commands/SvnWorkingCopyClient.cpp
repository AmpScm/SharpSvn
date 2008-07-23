// $Id: SvnRepositoryClient.cpp 304 2008-02-20 15:02:46Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SvnWorkingCopyClient::SvnWorkingCopyClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
}