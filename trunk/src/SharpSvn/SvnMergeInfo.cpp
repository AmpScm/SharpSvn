#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using System::Collections::Generic::List;

SvnMergeInfo::SvnMergeInfo(SvnTarget^ target, apr_hash_t* mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	_target = target;
	if(!mergeInfo)
	{
		_available = false;
		return;
	}

	List<String^>^ sources = gcnew List<String^>(0);

	for (apr_hash_index_t *hi = apr_hash_first(NULL, mergeInfo); hi; hi = apr_hash_next(hi))
	{
		const char *path;
		apr_hash_this(hi, (const void **) &path, NULL, NULL);

		sources->Add(Utf8_PtrToString(path));
	}
	_mergeSources = sources->AsReadOnly();
}