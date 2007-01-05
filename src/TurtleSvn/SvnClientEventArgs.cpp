#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace TurtleSvn::Apr;
using namespace TurtleSvn;


SvnException^ SvnClientNotifyEventArgs::Error::get()
{
	if(!_exception && _notify && _notify->err)
		_exception = SvnException::Create(_notify->err);

	return _exception;
}