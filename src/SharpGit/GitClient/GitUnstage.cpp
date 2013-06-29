#include "stdafx.h"
#include "GitClient.h"
#include "GitUnstage.h"

using namespace SharpGit;

bool GitClient::Unstage(String ^path)
{
	return Unstage(path, gcnew GitUnstageArgs());
}

bool GitClient::Unstage(String ^path, GitUnstageArgs^ args)
{
	return Unstage(gcnew array<String^> { path }, args);
}

bool GitClient::Unstage(IEnumerable<String^>^ paths)
{
	return Unstage(paths, gcnew GitUnstageArgs());
}

bool GitClient::Unstage(IEnumerable<String^>^ paths, GitUnstageArgs^ args)
{
	return false;
}
