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
using System.IO;
using System.Collections.ObjectModel;

using NUnit.Framework;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using System.Collections.Generic;
using SharpSvn.Tests.LookCommands;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests NSvn::Core::Client::Add
    /// </summary>
    [TestClass]
    public class AddTests : TestBase
    {
        /// <summary>
        /// Attempts to add a file, checking that the file actually was added
        /// </summary>
        [TestMethod]
        public void Add_BasicAdd()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            string dir = sbox.Wc;
            string testFile = Path.Combine(dir, "testfile.txt");

            File.WriteAllText(testFile, "This is a testfile");

            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Empty;

            Client.Add(testFile, a);

            // Check if the file is part of a working copy
            Guid g;
            Assert.That(Client.TryGetRepositoryId(testFile, out g));
            Assert.That(g, Is.Not.EqualTo(Guid.Empty));

            Collection<SvnStatusEventArgs> st;
            Client.GetStatus(testFile, out st);

            Assert.That(st.Count, Is.EqualTo(1));
            Assert.That(st[0].LocalNodeStatus, Is.EqualTo(SvnStatus.Added));

            Client.Configuration.LogMessageRequired = false;
            Assert.That(Client.Commit(dir));
        }

        /// <summary>
        /// Creates a subdirectory with items in it, tries to add it non-recursively.
        /// Checks that none of the subitems are added
        /// </summary>
        [TestMethod]
        public void Add_AddDirectoryNonRecursively()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);

            string dir1, dir2, testFile1, testFile2;
            CreateSubdirectories(sbox.Wc, out dir1, out dir2, out testFile1, out testFile2);

            this.Client.Notify += new EventHandler<SvnNotifyEventArgs>(this.NotifyCallback);
            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Empty;
            // do a non-recursive add here
            this.Client.Add(dir1, a);

            Assert.That(this.Notifications.Length == 1, "Too many or no notifications received. Added recursively?");
            Assert.That(this.GetSvnStatus(dir1), Is.EqualTo(SvnStatus.Added), "Subdirectory not added");

            Assert.That(GetSvnStatus(dir2), Is.Not.EqualTo(SvnStatus.Added), "Recursive add");
            Assert.That(GetSvnStatus(testFile1), Is.Not.EqualTo(SvnStatus.Added), "Recursive add");
            Assert.That(GetSvnStatus(testFile2), Is.Not.EqualTo(SvnStatus.Added), "Recursive add");
        }

        [TestMethod]
        public void TestAddWithParents()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string dir = sbox.Wc;

            string file = Path.Combine(dir, "a/b/d/e/f");

            Directory.CreateDirectory(Path.Combine(dir, "a/b/d/e"));
            TouchFile(file);

            SvnAddArgs aa = new SvnAddArgs();
            aa.ThrowOnError = false;
            //aa.AddParents = true;
            Assert.That(Client.Add(file, aa), Is.False);

            aa.ThrowOnError = true;
            aa.AddParents = true;

            Assert.That(Client.Add(file, aa));

            Client.Info(file, delegate(object sender, SvnInfoEventArgs e)
            {
                // This check verifies Subversion 1.5 behavior, but will probably
                // give the real guid in 1.6+
                Assert.That(e.RepositoryId, Is.Not.EqualTo(Guid.Empty));
            });

            Guid gg;
            Assert.That(Client.TryGetRepositoryId(file, out gg));

            Assert.That(gg, Is.Not.EqualTo(Guid.Empty));

        }

        [TestMethod]
        public void TestAddStupid()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);

            string wc = sbox.Wc;

            string dir = Path.Combine(wc, "1");
            Directory.CreateDirectory(dir);
            Client.Add(dir + "\\.");

            dir = Path.Combine(wc, "2");
            Directory.CreateDirectory(dir);
            Client.Add(dir + "\\");

            dir = Path.Combine(wc, "3");
            Directory.CreateDirectory(dir);
            Client.Add(dir + "\\\\");
        }

        /// <summary>
        /// Creates a subdirectory with some items in it. Attempts to add it recursively.
        /// </summary>
        [TestMethod]
        public void Add_AddDirectoryRecursively()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);
            string WcPath = sbox.Wc;

            string dir1, dir2, testFile1, testFile2;
            CreateSubdirectories(WcPath, out dir1, out dir2, out testFile1, out testFile2);

            this.Client.Notify += new EventHandler<SvnNotifyEventArgs>(this.NotifyCallback);
            // now a recursive add
            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Infinity;
            this.Client.Add(dir1, a);

            // enough notifications?
            Assert.That(this.Notifications.Length, Is.EqualTo(4), "Received wrong number of notifications");
            Assert.That(this.GetSvnStatus(dir1), Is.EqualTo(SvnStatus.Added), "Subdirectory not added");
            Assert.That(this.GetSvnStatus(dir2), Is.EqualTo(SvnStatus.Added), "Subsubdirectory not added");
            Assert.That(this.GetSvnStatus(testFile1), Is.EqualTo(SvnStatus.Added), "File in subdirectory not added");
            Assert.That(this.GetSvnStatus(testFile2), Is.EqualTo(SvnStatus.Added), "File in subsubdirectory not added");
        }

        List<SvnNotifyEventArgs> _notifications = new List<SvnNotifyEventArgs>();

        /// <summary>
        /// Callback method to be used as ClientContext.NotifyCallback
        /// </summary>
        /// <param name="notification">An object containing information about the notification</param>
        public virtual void NotifyCallback(object sender, SvnNotifyEventArgs e)
        {
            e.Detach();
            _notifications.Add(e);
        }

        /// <summary>
        /// The notifications generated during a call to Client::Add
        /// </summary>
        public SvnNotifyEventArgs[] Notifications
        {
            get
            {
                return _notifications.ToArray();
            }
        }

        [TestMethod]
        public void Add_AddWithForce()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);
            string WcPath = sbox.Wc;

            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Empty;

            string file = Path.Combine(WcPath, "AssemblyInfo.cs");
            TouchFile(file);
            this.Client.Add(file);
            try
            {
                this.Client.Add(file, a);
                Assert.Fail("Should have failed");
            }
            catch (SvnException)
            {
                // swallow
            }

            // should not fail
            a.Force = true;
            this.Client.Add(file, a);
        }

        [TestMethod]
        public void Add_AddFileWithNonAsciiFilename()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);
            string WcPath = sbox.Wc;
            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Infinity;

            string newFile = Path.Combine(WcPath, "eia.");
            File.Create(newFile).Close();
            this.Client.Add(newFile, a);
        }

        [TestMethod]
        public void Add_MultiLevelAdd()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);

            string subdir = Path.Combine(sbox.Wc, "trunk/a/b/c");
            Directory.CreateDirectory(Path.Combine(sbox.Wc, "trunk/a/b/c"));

            SvnAddArgs aa = new SvnAddArgs();
            aa.Force = true; // Continue
            aa.AddParents = true;

            Client.Add(subdir, aa);

            Client.Commit(sbox.Wc);
        }

        string _wcPath;
        Uri _reposUrl;

        /// <summary>
        /// The path to the working copy
        /// </summary>
        [Obsolete("Please use an SandBox")]
        string WcPath
        {
            get
            {
                if (_wcPath == null)
                {
                    ExtractWorkingCopy();
                }

                return _wcPath;
            }
        }

        Uri _reposUri;
        string _reposPath;
        /// <summary>
        /// The fully qualified URI to the repository
        /// </summary>
        [Obsolete]
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

        [Obsolete]
        void ExtractWorkingCopy()
        {
            if (ReposUrl == null)
                ExtractRepos();

            System.Diagnostics.Debug.Assert(Directory.Exists(ReposPath));

            this._wcPath = new SvnSandBox(this).GetTempDir();

            UnzipToFolder(Path.Combine(ProjectBase, "Zips/wc.zip"), _wcPath);
            RawRelocate(_wcPath, new Uri("file:///tmp/repos/"), ReposUrl);
            this.RenameAdminDirs(_wcPath);

            SvnClient cl = new SvnClient(); // Fix working copy to real location
            cl.Upgrade(_wcPath);
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

        /// <summary>
        /// extract our test repository
        /// </summary>
        void ExtractRepos()
        {
            if (_reposPath == null)
                _reposPath = new SvnSandBox(this).GetTempDir();

            UnzipToFolder(Path.Combine(ProjectBase, "Zips\\repos.zip"), _reposPath);

            _reposUri = SvnTools.LocalPathToUri(_reposPath, true);
        }

        [TestMethod]
        public void Add_AddAndDelete()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            Uri uri = sbox.RepositoryUri;
            string dir = sbox.Wc;

            string file = Path.Combine(dir, "test.txt");
            TouchFile(file);
            Client.Add(file);

            SvnDeleteArgs da = new SvnDeleteArgs();
            da.Force = true;

            Client.Delete(file, da);
            Assert.That(!File.Exists(file));

            file += ".2";

            TouchFile(file);
            Client.Add(file);

            Client.Commit(WcPath);

            File.Delete(file);
            Assert.That(!File.Exists(file));

            da.ThrowOnError = false; // This throws an error in 1.5 but succeeds anyway
            Assert.That(Client.Delete(file, da), "This should succeed in 1.7+");

            file += ".3";

            TouchFile(file);
            Client.Add(file);

            File.Delete(file);
            Assert.That(!File.Exists(file));

            da.ThrowOnError = true; // This works in 1.5
            da.KeepLocal = true;
            Client.Delete(file, da);

            Client.Status(dir,
            delegate(object sender, SvnStatusEventArgs e)
            {
                Assert.Fail("Nothing modified");
            });
        }

        static void CreateSubdirectories(string within, out string dir1, out string dir2, out string testFile1, out string testFile2)
        {
            dir1 = Path.Combine(within, "subdir");
            Directory.CreateDirectory(dir1);

            dir2 = Path.Combine(dir1, "subsubdir");
            Directory.CreateDirectory(dir2);

            testFile1 = CreateTextFile(within, @"subdir\testfile.txt");
            testFile2 = CreateTextFile(within, @"subdir\subsubdir\testfile2.txt");
        }

    }
}
