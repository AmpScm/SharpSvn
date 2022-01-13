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

#include "SvnChangeList.h"
#include "SvnCommandLineArgumentCollection.h"

namespace SharpSvn {
    using namespace SharpSvn::Implementation;
    using namespace System::Collections::Generic;
    using System::String;

    public ref class SvnTools sealed : SvnBase
    {
    private:
        SvnTools()
        {} // Static class

        literal String^ _hostChars = "._-";
        literal String^ _shareChars = "._-$ ";

        static String^ FindTruePath(String^ path, String^ root, bool bestEffort);

    public:
        /// <summary>Gets the repository Uri of a path, or <c>null</c> if path is not versioned</summary>
        static Uri^ GetUriFromWorkingCopy(String^ path);

        /// <summary>Gets the absolute pathname exactly like it is on disk (fixing casing); returns NULL for non existing paths</summary>
        static String^ GetTruePath(String^ path);

        /// <summary>Gets the absolute pathname exactly like it is on disk (fixing casing).
        /// For not existing paths, if bestEffort is TRUE, returns a path based on existing parents. Otherwise return NULL for not existing paths</summary>
        static String^ GetTruePath(String^ path, bool bestEffort);

        /// <summary>Gets a boolean indicating whether the path could contain a Subversion Working Copy</summary>
        /// <remarks>Assumes path is a directory</remarks>
        static bool IsManagedPath(String^ path);

        /// <summary>Gets a boolean indicating whether at least one of the
        /// parent paths or the path (file/directory) itself is a working copy.
        /// </summary>
        static bool IsBelowManagedPath(String^ path);

        /// <summary>
        /// Normalizes the path to a full path
        /// </summary>
        /// <summary>This normalizes drive letters to upper case and hostnames to lowercase to match Subversion 1.6 behavior</summary>
        static String^ GetNormalizedFullPath(String^ path);

        /// <summary>
        /// Checks whether normalization is required
        /// </summary>
        /// <remarks>This method does not verify the casing of invariant parts</remarks>
        static bool IsNormalizedFullPath(String^ path);

        /// <summary>
        /// Normalizes the Uri to a full absolute Uri
        /// </summary>
        static Uri^ GetNormalizedUri(Uri^ uri);

        /// <summary>
        /// Checks whether the specified path is an absolute path that doesn't end in an unneeded '\'
        /// </summary>
        static bool IsAbsolutePath(String^ path);

        /// <summary>
        /// Appends the specified path suffix to the given Uri
        /// </summary>
        static Uri^ AppendPathSuffix(Uri^ uri, String^ pathSuffix);


        /// <summary>
        /// Converts a string from a Uri path to a local path name; unescaping when necessary
        /// </summary>
        static String^ UriPartToPath(String^ uriPath);

        /// <summary>Gets a file:// style uri for the specified local path</summary>
        static Uri^ LocalPathToUri(String^ localPath, bool endSlash);

        static bool TryGetUriAncestor(IEnumerable<System::Uri^>^ uris, [Out] System::Uri ^%ancestorUri);
        static bool TryGetDirentAncestor(IEnumerable<String^>^ uris, [Out] System::String ^%ancestorDirent);
        static bool TryGetRelativePathAncestor(IEnumerable<String^>^ uris, [Out] System::String ^%ancestorRelpath);

    public:

        /// <summary>
        /// Converts a local relative path to a valid relative Uri
        /// </summary>
        static Uri^ PathToRelativeUri(String^ path);

        /// <summary>Gets the filename of the specified target</summary>
        static String^ GetFileName(Uri^ target);

        /// <summary>Gets the normalized directory name of path (Long path enabled version of <see cref="System::IO::Path::GetDirectoryName(String^)" />, always returning full paths)</summary>
        /// <returns>Directory information for path, or null if path denotes a root directory or is null. Returns String.Empty if path does not contain directory information.</returns>
        static String^ GetNormalizedDirectoryName(String^ path);

        static String^ GetPathRoot(String^ path);

        delegate String^ SplitCommandExpander(String^ from);

        ///
        static bool TrySplitCommandLine(String^ command, SplitCommandExpander^ expander, [Out] String^% application, [Out] String^% arguments);

        /// <summary>Invoke TrySplitCommandLine with <see cref="System::Environment::ExpandEnvironmentVariables"/> as expander</summary>
        static bool TrySplitCommandLine(String^ command, [Out] String^% application, [Out] String^% arguments)
        {
            return TrySplitCommandLine(command, gcnew SplitCommandExpander(&System::Environment::ExpandEnvironmentVariables), application, arguments);
        }

        static bool TryFindApplication(String^ applicationName, [Out] String^% path);

        generic<typename T>
        where T : System::Enum
        static bool IsEnumValueDefined(T value);


    internal:
        /// <summary>Long path capable version of <see cref="System::IO::Path::Combine(String^, String^)" /></summary>
        static String^ PathCombine(String^ path1, String^ path2);
    };
}
