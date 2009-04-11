// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
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

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Microsoft.Win32;
using QQn.ProcessRunner;
using System.Collections.ObjectModel;
using System.Diagnostics;

namespace SharpSvn.SourceIndexer.Tools
{
    public static class SourceServerTools
    {
        static bool? _available;
        public static bool Available
        {
            get { return (bool)(_available ?? (_available = GetAvailable())); }
        }

        static bool GetAvailable()
        {
            return
                !string.IsNullOrEmpty(PdbStrToolPath) && File.Exists(PdbStrToolPath) &&
                !string.IsNullOrEmpty(SrcToolToolPath) && File.Exists(SrcToolToolPath);
        }

        static string _pdbStrPath;
        public static string PdbStrToolPath
        {
            get { return _pdbStrPath ?? (_pdbStrPath = CalculateToolPath("pdbstr.exe")); }
        }

        static string _srcToolPath;
        public static string SrcToolToolPath
        {
            get { return _srcToolPath ?? (_srcToolPath = CalculateToolPath("srctool.exe")); }
        }

        static string _assemblyDir;
        static string AssemblyDir
        {
            get
            {
                if (_assemblyDir != null)
                    return string.IsNullOrEmpty(_assemblyDir) ? null : _assemblyDir;

                Uri codeBase;
                if (!Uri.TryCreate(typeof(SourceServerTools).Assembly.CodeBase, UriKind.Absolute, out codeBase)
                    || !codeBase.IsFile)
                {
                    _assemblyDir = "";
                    return null;
                }

                return _assemblyDir = SvnTools.GetNormalizedDirectoryName(codeBase.LocalPath);

            }
        }

        /// <summary>
        /// Tries to find the tool with the specified name using the path and knowledge about the Debugging SDK
        /// </summary>
        /// <param name="toolName"></param>
        /// <returns></returns>
        private static string CalculateToolPath(string toolName)
        {
            string path;

            if (AssemblyDir != null && File.Exists(path = Path.Combine(AssemblyDir, toolName)))
                return path;

            try
            {
                path = Registry.CurrentUser.GetValue("Software\\QQn\\SharpSvn\\CurrentVersion\\ToolPaths", null) as string;

                if (!string.IsNullOrEmpty(path) && File.Exists(path=Path.Combine(path, toolName)))
                    return path;

                path = Registry.LocalMachine.GetValue("Software\\QQn\\SharpSvn\\CurrentVersion\\ToolPaths", null) as string;

                if (!string.IsNullOrEmpty(path) && File.Exists(path=Path.Combine(path, toolName)))
                    return path;
            }
            catch { /* Eat all security exceptions */ }

            path = Environment.GetEnvironmentVariable("PATH");

            // First look in the path (as users would expect)
            if (!string.IsNullOrEmpty(path))
                foreach (string dir in path.Split(';'))
                {
                    string d = dir.Trim().Trim('"');
                    if (d.Length == 0)
                        continue;

                    if (!Directory.Exists(dir))
                        continue;

                    path = Path.Combine(dir, toolName);

                    if (File.Exists(path))
                        return path;
                }

            // Look for the Debugging tools for Windows SDK
            string programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);

            if (string.IsNullOrEmpty(programFiles))
                return null;

            path = Path.GetFullPath(programFiles);

            string dirName = Path.GetFileName(path);

            string[] dirsToSearch;
            if (dirName.EndsWith(")") && (dirName.IndexOf('(') > 0))
            {
                // If you are running in .Net 2.0/X86 on Windows X64 you get "c:\program files (X86)"
                // but the tools are in "c:\program files" with the x64 programs

                dirsToSearch = new string[] { path, Path.Combine(Path.GetDirectoryName(path), dirName.Substring(0, dirName.LastIndexOf('('))) };
            }
            else
                dirsToSearch = new string[] { path };

            foreach (string pDir in dirsToSearch)
            {
                DirectoryInfo dir = new DirectoryInfo(pDir);

                if (!dir.Exists)
                    continue;

                foreach (DirectoryInfo subDir in dir.GetDirectories("Debugging Tools for Windows*"))
                {
                    path = Path.Combine(Path.Combine(subDir.FullName, "sdk\\srcsrv"), toolName);

                    if (File.Exists(path))
                        return path;

                    path = Path.Combine(Path.Combine(subDir.FullName, "srcsrv"), toolName);

                    if (File.Exists(path))
                        return path;
                }
            }

            return null;
        }

        public static bool TryGetNotIndexedFiles(string pdbFile, out Collection<string> paths)
        {
            return TryGetNotIndexedFiles(pdbFile, new ProcessRunnerArgs(), out paths);
        }

        public static bool TryGetNotIndexedFiles(string pdbFile, ProcessRunArgs processArgs, out Collection<string> paths)
        {
            return TryGetFiles(pdbFile, true, processArgs, out paths);
        }

        public static bool TryGetAllFiles(string pdbFile, out Collection<string> paths)
        {
            return TryGetAllFiles(pdbFile, new ProcessRunnerArgs(), out paths);
        }

        public static bool TryGetAllFiles(string pdbFile, ProcessRunArgs processArgs, out Collection<string> paths)
        {
            return TryGetFiles(pdbFile, false, processArgs, out paths);
        }

        static bool TryGetFiles(string pdbFile, bool unindexedOnly, ProcessRunArgs processArgs, out Collection<string> paths)
        {
            if (string.IsNullOrEmpty(pdbFile))
                throw new ArgumentNullException("pdbFile");

            if (!File.Exists(pdbFile))
                throw new FileNotFoundException(string.Format("{0} not found", pdbFile));

            pdbFile = Path.GetFullPath(pdbFile);

            if (string.IsNullOrEmpty(SrcToolToolPath))
            {
                paths = null;
                return false;
            }
            while (true)
            {
                string cmdLine = unindexedOnly ? "-u " : "-r ";

                cmdLine += string.Format("\"{0}\"", pdbFile);

                ProcessRunnerArgs pa = new ProcessRunnerArgs(processArgs);

                using (ProcessRunnerResult result = ProcessRunner.Run(SrcToolToolPath, cmdLine, pa))
                {
                    result.WaitForExit();

                    StringBuilder srcToolError = null;

                    if (0 != result.ExitCode)
                    {
                        if (unindexedOnly && result.Output.Count == 1)
                        {
                            string line = result.Output[0].TrimEnd('.').Trim();

                            if (line.EndsWith("is not source indexed", StringComparison.OrdinalIgnoreCase))
                            {
                                unindexedOnly = false;
                                continue;
                            }
                        }

                        srcToolError = new StringBuilder();
                        foreach (string line in result.Output)
                        {
                            srcToolError.AppendFormat(line);
                        }
                    }

                    foreach (string line in result.Error)
                    {
                        if (srcToolError == null)
                            srcToolError = new StringBuilder();

                        srcToolError.AppendLine(line);
                    }

                    if (srcToolError != null)
                        throw new SourceServerToolException(srcToolError.ToString());

                    paths = new Collection<string>();

                    char[] invalidChars = new char[] { ';', '*', '?' };
                    foreach (string line in result.Output)
                    {
                        // line should be a valid path
                        // cc sometimes includes paths containing a '*' -> generated files
                        // cc sometimes includes a path ":123"

                        if (line.IndexOfAny(invalidChars) > 0)
                            continue; // Invalid path; impossible to open

                        int n = line.LastIndexOf(':');

                        if (n > 0 && n != 1)
                            continue; // Invalid path; only the volume can be separated by :

                        paths.Add(line);
                    }

                    return true;
                }
            }
        }

        public static bool IsSourceServerIndexed(string pdbFile)
        {
            return IsSourceServerIndexed(pdbFile, new ProcessRunArgs());
        }
        /// <summary>
        /// Gets a boolean indicating whether the specified file is sourceserver indexed
        /// </summary>
        /// <param name="pdbFile">The PDB file.</param>
        /// <returns>
        /// 	<c>true</c> if the file is source server indexed; otherwise, <c>false</c>.
        /// </returns>
        public static bool IsSourceServerIndexed(string pdbFile, ProcessRunArgs processArgs)
        {
            if (string.IsNullOrEmpty(pdbFile))
                throw new ArgumentNullException("pdbFile");

            if (!File.Exists(pdbFile))
                throw new FileNotFoundException(string.Format("{0} not found", pdbFile));

            pdbFile = Path.GetFullPath(pdbFile);

            if (string.IsNullOrEmpty(SrcToolToolPath))
                return false;

            string cmdLine = string.Format("-c \"{0}\"", pdbFile);

            using (ProcessRunnerResult result = ProcessRunner.Run(SrcToolToolPath, cmdLine, new ProcessRunnerArgs(processArgs)))
            {
                result.WaitForExit();

                if (!result.Error.IsEmpty)
                    throw new SourceServerToolException(result.Error.ToString());

                if (result.ExitCode != 0 && !result.Output.IsEmpty)
                {
                    string line = result.Output[0].TrimEnd('.').Trim();

                    if (line.EndsWith("is not source indexed", StringComparison.OrdinalIgnoreCase))
                        return false;
                }
                else if (result.ExitCode != 0)
                    throw new SourceServerToolException(result.Output.ToString());

                return true;
            }
        }

        public static bool TryExtract(string pdbFile, string toDir, bool flat, ProcessRunArgs processArgs)
        {
            if (string.IsNullOrEmpty(pdbFile))
                throw new ArgumentNullException("pdbFile");
            else if (string.IsNullOrEmpty(toDir))
                throw new ArgumentNullException("toDir");

            if (!File.Exists(pdbFile))
                throw new FileNotFoundException(string.Format("{0} not found", pdbFile));

            pdbFile = Path.GetFullPath(pdbFile);

            if (string.IsNullOrEmpty(SrcToolToolPath))
                return false;

            if (!Directory.Exists(toDir))
                Directory.CreateDirectory(toDir);

            string cmdLine = string.Format("-x \"{0}\" {2}\"-d:{1}\"", pdbFile, toDir, flat ? "-f " : "");

            using (ProcessRunnerResult result = ProcessRunner.Run(SrcToolToolPath, cmdLine, new ProcessRunnerArgs(processArgs)))
            {
                result.WaitForExit();

                return (result.ExitCode == 0);
            }
        }

        public static TextReader GetPdbStream(string pdbFile, string streamName)
        {
            return GetPdbStream(pdbFile, streamName, null);
        }

        public static TextReader GetPdbStream(string pdbFile, string streamName, ProcessRunArgs processArgs)
        {
            if (string.IsNullOrEmpty(pdbFile))
                throw new ArgumentNullException("pdbFile");
            else if (string.IsNullOrEmpty(streamName))
                throw new ArgumentNullException("streamName");

            if (!File.Exists(pdbFile))
                throw new FileNotFoundException(string.Format("{0} not found", pdbFile));

            pdbFile = Path.GetFullPath(pdbFile);

            if (string.IsNullOrEmpty(PdbStrToolPath))
                return null;

            string tempfile = Path.GetFullPath(Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString("N") + ".txt"));
            try
            {
                // We write the file to "con:" instead of a real file to capture the output without tempfile
                string cmdLine = string.Format("-r \"-p:{0}\" \"-s:{1}\" \"-i:{2}\"", pdbFile, streamName, tempfile);                

                using (ProcessRunnerResult result = ProcessRunner.Run(PdbStrToolPath, cmdLine, new ProcessRunnerArgs(processArgs)))
                {
                    result.WaitForExit();

                    if (result.ExitCode != 0)
                        return null;

                    if (File.Exists(tempfile))
                    {
                        using (StreamReader sr = File.OpenText(tempfile))
                        {
                            return new StringReader(sr.ReadToEnd());
                        }
                    }
                    else
                        return null;
                }
            }

            finally
            {
                if (File.Exists(tempfile))
                {
                    try
                    {
                        File.Delete(tempfile);
                    }
                    catch { }
                }
            }
        }
    }
}
