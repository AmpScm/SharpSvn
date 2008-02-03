// $Id: SvnPathTarget.h 171 2007-10-14 19:38:35Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClientContext.h"

namespace SharpSvn {

	using namespace System;

	ref class SvnRepositorySession : public SvnClientContext
	{
		initonly SharpSvn::Apr::AprBaton<SvnRepositorySession^>^ _clientBatton;
		AprPool _pool;

	public:
		SvnRepositorySession(void);

	private:
		~SvnRepositorySession();
		!SvnRepositorySession();

	};
}
