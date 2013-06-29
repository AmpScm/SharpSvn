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
	return TryGetInt32(GitConfigurationLevel::Unspecified, key, value);
}

bool GitConfiguration::TryGetInt64(String ^key, [Out] __int64 %value)
{
	return TryGetInt64(GitConfigurationLevel::Unspecified, key, value);
}

bool GitConfiguration::TryGetString(String ^key, [Out] String ^%value)
{
	return TryGetString(GitConfigurationLevel::Unspecified, key, value);
}

bool GitConfiguration::TryGetBoolean(String ^key, [Out] bool ^value)
{
	return TryGetBoolean(GitConfigurationLevel::Unspecified, key, value);
}

bool GitConfiguration::TryGetInt32(GitConfigurationLevel level, String ^key, [Out] int %value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	value = 0;

	GitPool pool;
	git_config *cfg = Handle;
	int r = 0;

	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		int32_t v = 0;

		r = git_config_get_int32(&v, cfg, pool.AllocString(key));

		value = v;

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return (r == 0);
}

bool GitConfiguration::TryGetInt64(GitConfigurationLevel level, String ^key, [Out] __int64 %value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	value = 0;

	GitPool pool;
	git_config *cfg = Handle;
	int r = 0;

	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		int64_t v = 0;

		r = git_config_get_int64(&v, cfg, pool.AllocString(key));

		value = v;

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return (r == 0);
}


bool GitConfiguration::TryGetString(GitConfigurationLevel level, String ^key, [Out] String ^%value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	value = nullptr;

	GitPool pool;
	git_config *cfg = Handle;
	int r = 0;

	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		const char *v;

		// Returns string owned by configuration
		r = git_config_get_string(&v, cfg, pool.AllocString(key));

		value = v ? Utf8_PtrToString(v) : nullptr;

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return (r == 0);
}

bool GitConfiguration::TryGetBoolean(GitConfigurationLevel level, String ^key, [Out] bool ^value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");

	value = false;

	GitPool pool;
	git_config *cfg = Handle;
	int r = 0;

	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		int v = 0;

		r = git_config_get_bool(&v, cfg, pool.AllocString(key));

		value = (v != 0);

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return (r == 0);
}

bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, int value)
{
	return Set(level, key, gcnew GitNoArgs(), value);
}
bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, __int64 value)
{
	return Set(level, key, gcnew GitNoArgs(), value);
}

bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, String ^value)
{
	return Set(level, key, gcnew GitNoArgs(), value);
}

bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, bool value)
{
	return Set(level, key, gcnew GitNoArgs(), value);
}

bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, GitArgs^ args, int value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	git_config *cfg = Handle;
	int r = 0;
	
	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		r = git_config_set_int32(cfg, pool.AllocString(key), value);

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, GitArgs^ args, __int64 value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	git_config *cfg = Handle;
	int r = 0;
	
	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		r = git_config_set_int64(cfg, pool.AllocString(key), value);

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, GitArgs^ args, String ^value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");
	else if (! value)
		throw gcnew ArgumentNullException("value");

	GitPool pool;

	git_config *cfg = Handle;
	int r = 0;
	
	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		r = git_config_set_string(cfg, pool.AllocString(key), pool.AllocString(value));

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Set(GitConfigurationLevel level, String^ key, GitArgs^ args, bool value)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;


	git_config *cfg = Handle;
	int r = 0;
	
	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		r = git_config_set_bool(cfg, pool.AllocString(key), value);

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return args->HandleGitError(this, r);
}

bool GitConfiguration::Delete(GitConfigurationLevel level, String ^key)
{
	return Delete(level, key, gcnew GitNoArgs());
}

bool GitConfiguration::Delete(GitConfigurationLevel level, String ^key, GitArgs ^args)
{
	if (String::IsNullOrEmpty(key))
		throw gcnew ArgumentNullException("key");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	git_config *cfg = Handle;
	int r = 0;
	
	if (level != GitConfigurationLevel::Unspecified)
		r = git_config_open_level(&cfg, Handle, (git_config_level_t)level);

	if (r == 0)
	{
		r = git_config_delete_entry(cfg, pool.AllocString(key));

		if (level != GitConfigurationLevel::Unspecified)
			git_config_free(cfg);
	}

	return args->HandleGitError(this, r);
}
