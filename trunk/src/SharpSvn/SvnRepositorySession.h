// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientContext.h"

namespace SharpSvn {

	using namespace System;

	ref class SvnRepositorySession : public SvnClientContext
	{
		initonly AprBaton<SvnRepositorySession^>^ _clientBatton;
		AprPool _pool;

	public:
		SvnRepositorySession(void);

	private:
		~SvnRepositorySession();
		!SvnRepositorySession();

	};
}
