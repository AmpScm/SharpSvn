// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnEnums.h"
#include "SvnCommitItem.h"
#include "svn_compat.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;
	using System::Collections::ObjectModel::KeyedCollection;

	ref class SvnException;
	ref class SvnMergeRange;

	public ref class SvnEventArgs abstract : public System::EventArgs
	{
	protected:
		SvnEventArgs()
		{
		}

	public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties values are stored in managed code</description>
		void Detach()
		{
			Detach(true);
		}
	protected public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties)
		{
			UNUSED_ALWAYS(keepProperties);
		}

	internal:
		static int SafeGetHashCode(Object^ value)
		{
			return value ? value->GetHashCode() : 0;
		}
	};
};

#include "EventArgs/SvnCancelEventArgs.h"
#include "EventArgs/SvnCommittingEventArgs.h"
#include "EventArgs/SvnConflictEventArgs.h"
#include "EventArgs/SvnNotifyEventArgs.h"
#include "EventArgs/SvnBlameEventArgs.h"
#include "EventArgs/SvnDiffSummaryEventArgs.h"
#include "EventArgs/SvnErrorEventArgs.h"
#include "EventArgs/SvnInfoEventArgs.h"
#include "EventArgs/SvnListEventArgs.h"
#include "EventArgs/SvnListChangeListEventArgs.h"
#include "EventArgs/SvnLogEventArgs.h"
#include "EventArgs/SvnMergesEligibleEventArgs.h"
#include "EventArgs/SvnMergesMergedEventArgs.h"
#include "EventArgs/SvnProcessingEventArgs.h"
#include "EventArgs/SvnProgressEventArgs.h"
#include "EventArgs/SvnPropertyListEventArgs.h"
#include "EventArgs/SvnStatusEventArgs.h"

#include "SvnWorkingCopyState.h"

