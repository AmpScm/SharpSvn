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
using Microsoft.VisualStudio.TestTools.UnitTesting;

using ICSharpCode.SharpZipLib.Zip;
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
        CollabRepos
    }
    /// <summary>
    /// Serves as a base class for tests for NSvn::Core::Add
    /// </summary>
    public class TestBase : IHasTestContext
    {
        SortedList<TestReposType, Uri> _reposs = new SortedList<TestReposType, Uri>();
        Uri _reposUri;
        string _reposPath;
        string _wcPath;

        public TestBase()
        {
            string asm = this.GetType().FullName;
            this.REPOS_FILE = "repos.zip";
            this.WC_FILE = "wc.zip";
            UseEmptyRepositoryForWc = true;
        }

        protected string GetTempDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            return sbox.GetTempDir();
        }

        protected Uri CreateRepos(TestReposType type)
        {
            switch (type)
            {
                case TestReposType.Empty:
                    SvnSandBox sbox = new SvnSandBox(this);
                    return sbox.CreateRepository(SandBoxRepository.Empty);
                case TestReposType.EmptyNoMerge:
                    using (SvnRepositoryClient rc = new SvnRepositoryClient())
                    {
                        string path = GetTempDir();

                        SvnCreateRepositoryArgs rca = new SvnCreateRepositoryArgs();
                        rca.RepositoryCompatibility = SvnRepositoryCompatibility.Subversion14;
                        rc.CreateRepository(path, rca);

                        return SvnTools.LocalPathToUri(path, true);
                    }
                case TestReposType.CollabRepos:
                    sbox = new SvnSandBox(this);
                    return sbox.CreateRepository(SandBoxRepository.MergeScenario);
                case TestReposType.AnkhRepos:
                    sbox = new SvnSandBox(this);
                    return sbox.CreateRepository(SandBoxRepository.AnkhSvnCases);
                default:
                    throw new ArgumentException();
            }
        }

        public Uri GetReposUri(TestReposType type)
        {
            Uri reposUri;
            if (!_reposs.TryGetValue(type, out reposUri))
            {
                reposUri = CreateRepos(type);
                _reposs[type] = reposUri;
            }
            return reposUri;
        }

        public string GetRepos(TestReposType type)
        {
            return GetReposUri(type).AbsolutePath;
        }

        [Obsolete("Please use an SBox")]
        protected Uri CollabReposUri
        {
            get { return GetReposUri(TestReposType.CollabRepos); }
        }

        [TestInitialize]
        public void SetUp()
        {
            this.notifications = new List<SvnNotifyEventArgs>();
            this.client = new SvnClient();
            client.Configuration.LogMessageRequired = false;
            client.Notify += new EventHandler<SvnNotifyEventArgs>(OnClientNotify);

            _reposPath = null;
            _reposUri = null;
            _wcPath = null;
            _reposs.Clear();
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

        protected static void RecursiveDelete(string path)
        {
            if (Directory.Exists(path))
            {
                foreach (FileInfo f in new DirectoryInfo(path).GetFiles("*", SearchOption.AllDirectories))
                {
                    f.Attributes = FileAttributes.Normal;
                }

                foreach (DirectoryInfo f in new DirectoryInfo(path).GetDirectories("*", SearchOption.AllDirectories))
                {
                    f.Attributes = FileAttributes.Normal;
                }

                Directory.Delete(path, true);
            }
        }

        /// <summary>
        /// extract our test repository
        /// </summary>
        void ExtractRepos()
        {
            if (_reposPath == null)
                _reposPath = GetTempDir();

            UnzipToFolder(Path.Combine(ProjectBase, "Zips\\repos.zip"), _reposPath);

            _reposUri = SvnTools.LocalPathToUri(_reposPath, true);
        }

        void ExtractWorkingCopy()
        {
            if (ReposUrl == null)
                ExtractRepos();

            System.Diagnostics.Debug.Assert(Directory.Exists(ReposPath));

            this._wcPath = GetTempDir();

            UnzipToFolder(Path.Combine(ProjectBase, "Zips/" + WC_FILE), _wcPath);
            RawRelocate(_wcPath, new Uri("file:///tmp/repos/"), ReposUrl);
            this.RenameAdminDirs(_wcPath);

            SvnClient cl = new SvnClient(); // Fix working copy to real location
            cl.Upgrade(_wcPath);
        }

        protected static void RawRelocate(string wc, Uri from, Uri to)
        {
            foreach (DirectoryInfo adm_dir in new DirectoryInfo(wc).GetDirectories(".svn", SearchOption.AllDirectories))
            {
                string entries = Path.Combine(adm_dir.FullName, "entries");
                string txt = File.ReadAllText(entries).Replace(from.AbsoluteUri.TrimEnd('/'), to.AbsoluteUri.TrimEnd('/'));

                File.SetAttributes(entries, FileAttributes.Normal);
                File.WriteAllText(entries, txt);
                File.SetAttributes(entries, FileAttributes.ReadOnly);
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

        protected static void UnzipToFolder(string zipFile, string toFolder)
        {
            Assert.That(File.Exists(zipFile));
            FastZip fz = new FastZip();
            fz.CreateEmptyDirectories = true;
            fz.RestoreAttributesOnExtract = true;
            fz.RestoreDateTimeOnExtract = true;

            fz.ExtractZip(zipFile, toFolder, null);
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
        /// The fully qualified URI to the repository
        /// </summary>
        public Uri ReposUrl
        {
            get
            {
                if (_reposUri == null)
                    ExtractRepos();

                System.Diagnostics.Debug.Assert(Directory.Exists(_reposPath));

                return _reposUri;
            }
        }

        /// <summary>
        /// The path to the repository
        /// </summary>
        public string ReposPath
        {
            get
            {
                if (_reposPath == null)
                    ExtractRepos();

                System.Diagnostics.Debug.Assert(Directory.Exists(_reposPath));

                return _reposPath;
            }
        }

        public bool UseEmptyRepositoryForWc { get; set; }

        /// <summary>
        /// The path to the working copy
        /// </summary>
        [Obsolete("Please use an SandBox")]
        public string WcPath
        {
            get
            {
                if (_wcPath == null)
                {
                    if (UseEmptyRepositoryForWc)
                    {
                        _wcPath = GetTempDir();
                        Client.CheckOut(GetReposUri(TestReposType.Empty), _wcPath);
                    }
                    else
                        ExtractWorkingCopy();
                }

                return _wcPath;
            }
        }

        /// <summary>
        /// The notifications generated during a call to Client::Add
        /// </summary>
        public SvnNotifyEventArgs[] Notifications
        {
            get
            {
                return this.notifications.ToArray();
            }
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
        /// Callback method to be used as ClientContext.NotifyCallback
        /// </summary>
        /// <param name="notification">An object containing information about the notification</param>
        public virtual void NotifyCallback(object sender, SvnNotifyEventArgs e)
        {
            e.Detach();
            this.notifications.Add(e);
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

        protected string Configuration
        {
            get
            {
#if DEBUG
                return "DEBUG";
#else
                return "RELEASE";
#endif
            }
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
            if (TRAD_WC_ADMIN_DIR == SvnClient.AdministrativeDirectoryName)
                return;

            string adminDir = Path.Combine(path, TRAD_WC_ADMIN_DIR);
            string newDir = Path.Combine(path, SvnClient.AdministrativeDirectoryName);

            if (Directory.Exists(adminDir))
            {
                Directory.Move(adminDir, newDir);
            }

            foreach (string dir in Directory.GetDirectories(path))
                this.RenameAdminDirs(dir);
        }

        /// <summary>
        /// Starts a svnserve instance.
        /// </summary>
        /// <param name="root">The root directory to use for svnserve.</param>
        /// <returns></returns>
        protected Process StartSvnServe(string root)
        {
            ProcessStartInfo psi = new ProcessStartInfo(Path.GetFullPath(Path.Combine(ProjectBase, "..\\..\\imports\\release\\bin\\svnserve.exe")),
                String.Format("--daemon --root {0} --listen-host 127.0.0.1 --listen-port {1}", root,
                PortNumber));

            psi.CreateNoWindow = true;
            psi.UseShellExecute = false;

            return Process.Start(psi);
        }

        protected void SetReposAuth()
        {
            string conf = Path.Combine(this.ReposPath,
                Path.Combine("conf", "svnserve.conf"));
            string authConf = Path.Combine(this.ReposPath,
                Path.Combine("conf", "svnserve.auth.conf"));
            File.Copy(authConf, conf, true);
        }

        protected static readonly int PortNumber = 7777 + new Random().Next(5000);



        protected static void TouchFile(string filename)
        {
            TouchFile(filename, false);
        }

        protected static void Touch2(string filename)
        {
            File.WriteAllText(filename, Guid.NewGuid().ToString());
        }

        protected static void TouchFile(string filename, bool createDir)
        {
            string dir = Path.GetDirectoryName(filename);
            if (createDir && !Directory.Exists(dir))
                Directory.CreateDirectory(dir);

            using (FileStream fs = File.Create(filename))
            {
                fs.Write(new byte[0], 0, 0);
            }
        }

        protected void InstallRevpropHook(string reposPath)
        {
            string bat = Path.ChangeExtension(SvnHookArguments.GetHookFileName(reposPath, SvnHookType.PreRevPropChange), ".bat");

            File.WriteAllText(bat, "exit 0");
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


        protected readonly string REPOS_FILE;
        private const string REPOS_NAME = "repos";
        protected readonly string WC_FILE;
        protected const string WC_NAME = "wc";
        protected const string TRAD_WC_ADMIN_DIR = ".svn";
        private SvnClient client;

        protected List<SvnNotifyEventArgs> notifications;

        Microsoft.VisualStudio.TestTools.UnitTesting.TestContext _tcx;
        public Microsoft.VisualStudio.TestTools.UnitTesting.TestContext TestContext
        {
            get { return _tcx;  }
            set { _tcx = value; }
        }

        [Microsoft.VisualStudio.TestTools.UnitTesting.ClassCleanup]
        public void CleanTestData()
        {
            SvnSandBox.Clear();
        }
    }
}
