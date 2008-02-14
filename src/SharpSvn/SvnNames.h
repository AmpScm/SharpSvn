#pragma once
#include "svn_props.h"
#include "svn_config.h"
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

		public ref class SvnConfigNames sealed
		{
			SvnConfigNames()
			{}

		public:
			static initonly String^ ServersCategory					= SVN_CONFIG_CATEGORY_SERVERS;
			static initonly String^		GroupsSection				= SVN_CONFIG_SECTION_GROUPS;
			static initonly String^		GlobalSection				= SVN_CONFIG_SECTION_GLOBAL;
			static initonly String^		HttpProxyHost				= SVN_CONFIG_OPTION_HTTP_PROXY_HOST;
			static initonly String^		HttpProxyPort				= SVN_CONFIG_OPTION_HTTP_PROXY_PORT;
			static initonly String^		HttpProxyUserName			= SVN_CONFIG_OPTION_HTTP_PROXY_USERNAME;
			static initonly String^		HttpProxyPassword			= SVN_CONFIG_OPTION_HTTP_PROXY_PASSWORD;
			static initonly String^		HttpProxyExceptions			= SVN_CONFIG_OPTION_HTTP_PROXY_EXCEPTIONS;
			static initonly String^		HttpTimeout					= SVN_CONFIG_OPTION_HTTP_TIMEOUT;
			static initonly String^		HttpCompression				= SVN_CONFIG_OPTION_HTTP_COMPRESSION;
			/* SVN_CONFIG_OPTION_NEON_DEBUG_MASK: only available in debug builds of svn itself */
			static initonly String^		HttpAuthTypes				= SVN_CONFIG_OPTION_HTTP_AUTH_TYPES;
			static initonly String^		HttpLibrary					= SVN_CONFIG_OPTION_HTTP_LIBRARY;

			static initonly String^		SslAuthorityFiles			= SVN_CONFIG_OPTION_SSL_AUTHORITY_FILES;
			static initonly String^		SslTrustDefaultCertificateAuthority = SVN_CONFIG_OPTION_SSL_TRUST_DEFAULT_CA;
			static initonly String^		SslClientCertificateFile	= SVN_CONFIG_OPTION_SSL_CLIENT_CERT_FILE;
			static initonly String^		SslClientCertificatePassword= SVN_CONFIG_OPTION_SSL_CLIENT_CERT_PASSWORD;

		public:
			static initonly String^ ConfigCategory					= SVN_CONFIG_CATEGORY_CONFIG;
			static initonly String^		AuthSection					= SVN_CONFIG_SECTION_AUTH;
			static initonly String^			StorePasswords			= SVN_CONFIG_OPTION_STORE_PASSWORDS;
			static initonly String^			StoreAuthCredentials	= SVN_CONFIG_OPTION_STORE_AUTH_CREDS;

			static initonly String^		HelpersSection				= SVN_CONFIG_SECTION_HELPERS;
			static initonly String^			EditorCommand			= SVN_CONFIG_OPTION_EDITOR_CMD;
			static initonly String^			DiffCommand				= SVN_CONFIG_OPTION_DIFF_CMD;
			static initonly String^			Diff3Command			= SVN_CONFIG_OPTION_DIFF3_CMD;
			static initonly String^			Diff3HasProgramArguments= SVN_CONFIG_OPTION_DIFF3_HAS_PROGRAM_ARG;
			static initonly String^			MergeToolCommand		= SVN_CONFIG_OPTION_MERGE_TOOL_CMD;

			static initonly String^		MiscellanySection			= SVN_CONFIG_SECTION_MISCELLANY;
			static initonly String^			GlobalIgnores			= SVN_CONFIG_OPTION_GLOBAL_IGNORES;
			static initonly String^			LogEncoding				= SVN_CONFIG_OPTION_LOG_ENCODING;
			static initonly String^			UseCommitTimes			= SVN_CONFIG_OPTION_USE_COMMIT_TIMES;
			static initonly String^			TemplateRoot			= SVN_CONFIG_OPTION_TEMPLATE_ROOT;
			static initonly String^			EnableAutoProps			= SVN_CONFIG_OPTION_ENABLE_AUTO_PROPS;
			static initonly String^			NoUnlock				= SVN_CONFIG_OPTION_NO_UNLOCK;
			static initonly String^			MimeTypesFile			= SVN_CONFIG_OPTION_MIMETYPES_FILE;
			static initonly String^			PreservedConflictFileExtensions	= SVN_CONFIG_OPTION_PRESERVED_CF_EXTS;
			static initonly String^			InteractiveConflicts	= SVN_CONFIG_OPTION_INTERACTIVE_CONFLICTS;

			static initonly String^		TunnelsSection				= SVN_CONFIG_SECTION_TUNNELS;

			static initonly String^		AutoPropsSection			= SVN_CONFIG_SECTION_AUTO_PROPS;


			static initonly System::Collections::ObjectModel::ReadOnlyCollection<String^>^ SvnDefaultGlobalIgnores
				= gcnew System::Collections::ObjectModel::ReadOnlyCollection<String^>(
						dynamic_cast<IList<String^>^>(static_cast<String^>(SVN_CONFIG_DEFAULT_GLOBAL_IGNORES)->Split(' ')));
		};
	}
}
