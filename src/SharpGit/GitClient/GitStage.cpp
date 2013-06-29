#include "stdafx.h"
#include "GitClient.h"
#include "GitStage.h"

using namespace SharpGit;

bool GitClient::Stage(String ^path)
{
	return Stage(path, gcnew GitStageArgs());
}

bool GitClient::Stage(String ^path, GitStageArgs^ args)
{
	return Stage(gcnew array<String^> { path }, args);
}

bool GitClient::Stage(IEnumerable<String^>^ paths)
{
	return Stage(paths, gcnew GitStageArgs());
}

bool GitClient::Stage(IEnumerable<String^>^ paths, GitStageArgs^ args)
{
	return false;
}
