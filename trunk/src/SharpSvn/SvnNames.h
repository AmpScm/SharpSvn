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

#pragma once

#include "svn_props.h"
#include "svn_config.h"
namespace SharpSvn {

	public ref class SvnKeywords sealed
	{
		SvnKeywords()
		{}

	public:
		/// <summary>The maximum size of an expanded or un-expanded keyword.</summary>
		literal int MaxLength = SVN_KEYWORD_MAX_LEN;

		/// <summary>The most recent revision in which this file was changed</summary>
		literal String^ LastChangedRevision = SVN_KEYWORD_REVISION_LONG;

		/// <summary>Short version of LastChangedRevision</summary>
		literal String^ Rev = SVN_KEYWORD_REVISION_SHORT;

		/// <summary>Medium version of LastChangedRevision, matching the one CVS uses</summary>
		literal String^ Revision = SVN_KEYWORD_REVISION_MEDIUM;

		/// <summary>The most recent date (repository time) when this file was changed.</summary>
		literal String^ LastChangedDate = SVN_KEYWORD_DATE_LONG;

		/// <summary>Short version of LastChangedDate</summary>
		literal String^ Date = SVN_KEYWORD_DATE_SHORT;

		/// <summary>Who most recently committed to this file.</summary>
		literal String^ LastChangedBy = SVN_KEYWORD_AUTHOR_LONG;

		/// <summary>Short version of LastChangedBy</summary>
		literal String^ Author = SVN_KEYWORD_AUTHOR_SHORT;

		/// <summary>The URL for the head revision of this file.</summary>
		literal String^ HeadURL = SVN_KEYWORD_URL_LONG;

		/// <summary>Short version of HeadURL</summary>
		literal String^ URL = SVN_KEYWORD_URL_SHORT;

		/// <summary>A compressed combination of the other four keywords.</summary>
		literal String^ Id = SVN_KEYWORD_ID;

		/// <summary>A full combination of the first four keywords.</summary>
		literal String^ Header = SVN_KEYWORD_HEADER;

		/// <summary>Gets a list of TortoiseSvn defined directory properties, which are applied recursively on all directories below the set root</summary>
		static initonly System::Collections::ObjectModel::ReadOnlyCollection<String^>^ PredefinedKeywords = gcnew System::Collections::ObjectModel::ReadOnlyCollection<String^>(dynamic_cast<IList<String^>^>(gcnew array<String^>
		{
			LastChangedRevision,
			Rev,
			Revision,
			LastChangedDate,
			Date,
			LastChangedBy,
			Author,
			HeadURL,
			URL,
			Id,
			Header
		}));
	};

	public ref class SvnPropertyNames sealed
	{
		SvnPropertyNames()
		{}

	public:
		/// <summary>All Subversion property names start with this</summary>
		literal String^ SvnPropertyPrefix = SVN_PROP_PREFIX;
		/// <summary>
		/// Properties whose values are interpreted as booleans (such as
		/// svn:executable, svn:needs_lock, and svn:special) always fold their
		/// value to this.
		/// </summary>
		literal String^ SvnBooleanValue = SVN_PROP_BOOLEAN_TRUE;

		literal String^ SvnMimeType = SVN_PROP_MIME_TYPE;
		literal String^ SvnIgnore = SVN_PROP_IGNORE;
		literal String^ SvnEolStyle = SVN_PROP_EOL_STYLE;
		literal String^ SvnKeywords = SVN_PROP_KEYWORDS;
		literal String^ SvnExecutable = SVN_PROP_EXECUTABLE;
		literal String^ SvnNeedsLock = SVN_PROP_NEEDS_LOCK;
		literal String^ SvnSpecial = SVN_PROP_SPECIAL;
		literal String^ SvnExternals = SVN_PROP_EXTERNALS;
		literal String^ SvnMergeInfo = SVN_PROP_MERGEINFO;

		literal String^ SvnAuthor = SVN_PROP_REVISION_AUTHOR;
		literal String^ SvnLog = SVN_PROP_REVISION_LOG;
		literal String^ SvnDate = SVN_PROP_REVISION_DATE;
		literal String^ SvnOriginalDate = SVN_PROP_REVISION_ORIG_DATE;
		literal String^ SvnAutoVersioned = SVN_PROP_REVISION_AUTOVERSIONED;

		literal String^ SvnSyncLock = SVNSYNC_PROP_LOCK;
		literal String^ SvnSyncFromUrl = SVNSYNC_PROP_FROM_URL;
		literal String^ SvnSyncFromUuid = SVNSYNC_PROP_FROM_UUID;
		literal String^ SvnSyncLastMergedRev = SVNSYNC_PROP_LAST_MERGED_REV;
		literal String^ SvnSyncCurrentlyCopying = SVNSYNC_PROP_CURRENTLY_COPYING;

		static initonly System::Collections::ObjectModel::ReadOnlyCollection<String^>^ AllSvnRevisionProperties = gcnew System::Collections::ObjectModel::ReadOnlyCollection<String^>(dynamic_cast<IList<String^>^>(gcnew array<String^> {SVN_PROP_REVISION_ALL_PROPS }));

		// From TortoiseSvn src\TortoiseProc\ProjectProperties.h
		literal String^ BugTrackPrefix			= "bugtraq:";
		literal String^ BugTrackLabel			= "bugtraq:label";
		literal String^ BugTrackMessage         = "bugtraq:message";
		literal String^ BugTrackNumber			= "bugtraq:number";
		literal String^ BugTrackLogRegex		= "bugtraq:logregex";
		literal String^ BugTrackUrl				= "bugtraq:url";
		literal String^ BugTrackWarnIfNoIssue	= "bugtraq:warnifnoissue";
		literal String^ BugTrackAppend			= "bugtraq:append";

		literal String^ TortoiseSvnPrefix			= "tsvn:";
		literal String^ TortoiseSvnLogTemplate		= "tsvn:logtemplate";
		literal String^ TortoiseSvnLogWidthLine		= "tsvn:logwidthmarker";
		literal String^ TortoiseSvnLogMinSize		= "tsvn:logminsize";
		literal String^ TortoiseSvnLockMsgMinSize	= "tsvn:lockmsgminsize";
		literal String^ TortoiseSvnLogFileListLang	= "tsvn:logfilelistenglish";
		literal String^ TortoiseSvnLogSummary		= "tsvn:logsummary";
		literal String^ TortoiseSvnProjectLanguage  = "tsvn:projectlanguage";
		literal String^ TortoiseSvnUserFileProperty = "tsvn:userfileproperties";
		literal String^ TortoiseSvnUserDirProperty  = "tsvn:userdirproperties";
		literal String^ TortoiseSvnAutoProps		= "tsvn:autoprops";

		literal String^ WebViewerPrefix				= "webviewer:";
		literal String^ WebViewerRevision			= "webviewer:revision";
		literal String^ WebViewerPathRevision		= "webviewer:pathrevision";

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

	public:
		/// <summary>Tries to parse a Svn property into a <see cref="DateTime" /> value</summary>
		static bool TryParseDate(String^ date, [Out] DateTime% value);
		/// <summary>Formats a DateTime in Subversions string format</summary>
		static String^ FormatDate(DateTime value);
	};

	public ref class SvnConfigNames sealed
	{
		SvnConfigNames()
		{}

	public:
		literal String^ ServersCategory					= SVN_CONFIG_CATEGORY_SERVERS;
		literal String^		GroupsSection				= SVN_CONFIG_SECTION_GROUPS;
		literal String^		GlobalSection				= SVN_CONFIG_SECTION_GLOBAL;
		literal String^		HttpProxyHost				= SVN_CONFIG_OPTION_HTTP_PROXY_HOST;
		literal String^		HttpProxyPort				= SVN_CONFIG_OPTION_HTTP_PROXY_PORT;
		literal String^		HttpProxyUserName			= SVN_CONFIG_OPTION_HTTP_PROXY_USERNAME;
		literal String^		HttpProxyPassword			= SVN_CONFIG_OPTION_HTTP_PROXY_PASSWORD;
		literal String^		HttpProxyExceptions			= SVN_CONFIG_OPTION_HTTP_PROXY_EXCEPTIONS;
		literal String^		HttpTimeout					= SVN_CONFIG_OPTION_HTTP_TIMEOUT;
		literal String^		HttpCompression				= SVN_CONFIG_OPTION_HTTP_COMPRESSION;
		/* SVN_CONFIG_OPTION_NEON_DEBUG_MASK: only available in debug builds of svn itself */
		literal String^		HttpAuthTypes				= SVN_CONFIG_OPTION_HTTP_AUTH_TYPES;
		literal String^		HttpLibrary					= SVN_CONFIG_OPTION_HTTP_LIBRARY;

		literal String^		SslAuthorityFiles			= SVN_CONFIG_OPTION_SSL_AUTHORITY_FILES;
		literal String^		SslTrustDefaultCertificateAuthority = SVN_CONFIG_OPTION_SSL_TRUST_DEFAULT_CA;
		literal String^		SslClientCertificateFile	= SVN_CONFIG_OPTION_SSL_CLIENT_CERT_FILE;
		literal String^		SslClientCertificatePassword= SVN_CONFIG_OPTION_SSL_CLIENT_CERT_PASSWORD;

	public:
		literal String^ ConfigCategory					= SVN_CONFIG_CATEGORY_CONFIG;
		literal String^		AuthSection					= SVN_CONFIG_SECTION_AUTH;
		literal String^			StorePasswords			= SVN_CONFIG_OPTION_STORE_PASSWORDS;
		literal String^			StoreAuthCredentials	= SVN_CONFIG_OPTION_STORE_AUTH_CREDS;

		literal String^		HelpersSection				= SVN_CONFIG_SECTION_HELPERS;
		literal String^			EditorCommand			= SVN_CONFIG_OPTION_EDITOR_CMD;
		literal String^			DiffCommand				= SVN_CONFIG_OPTION_DIFF_CMD;
		literal String^			Diff3Command			= SVN_CONFIG_OPTION_DIFF3_CMD;
		literal String^			Diff3HasProgramArguments= SVN_CONFIG_OPTION_DIFF3_HAS_PROGRAM_ARG;
		literal String^			MergeToolCommand		= SVN_CONFIG_OPTION_MERGE_TOOL_CMD;

		literal String^		MiscellanySection			= SVN_CONFIG_SECTION_MISCELLANY;
		literal String^			GlobalIgnores			= SVN_CONFIG_OPTION_GLOBAL_IGNORES;
		literal String^			LogEncoding				= SVN_CONFIG_OPTION_LOG_ENCODING;
		literal String^			UseCommitTimes			= SVN_CONFIG_OPTION_USE_COMMIT_TIMES;
		literal String^			TemplateRoot			= SVN_CONFIG_OPTION_TEMPLATE_ROOT;
		literal String^			EnableAutoProps			= SVN_CONFIG_OPTION_ENABLE_AUTO_PROPS;
		literal String^			NoUnlock				= SVN_CONFIG_OPTION_NO_UNLOCK;
		literal String^			MimeTypesFile			= SVN_CONFIG_OPTION_MIMETYPES_FILE;
		literal String^			PreservedConflictFileExtensions	= SVN_CONFIG_OPTION_PRESERVED_CF_EXTS;
		literal String^			InteractiveConflicts	= SVN_CONFIG_OPTION_INTERACTIVE_CONFLICTS;

		literal String^		TunnelsSection				= SVN_CONFIG_SECTION_TUNNELS;

		literal String^		AutoPropsSection			= SVN_CONFIG_SECTION_AUTO_PROPS;

		static initonly System::Collections::ObjectModel::ReadOnlyCollection<String^>^ SvnDefaultGlobalIgnores
			= gcnew System::Collections::ObjectModel::ReadOnlyCollection<String^>(
			dynamic_cast<IList<String^>^>(static_cast<String^>(SVN_CONFIG_DEFAULT_GLOBAL_IGNORES)->Split(' ')));
	};
}
