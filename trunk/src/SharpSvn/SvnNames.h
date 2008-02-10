#pragma once
#include "svn_props.h"
namespace SharpSvn {
	namespace Implementation {


		public ref class SvnPropertyNames sealed 
		{
			SvnPropertyNames()
			{}

		public:
			/// <summary>All Subversion property names start with this</summary>
			static initonly String^ SvnPropertyPrefix = SVN_PROP_PREFIX;
			/// <summary>
			/// Properties whose values are interpreted as booleans (such as
			/// svn:executable, svn:needs_lock, and svn:special) always fold their
			/// value to this.
			/// </summary>
			static initonly String^ SvnBooleanValue = SVN_PROP_BOOLEAN_TRUE;

			static initonly String^ SvnMimeType = SVN_PROP_MIME_TYPE;
			static initonly String^ SvnIgnore = SVN_PROP_IGNORE;
			static initonly String^ SvnEolStyle = SVN_PROP_EOL_STYLE;
			static initonly String^ SvnKeywords = SVN_PROP_KEYWORDS;
			static initonly String^ SvnExecutable = SVN_PROP_EXECUTABLE;
			static initonly String^ SvnNeedsLock = SVN_PROP_NEEDS_LOCK;
			static initonly String^ SvnSpecial = SVN_PROP_SPECIAL;
			static initonly String^ SvnExternals = SVN_PROP_EXTERNALS;
			static initonly String^ SvnMergeInfo = SVN_PROP_MERGEINFO;

			static initonly String^ SvnAuthor = SVN_PROP_REVISION_AUTHOR;
			static initonly String^ SvnLog = SVN_PROP_REVISION_LOG;
			static initonly String^ SvnDate = SVN_PROP_REVISION_DATE;
			static initonly String^ SvnOriginalDate = SVN_PROP_REVISION_ORIG_DATE;
			static initonly String^ SvnAutoVersioned = SVN_PROP_REVISION_AUTOVERSIONED;

			static initonly String^ SvnSyncLock = SVNSYNC_PROP_LOCK;
			static initonly String^ SvnSyncFromUrl = SVNSYNC_PROP_FROM_URL;
			static initonly String^ SvnSyncFromUuid = SVNSYNC_PROP_FROM_UUID;
			static initonly String^ SvnSyncLastMergedRev = SVNSYNC_PROP_LAST_MERGED_REV;
			static initonly String^ SvnSyncCurrentlyCopying = SVNSYNC_PROP_CURRENTLY_COPYING;

			static initonly System::Collections::ObjectModel::ReadOnlyCollection<String^>^ AllSvnRevisionProperties = gcnew System::Collections::ObjectModel::ReadOnlyCollection<String^>(dynamic_cast<IList<String^>^>(gcnew array<String^> {SVN_PROP_REVISION_ALL_PROPS }));

			// From TortoiseSvn src\TortoiseProc\ProjectProperties.h
			static initonly String^ BugTrackPrefix			= "bugtraq:";
			static initonly String^ BugTrackLabel			= "bugtraq:label";
			static initonly String^ BugTrackMessage         = "bugtraq:message";
			static initonly String^ BugTrackNumber			= "bugtraq:number";
			static initonly String^ BugTrackLogRegex		= "bugtraq:logregex";
			static initonly String^ BugTrackUrl				= "bugtraq:url";
			static initonly String^ BugTrackWarnIfNoIssue	= "bugtraq:warnifnoissue";
			static initonly String^ BugTrackAppend			= "bugtraq:append";

			static initonly String^ TortoiseSvnPrefix			= "tsvn:";
			static initonly String^ TortoiseSvnLogTemplate		= "tsvn:logtemplate";
			static initonly String^ TortoiseSvnLogWidthLine		= "tsvn:logwidthmarker";
			static initonly String^ TortoiseSvnLogMinSize		= "tsvn:logminsize";
			static initonly String^ TortoiseSvnLockMsgMinSize	= "tsvn:lockmsgminsize";
			static initonly String^ TortoiseSvnLogFileListLang	= "tsvn:logfilelistenglish";
			static initonly String^ TortoiseSvnLogSummary		= "tsvn:logsummary";
			static initonly String^ TortoiseSvnProjectLanguage  = "tsvn:projectlanguage";
			static initonly String^ TortoiseSvnUserFileProperty = "tsvn:userfileproperties";
			static initonly String^ TortoiseSvnUserDirProperty  = "tsvn:userdirproperties";
			static initonly String^ TortoiseSvnAutoProps		= "tsvn:autoprops";

			static initonly String^ WebViewerPrefix				= "webviewer:";
			static initonly String^ WebViewerRevision			= "webviewer:revision";
			static initonly String^ WebViewerPathRevision		= "webviewer:pathrevision";

			/// <summary>Gets a list of TortoiseSvn defined directory properties, which are applied recursively on all directories below the set root</summary>
			static initonly System::Collections::ObjectModel::ReadOnlyCollection<String^>^ TortoiseSvnDirectoryProperties = gcnew System::Collections::ObjectModel::ReadOnlyCollection<String^>(dynamic_cast<IList<String^>^>(gcnew array<String^> 
				{  
					BugTrackLabel,
					BugTrackMessage,
					BugTrackNumber,
					BugTrackLogRegex,
					BugTrackUrl,
					BugTrackWarnIfNoIssue,
					BugTrackAppend,

					TortoiseSvnLogTemplate,
					TortoiseSvnLogWidthLine,
					TortoiseSvnLogMinSize,
					TortoiseSvnLogFileListLang,
					TortoiseSvnLogSummary,
					TortoiseSvnProjectLanguage,
					TortoiseSvnUserFileProperty,
					TortoiseSvnUserDirProperty,
					TortoiseSvnAutoProps
				}));
			// /TortoiseSvn
		};
	}
}
