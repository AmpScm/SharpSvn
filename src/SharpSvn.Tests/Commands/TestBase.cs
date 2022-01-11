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

// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

using SharpSvn.Tests.Commands.Utils;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests.Commands
{
    public enum TestReposType
    {
        Empty,
        EmptyNoMerge,
        AnkhRepos,
        CollabRepos,
        GreekRepos
    }
    /// <summary>
    /// Serves as a base class for tests for NSvn::Core::Add
    /// </summary>
    public class TestBase : IHasTestContext
    {
        private SvnClient client;

        public TestBase()
        {
            string asm = this.GetType().FullName;
            this.client = new SvnClient();
            client.Configuration.LogMessageRequired = false;
            client.Notify += new EventHandler<SvnNotifyEventArgs>(OnClientNotify);

            SetupAuth(client);
        }

        public bool IsCore()
        {
#if NETCOREAPP
            return true;
#elif NETFRAMEWORK
            return false;
#else
#error What?
#endif
        }
        public static void SetupAuth(SvnClient client)
        {
            client.Authentication.UserNamePasswordHandlers += (sender, e) =>
            {
                if (e.RealmUri == new Uri("https://ctf.open.collab.net/"))
                {
                    e.UserName = "guest";
                    e.Password = "";
                }
            };
            client.Authentication.SslServerTrustHandlers += (sender, e) =>
            {
                if (e.RealmUri == new Uri("https://ctf.open.collab.net/"))
                {
                    e.AcceptedFailures = e.Failures;
                }
            };
        }

        protected SvnClient NewSvnClient(bool expectCommit, bool expectConflict)
        {
            SvnClient client = new SvnClient();

            client.Conflict += delegate(object sender, SvnConflictEventArgs e)
            {
                Assert.That(true, Is.EqualTo(expectConflict), "Conflict expected");
            };

            client.Committing += delegate(object sender, SvnCommittingEventArgs e)
            {
                Assert.That(true, Is.EqualTo(expectCommit), "Commit expected");
                if (e.LogMessage == null)
                    e.LogMessage = "";

                GC.KeepAlive(e.Items);
            };

            SetupAuth(client);

            client.Notify += OnClientNotify;

            return client;
        }

        protected virtual void OnClientNotify(object sender, SvnNotifyEventArgs e)
        {
            if (!Enum.IsDefined(typeof(SvnNotifyAction), e.Action))
                Console.WriteLine("Enum value {0} not defined in SvnNotifyAction", e.Action);

            if (e.Uri != null)
            {
            }
            else if (e.Path != null)
            {
                bool found = File.Exists(e.FullPath) || Directory.Exists(e.FullPath) || Directory.Exists(Path.GetDirectoryName(e.FullPath));

                if (e.CommandType != SvnCommandType.Blame
                    && !(e.CommandType == SvnCommandType.Move && e.Action == SvnNotifyAction.Delete))
                {
                    Assert.That(found,
                        "{0} is not a valid path and it's directory does not exist\n (Raw value = {1}, Current Directory = {2}, Action = {3}, CommandType = {4})",
                        e.FullPath, e.Path, Environment.CurrentDirectory, e.Action, e.CommandType);
                }
            }
        }

        public static string ProjectBase
        {
            get
            {
                string assemblyDir = Path.GetDirectoryName(new Uri(typeof(TestBase).Assembly.CodeBase).LocalPath);

                return Path.GetFullPath(Path.Combine(assemblyDir, "..\\..\\.."));
            }
        }

        /// <summary>
        /// Determines the SVN status of a given path
        /// </summary>
        /// <param name="path">The path to check</param>
        /// <returns>Same character codes as used by svn st</returns>
        public SvnStatus GetSvnStatus(string path)
        {
            SvnStatusArgs sa = new SvnStatusArgs();
            sa.Depth = SvnDepth.Empty;
            sa.AddExpectedError(SvnErrorCode.SVN_ERR_WC_PATH_NOT_FOUND);
            Collection<SvnStatusEventArgs> results;

            if (!Client.GetStatus(path, sa, out results)
                || results.Count != 1)
            {
                return SvnStatus.None;
            }

            return results[0].LocalNodeStatus;
        }

        /// <summary>
        /// Runs a command
        /// </summary>
        /// <param name="command">The command to run</param>
        /// <param name="args">Arguments to the command</param>
        /// <returns>The output from the command</returns>
        public string RunCommand(string command, string args)
        {
            if (command == "svn")
                command = Path.GetFullPath(Path.Combine(ProjectBase, "..\\..\\imports\\release\\bin\\svn.exe"));

            //System.Diagnostics.Trace.Assert(File.Exists(command), "Command exists");

            ProcessStartInfo psi = new ProcessStartInfo(command, args);
            psi.CreateNoWindow = true;
            psi.RedirectStandardOutput = true;
            psi.RedirectStandardError = true;
            psi.UseShellExecute = false;

            Process proc = Process.Start(psi);

            //Console.WriteLine( proc.MainModule.FileName );

            ProcessReader outreader = new ProcessReader(proc.StandardOutput);
            ProcessReader errreader = new ProcessReader(proc.StandardError);
            outreader.Start();
            errreader.Start();

            proc.WaitForExit();

            outreader.Wait();
            errreader.Wait();

            if (proc.ExitCode != 0)
                throw new ApplicationException("command exit code was " +
                    proc.ExitCode.ToString() +
                    Environment.NewLine + errreader.Output + Environment.NewLine +
                    "Command was " +
                    proc.StartInfo.FileName + " " + proc.StartInfo.Arguments);


            // normalize newlines
            string[] lines = Regex.Split(outreader.Output, @"\r?\n");
            return String.Join(Environment.NewLine, lines);
        }

        /// <summary>
        /// The client object.
        /// </summary>
        public SvnClient Client
        {
            [System.Diagnostics.DebuggerStepThrough]
            get { return this.client; }
        }

        /// <summary>
        /// Creates a textfile with the given name in the WC
        /// </summary>
        /// <param name="name">The name of the ifle to create</param>
        /// <returns>The path to the created text file</returns>
        protected static string CreateTextFile(string WcPath, string name)
        {
            string path = Path.Combine(WcPath, name);
            using (StreamWriter writer = File.CreateText(path))
                writer.Write("Hello world");

            return path;
        }

        protected string GetTempFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            return sbox.GetTempFile();
        }

        /// <summary>
        /// Rename the administrative subdirectories if necessary.
        /// </summary>
        /// <param name="path"></param>
        protected void RenameAdminDirs(string path)
        {
            if (SvnClient.AdministrativeDirectoryName == ".svn")
                return;

            string adminDir = Path.Combine(path, ".svn");
            string newDir = Path.Combine(path, SvnClient.AdministrativeDirectoryName);

            if (Directory.Exists(adminDir))
            {
                Directory.Move(adminDir, newDir);
            }

            foreach (string dir in Directory.GetDirectories(path))
                this.RenameAdminDirs(dir);
        }

        protected static void TouchFile(string filename)
        {
            using (FileStream fs = File.Create(filename))
            {
                fs.Write(new byte[0], 0, 0);
            }
        }

        protected static void Touch2(string filename)
        {
            File.WriteAllText(filename, Guid.NewGuid().ToString());
        }

        public static void ForcedDeleteDirectory(string dir)
        {
            foreach (FileInfo fif in new DirectoryInfo(dir).GetFiles("*", SearchOption.AllDirectories))
            {
                if ((fif.Attributes & FileAttributes.ReadOnly) != 0)
                    fif.Attributes = FileAttributes.Normal;
            }
            Directory.Delete(dir, true);
        }

        static bool _checkedBDB;
        static void EnsureBDBAvailability(Microsoft.VisualStudio.TestTools.UnitTesting.TestContext value)
        {
            if (_checkedBDB)
                return;

            _checkedBDB = true;

            string dir = Path.GetDirectoryName(new Uri(typeof(TestBase).Assembly.CodeBase).LocalPath);

            string bdbX86 = "SharpSvn-DB44-20-win32.svnDll";
            string bdbX64 = "SharpSvn-DB44-20-x64.svnDll";

            if (!File.Exists(Path.Combine(dir, bdbX86)) && !File.Exists(Path.Combine(dir, bdbX64)))
            {
                string pd = dir;

                while (pd != null)
                {
                    DirectoryInfo imports = new DirectoryInfo(Path.Combine(pd, "imports/release"));

                    if (imports.Exists)
                    {
                        foreach (FileInfo f in imports.GetFiles("*.svnDll", SearchOption.AllDirectories))
                        {
                            File.Copy(f.FullName, Path.Combine(dir, f.Name));
                        }
                        return;
                    }

                    pd = Path.GetDirectoryName(pd);
                }

            }
        }

        Microsoft.VisualStudio.TestTools.UnitTesting.TestContext _tcx;
        public Microsoft.VisualStudio.TestTools.UnitTesting.TestContext TestContext
        {
            get { return _tcx; }
            set
            {
                _tcx = value;
                EnsureBDBAvailability(value);
            }
        }

        [Microsoft.VisualStudio.TestTools.UnitTesting.ClassCleanup]
        public void CleanTestData()
        {
            SvnSandBox.Clear();
        }
    }
}
