// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "Log.h"

namespace SharpSvn {

	/// <threadsafety static="true" instance="false"/>
	public ref class SvnMergesMergedArgs : public SvnClientArgs
	{
		SvnRevisionPropertyNameCollection^ _retrieveProperties;
		bool _logChangedPaths;
	internal:
		SvnTarget^ _sourceTarget;
	public:
		SvnMergesMergedArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::MergesMerged;
			}
		}

	public:
		event EventHandler<SvnMergesMergedEventArgs^>^ List;

	protected public:
		virtual void OnList(SvnMergesMergedEventArgs^ e)
		{
			List(this, e);
		}

	public:
		/// <summary>Gets or sets a boolean indicating whether the paths changed in the revision should be provided</summary>
		property bool RetrieveChangedPaths
		{
			bool get()
			{
				return _logChangedPaths;
			}
			void set(bool value)
			{
				_logChangedPaths = value;
			}
		}

		/// <summary>Gets the list of properties to retrieve. Only SVN 1.5+ repositories allow adding custom properties to this list</summary>
		property SvnRevisionPropertyNameCollection^ RetrieveProperties
		{
			SvnRevisionPropertyNameCollection^ get()
			{
				if (!_retrieveProperties)
					_retrieveProperties = gcnew SvnRevisionPropertyNameCollection(false);

				return _retrieveProperties;
			}
		}

	internal:
		property bool RetrievePropertiesUsed
		{
			bool get()
			{
				return _retrieveProperties != nullptr;
			}
		}
	};

}
