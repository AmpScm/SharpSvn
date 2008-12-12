// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
		throw gcnew ArgumentNullException("diff");

	_diff = diff;
}

generic<typename TToken> where TToken : ref class
SvnDiff<TToken>::~SvnDiff()
{
	_diff = nullptr;
	if (_diffBaton)
		delete _diffBaton;
}
