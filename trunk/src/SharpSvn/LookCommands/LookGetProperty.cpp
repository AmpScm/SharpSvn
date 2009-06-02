#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookGetPropertyArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, [Out] String^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path) // path "" = Repos root
		throw gcnew ArgumentNullException("path");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetProperty(lookOrigin, path, propertyName, gcnew SvnLookGetPropertyArgs(), value);
}

bool SvnLookClient::GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, [Out] SvnPropertyValue^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path) // path "" = Repos root
		throw gcnew ArgumentNullException("path");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetProperty(lookOrigin, path, propertyName, gcnew SvnLookGetPropertyArgs(), value);
}

bool SvnLookClient::GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, SvnLookGetPropertyArgs^ args, [Out] String^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path) // path "" = Repos root
		throw gcnew ArgumentNullException("path");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnPropertyValue^ val = nullptr;
	try
	{
		return GetProperty(lookOrigin, path, propertyName, args, val);
	}
	finally
	{
		if (val)
			value = val->StringValue;
		else
			value = nullptr;
	}
}

bool SvnLookClient::GetProperty(SvnLookOrigin^ lookOrigin, String^ path, String^ propertyName, SvnLookGetPropertyArgs^ args, [Out] SvnPropertyValue^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path) // path "" = Repos root
		throw gcnew ArgumentNullException("path");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_fs_root_t* root = nullptr;
	svn_error_t* r = open_origin(lookOrigin, &root, nullptr, nullptr, %pool);

	if (r)
		return args->HandleResult(this, r);

	svn_string_t *prop;
	const char* pcPropName = pool.AllocString(propertyName);

	if (r = svn_fs_node_prop(&prop, root, pool.AllocCanonical(path), pcPropName, pool.Handle))
		return args->HandleResult(this, r);

	value = SvnPropertyValue::Create(pcPropName, prop, nullptr);

	return args->HandleResult(this, r);
}