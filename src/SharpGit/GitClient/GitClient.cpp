#include "stdafx.h"

#include "GitClient.h"
#include "GitLibraryAttribute.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace SharpGit;
using namespace SharpGit::Implementation;

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

static int CompareLibrary(GitLibrary^ x, GitLibrary^ y)
{
	return StringComparer::OrdinalIgnoreCase->Compare(x->Name, y->Name);
}

ICollection<GitLibrary^>^ GitClient::GitLibraries::get()
{
	if (_gitLibraries)
		return _gitLibraries;

	System::Collections::Generic::List<GitLibrary^>^ libs
		= gcnew System::Collections::Generic::List<GitLibrary^>();

	for each(GitLibraryAttribute^ i in GitClient::typeid->Assembly->GetCustomAttributes(GitLibraryAttribute::typeid, false))
	{
		libs->Add(gcnew GitLibrary(i));
	}

	libs->Sort(gcnew Comparison<GitLibrary^>(CompareLibrary));

	return _gitLibraries = libs->AsReadOnly();
}
