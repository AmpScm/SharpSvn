#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookPropertyListArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::GetPropertyList(SvnLookOrigin^ lookOrigin, String^ path, [Out] SvnPropertyCollection^% properties)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path)
		throw gcnew ArgumentNullException("path");

	return GetPropertyList(lookOrigin, path, gcnew SvnLookPropertyListArgs(), properties);
}

bool SvnLookClient::GetPropertyList(SvnLookOrigin^ lookOrigin, String^ path, SvnLookPropertyListArgs^ args, [Out] SvnPropertyCollection^% properties)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path)
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_fs_root_t* root = nullptr;
	svn_error_t* r = open_origin(lookOrigin, &root, nullptr, nullptr, %pool);

	if (r)
		return args->HandleResult(this, r);

	apr_hash_t *props;

	if (r = svn_fs_node_proplist(&props, root, pool.AllocCanonical(path), pool.Handle))
		return args->HandleResult(this, r);

	properties = SvnBase::CreatePropertyDictionary(props, %pool);

	return args->HandleResult(this, SVN_NO_ERROR);
}

