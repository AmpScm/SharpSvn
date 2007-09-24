#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::GetSuggestedMergeSources(SvnTarget ^target, [Out]IList<Uri^>^% mergeSources)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	GetSuggestedMergeSources(target, gcnew SvnGetSuggestedMergeSourcesArgs(), mergeSources);
}

bool SvnClient::GetSuggestedMergeSources(SvnTarget ^target, SvnGetSuggestedMergeSourcesArgs^ args, [Out]IList<Uri^>^% mergeSources)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	mergeSources = nullptr;

	apr_array_header_t* svnMergeSources = nullptr;

	svn_opt_revision_t rev = target->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);
	svn_error_t* err = svn_client_suggest_merge_sources(
		&svnMergeSources,
		pool.AllocString(target->TargetName),
		&rev,
		CtxHandle,
		pool.Handle);

	if(!err && svnMergeSources)
	{
		array<Uri^>^ uris = gcnew array<Uri^>(svnMergeSources->nelts);

		const char** sources = (const char**)svnMergeSources->elts;
		for(int i = 0; i < svnMergeSources->nelts; i++)
		{
			uris[i] = gcnew Uri(Utf8_PtrToString(sources[i]), UriKind::Absolute);
		}
		
		mergeSources = safe_cast<IList<Uri^>^>(uris);
	}

	return args->HandleResult(err);
}