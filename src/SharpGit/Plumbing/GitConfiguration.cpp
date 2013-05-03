#include "stdafx.h"

#include "GitRepository.h"
#include "GitConfiguration.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;
using namespace SharpGit::Implementation;

struct git_config {};

bool GitConfiguration::IsDisposed::get()
{
	return !_config || (_repository && _repository->IsDisposed);
}

bool GitConfiguration::Reload()
{
	return Reload(gcnew GitNoArgs());
}

bool GitConfiguration::Reload(GitArgs ^args)
{
	if (! args)
		throw gcnew ArgumentNullException("args");

	int r = git_config_refresh(Handle);

	return args->HandleGitError(this, r);
}

bool GitConfiguration::TryGetInt32(String ^key, [Out] int %value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	GitPool pool;

	value = 0;

	int32_t v;

	int r = git_config_get_int32(&v, Handle, pool.AllocString(key));

	value = v;

	return (r == 0);
}

bool GitConfiguration::TryGetInt64(String ^key, [Out] __int64 %value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	GitPool pool;

	value = 0;

	int64_t v = 0;

	int r = git_config_get_int64(&v, Handle, pool.AllocString(key));
	value = v;

	return (r == 0);
}


bool GitConfiguration::TryGetString(String ^key, [Out] String ^%value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	GitPool pool;

	value = nullptr;

	const char *v;

	// Returns string owned by configuration
	int r = git_config_get_string(&v, Handle, pool.AllocString(key));

	value = v ? Utf8_PtrToString(v) : nullptr;

	return (r == 0);
}

bool GitConfiguration::TryGetBoolean(String ^key, [Out] bool ^value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	GitPool pool;

	value = nullptr;

	int v = 0;

	// Returns string owned by configuration
	int r = git_config_get_bool(&v, Handle, pool.AllocString(key));

	value = (v != 0);

	return (r == 0);
}

bool GitConfiguration::Set(String^ key, int value)
{
	return Set(key, gcnew GitNoArgs(), value);
}
bool GitConfiguration::Set(String^ key, __int64 value)
{
	return Set(key, gcnew GitNoArgs(), value);
}

bool GitConfiguration::Set(String^ key, String ^value)
{
	return Set(key, gcnew GitNoArgs(), value);
}

bool GitConfiguration::Set(String^ key, bool value)
{
	return Set(key, gcnew GitNoArgs(), value);
}

bool GitConfiguration::Set(String^ key, GitArgs^ args, int value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	int r = git_config_set_int32(Handle, pool.AllocString(key), value);

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Set(String^ key, GitArgs^ args, __int64 value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	int r = git_config_set_int64(Handle, pool.AllocString(key), value);

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Set(String^ key, GitArgs^ args, String ^value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");
	else if (! value)
		throw gcnew ArgumentNullException("value");

	GitPool pool;

	int r = git_config_set_string(Handle, pool.AllocString(key), pool.AllocString(value));

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Set(String^ key, GitArgs^ args, bool value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	int r = git_config_set_bool(Handle, pool.AllocString(key), value);

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Delete(String ^key)
{
	return Delete(key, gcnew GitNoArgs());
}

bool GitConfiguration::Delete(String ^key, GitArgs ^args)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

		GitPool pool;

	int r = git_config_delete_entry(Handle, pool.AllocString(key));

	return args->HandleGitError(this, r);
}