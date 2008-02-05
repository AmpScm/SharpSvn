// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#ifdef UNUSED_FOR_NOW
#include "SvnAll.h"
#include "SvnRepositorySession.h"

#include <svn_ra.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

SvnRepositorySession::SvnRepositorySession()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBatton = gcnew AprBaton<SvnRepositorySession^>(this);

	Initialize();
}

SvnRepositorySession::~SvnRepositorySession()
{
	delete _clientBatton;
}

static svn_error_t *open_tmp_file(apr_file_t **fp, void *callback_baton, apr_pool_t *pool);


void SvnRepositorySession::Initialize()
{
	svn_ra_callbacks2_t* cb = nullptr;

	if(svn_ra_create_callbacks(&cb, _pool.Handle))
		throw gcnew InvalidOperationException();

	_callbacks = cb;

	cb->open_tmp_file = open_tmp_file;
	//_callbacks->
}

static svn_error_t *
open_tmp_file(apr_file_t **fp,
              void *callback_baton,
              apr_pool_t *pool)
{
  const char *temp_dir;

  /* "Say, Subversion.  Seen any good tempdirs lately?" */
  SVN_ERR(svn_io_temp_dir(&temp_dir, pool));

  /* Open a unique file;  use APR_DELONCLOSE. */
  return svn_io_open_unique_file2(fp, NULL,
                                  svn_path_join(temp_dir, "sharpsvn", pool),
                                  ".tmp", svn_io_file_del_on_close, pool);
}
#endif