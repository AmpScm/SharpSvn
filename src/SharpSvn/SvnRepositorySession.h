// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once
#ifdef UNUSED_FOR_NOW
#include "SvnClientContext.h"

namespace SharpSvn {

	using namespace System;

	ref class SvnRepositorySession : public SvnClientContext
	{
		initonly AprBaton<SvnRepositorySession^>^ _clientBatton;
		AprPool _pool;

	private:
		svn_ra_callbacks2_t* _callbacks;

	public:
		SvnRepositorySession(void);

	private:
		~SvnRepositorySession();
		void Initialize();

	};
}
#endif