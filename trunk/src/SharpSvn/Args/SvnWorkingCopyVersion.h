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

#pragma once

#include "AprArray.h"

namespace SharpSvn {
	using namespace SharpSvn::Implementation;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;

	ref class SvnCommitItem;
	ref class SvnClient;
	ref class SvnClientArgs;

	public ref class SvnWorkingCopyVersion sealed : public SvnCommandResult
	{
		initonly __int64 _minRev;
		initonly __int64 _maxRev;
		initonly bool _switched;
		initonly bool _modified;
		initonly bool _sparse;

	internal:
		SvnWorkingCopyVersion(__int64 minrev, __int64 maxrev, bool switched, bool modified, bool sparse)
		{
			_minRev = minrev;
			_maxRev = maxrev;
			_switched = switched;
			_modified = modified;
			_sparse = sparse;
		}

	public:
		property __int64 Start
		{
			__int64 get()
			{
				return _minRev;
			}
		}

		property __int64 End
		{
			__int64 get()
			{
				return _maxRev;
			}
		}

		property bool Switched
		{
			bool get()
			{
				return _switched;
			}
		}

		property bool Modified
		{
			bool get()
			{
				return _modified;
			}
		}

		property bool IncompleteWorkingCopy
		{
			bool get()
			{
				return _sparse;
			}
		}
	};

};