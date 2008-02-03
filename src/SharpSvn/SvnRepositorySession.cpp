// $Id: SvnMergeInfo.cpp 171 2007-10-14 19:38:35Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "SvnRepositorySession.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;


SvnRepositorySession::SvnRepositorySession()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
}

SvnRepositorySession::~SvnRepositorySession()
{
}

SvnRepositorySession::!SvnRepositorySession()
{
}