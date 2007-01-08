#include "stdafx.h"

#include "SvnAll.h"
#include "SvnCommitArgs.h"

using namespace TurtleSvn;
#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace TurtleSvn::Apr;
using namespace TurtleSvn;

SvnCommitItem::SvnCommitItem(const svn_client_commit_item2_t *commitItemInfo)
{
	if(!commitItemInfo)
		throw gcnew NullReferenceException("commitItemInfo");

	_info = commitItemInfo;
}

void SvnCommitItem::Detach(bool keepProperties)
{
	if(!_info)
		return;

	if(keepProperties)
	{
		GC::KeepAlive(Path);
		GC::KeepAlive(Uri);
		GC::KeepAlive(CopyFromUri);
	}
	_info = nullptr;
}

