// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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