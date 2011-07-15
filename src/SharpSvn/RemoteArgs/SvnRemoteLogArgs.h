#pragma once

#include "SvnRemoteSessionArgs.h"

namespace SharpSvn {
    ref class SvnRevisionPropertyNameCollection;

	namespace Remote {

	public ref class SvnRemoteLogArgs : public SvnRemoteSessionArgs
	{
        int _limit;
        bool _noLogChangedPaths;
        bool _strictNodeHistory;
        bool _includeMerged;
        bool _retrieveAllProperties;
        SvnRevisionPropertyNameCollection^ _retrieveProperties;

    internal:
        int _mergeLogLevel;

	public:
		DECLARE_EVENT(SvnRemoteLogEventArgs^, Log);

	protected public:
		virtual void OnLog(SvnRemoteLogEventArgs^ e)
		{
			Log(this, e);
		}

    public:
        /// <summary>Gets or sets the limit on the number of log items fetched</summary>
		property int Limit
		{
			int get()
			{
				return _limit;
			}

			void set(int value)
			{
				_limit = (value >= 0) ? value : 0;
			}
		}

		/// <summary>Gets or sets a boolean indicating whether the paths changed in the revision should be provided</summary>
		property bool RetrieveChangedPaths
		{
			bool get()
			{
				return !_noLogChangedPaths;
			}
			void set(bool value)
			{
				_noLogChangedPaths = !value;
			}
		}

        /// <summary>Gets or sets a boolean indicating whether only the history of this exact node should be fetched (Aka stop on copying)</summary>
		/// <remarks>If @a StrictNodeHistory is set, copy history (if any exists) will
		/// not be traversed while harvesting revision logs for each target. </remarks>
		property bool StrictNodeHistory
		{
			bool get()
			{
				return _strictNodeHistory;
			}
			void set(bool value)
			{
				_strictNodeHistory = value;
			}
		}

		/// <summary>Gets or sets a boolean indicating whether the merged revisions should be fetched instead of the node changes</summary>
		property bool RetrieveMergedRevisions
		{
			bool get()
			{
				return _includeMerged;
			}
			void set(bool value)
			{
				_includeMerged = value;
			}
		}

    /// <summary>Gets the list of properties to retrieve. Only SVN 1.5+ repositories allow adding custom properties to this list</summary>
		/// <remarks>This list defaults to the author, date, logmessage properties. Clear the list to retrieve no properties,
		/// or set RetrieveAllProperties to true to retrieve all properties.
		/// </remarks>
		property SvnRevisionPropertyNameCollection^ RetrieveProperties
		{
			SvnRevisionPropertyNameCollection^ get();
		}

		/// <summary>Gets or sets a boolean indication whether to retrieve all revision properties</summary>
		/// <remarks>Default value: false</remarks>
		property bool RetrieveAllProperties
		{
			bool get()
			{
				return _retrieveAllProperties;
			}
			void set(bool value)
			{
				_retrieveAllProperties = value;
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
}