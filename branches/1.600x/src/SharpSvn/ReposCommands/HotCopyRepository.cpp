#include "stdafx.h"
#include "SvnAll.h"
#include "ReposArgs/HotCopyRepository.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;


bool SvnRepositoryClient::HotCopy(String^ fromRepository, String^ toRepository)
{
	if (String::IsNullOrEmpty(fromRepository))
		throw gcnew ArgumentNullException("fromRepository");
	else if (String::IsNullOrEmpty(toRepository))
		throw gcnew ArgumentNullException("toRepository");
	else if (!IsNotUri(fromRepository))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "fromRepository");
	else if (!IsNotUri(toRepository))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toRepository");

	return HotCopy(fromRepository, toRepository, gcnew SvnHotCopyRepositoryArgs());
}

bool SvnRepositoryClient::HotCopy(String^ fromRepository, String^ toRepository, SvnHotCopyRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(fromRepository))
		throw gcnew ArgumentNullException("fromRepository");
	else if (String::IsNullOrEmpty(toRepository))
		throw gcnew ArgumentNullException("toRepository");
	else if (!IsNotUri(fromRepository))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "fromRepository");
	else if (!IsNotUri(toRepository))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toRepository");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	return args->HandleResult(this,
					   svn_repos_hotcopy(pool.AllocPath(fromRepository),
										 pool.AllocPath(toRepository),
										 args->CleanLogFiles,
										 pool.Handle));
}