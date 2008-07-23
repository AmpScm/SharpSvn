// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"
#include "SvnDiff.h"
#include "UnmanagedStructs.h"

using namespace SharpSvn::Diff;

generic<typename TToken> where TToken : ref class
SvnDiffSource<TToken>::SvnDiffSource()
{
}

generic<typename TToken> where TToken : ref class
SvnDiffSource<TToken>::~SvnDiffSource()
{
}

generic<typename TToken> where TToken : ref class
SvnDiff<TToken>::SvnDiff(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified)
{
	Init(original, modified, nullptr, nullptr);
}

generic<typename TToken> where TToken : ref class
SvnDiff<TToken>::SvnDiff(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified, SvnDiffSource<TToken>^ latest)	
{
	Init(original, modified, latest, nullptr);
}

generic<typename TToken> where TToken : ref class
SvnDiff<TToken>::SvnDiff(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified, SvnDiffSource<TToken>^ latest, SvnDiffSource<TToken>^ ancestor)
{
	Init(original, modified, latest, ancestor);
}

generic<typename TToken> where TToken : ref class
void SvnDiff<TToken>::Init(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified, SvnDiffSource<TToken>^ latest, SvnDiffSource<TToken>^ ancestor)
{
	if (!original)
		throw gcnew ArgumentNullException("original");
	else if (!modified)
		throw gcnew ArgumentNullException("modified");
	else if (!latest && ancestor)
		throw gcnew ArgumentNullException("latest");
	// ancestor can be null!

	throw gcnew NotImplementedException();
}


generic<typename TToken> where TToken : ref class
SvnDiff<TToken>::SvnDiff(svn_diff_t* diff, AprPool^ pool)
	: _pool(pool)
{
	if (!diff)
		throw gcnew ArgumentException("diff");

	_diff = diff;
}

generic<typename TToken> where TToken : ref class
SvnDiff<TToken>::~SvnDiff()
{
	_diff = nullptr;
	if (_diffBaton)
		delete _diffBaton;
}
