#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using System::Collections::Generic::List;

SvnMergeSources::SvnMergeSources(SvnTarget^ target, apr_array_header_t* mergeSources)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!mergeSources)
		throw gcnew ArgumentNullException("mergeSources");

	_target = target;

	List<Uri^>^ sources = gcnew List<Uri^>(mergeSources->nelts);

	const char** uris = (const char**)mergeSources->elts;

	for(int i = 0; i < sources->Count; i++)
		sources->Add(gcnew Uri(Utf8_PtrToString(*(uris++))));

	_mergeSources = sources->AsReadOnly();
}