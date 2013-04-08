#include "stdafx.h"
#include "GitClient.h"

using namespace SharpGit;

GitClient::~GitClient()
{
}

System::Version^ GitClient::Version::get()
{
	int major, minor, rev;

	git_libgit2_version(&major, &minor, &rev);

	return gcnew System::Version(major, minor, rev);
}

System::Version^ GitClient::SharpGitVersion::get()
{
	return (gcnew System::Reflection::AssemblyName(GitClient::typeid->Assembly->FullName))->Version;
}
