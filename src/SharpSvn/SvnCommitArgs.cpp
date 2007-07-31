#include "stdafx.h"

#include "SvnAll.h"
#include "SvnCommitArgs.h"

using namespace SharpSvn;
#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Apr;
using namespace SharpSvn;

SvnCommitItem::SvnCommitItem(const svn_client_commit_item3_t *commitItemInfo)
{
	if(!commitItemInfo)
		throw gcnew NullReferenceException("commitItemInfo");

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

