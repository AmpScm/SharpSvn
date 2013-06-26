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
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	return args->HandleResult(this,
					   svn_repos_hotcopy2(pool.AllocDirent(fromRepository),
										  pool.AllocDirent(toRepository),
										  args->CleanLogFiles,
										  args->Incremental,
										  CtxHandle->cancel_func,
										  CtxHandle->cancel_baton,
										  pool.Handle));
}