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

// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests NSvn::Core::Client::Add
    /// </summary>
    [TestFixture]
    public class AddTests : TestBase
    {
        /// <summary>
        /// Attempts to add a file, checking that the file actually was added
        /// </summary>
        [Test]
        public void TestBasic()
        {
            string dir = GetTempDir();
            string testFile = Path.Combine(dir, "testfile.txt");

            Client.CheckOut(ReposUrl, dir);

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
            Assert.That(st[0].LocalContentStatus, Is.EqualTo(SvnStatus.Added));

            // Verify status with external command
            Assert.That(this.GetSvnStatus(testFile), Is.EqualTo('A'), "svn st does not report the file as added");

            Client.Configuration.LogMessageRequired = false;
            Assert.That(Client.Commit(dir));
        }

        /// <summary>
        /// Creates a subdirectory with items in it, tries to add it non-recursively.
        /// Checks that none of the subitems are added
        /// </summary>
        [Test]
        public void TestAddDirectoryNonRecursively()
        {
            string dir1, dir2, testFile1, testFile2;
            this.CreateSubdirectories(out dir1, out dir2, out testFile1, out testFile2);

            this.Client.Notify += new EventHandler<SvnNotifyEventArgs>(this.NotifyCallback);
            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Empty;
            // do a non-recursive add here
            this.Client.Add(dir1, a);

            Assert.That(this.Notifications.Length == 1, "Too many or no notifications received. Added recursively?");
            Assert.That(this.GetSvnStatus(dir1), Is.EqualTo('A'), "Subdirectory not added");

            Assert.That(GetSvnStatus(dir2), Is.Not.EqualTo('A'), "Recursive add");
            Assert.That(GetSvnStatus(testFile1), Is.Not.EqualTo('A'), "Recursive add");
            Assert.That(GetSvnStatus(testFile2), Is.Not.EqualTo('A'), "Recursive add");
        }

        [Test]
        public void TestAddWithParents()
        {
            string dir = WcPath;

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

		[Test]
		public void TestAddStupid()
		{
			string wc = GetTempDir();
			Client.CheckOut(ReposUrl, wc);

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
        [Test]
        public void TestAddDirectoryRecursively()
        {
            string dir1, dir2, testFile1, testFile2;
            this.CreateSubdirectories(out dir1, out dir2, out testFile1, out testFile2);

            this.Client.Notify += new EventHandler<SvnNotifyEventArgs>(this.NotifyCallback);
            // now a recursive add
            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Infinity;
            this.Client.Add(dir1, a);

            // enough notifications?
            Assert.That(this.Notifications.Length, Is.EqualTo(4), "Received wrong number of notifications");
            Assert.That(this.GetSvnStatus(dir1), Is.EqualTo('A'), "Subdirectory not added");
            Assert.That(this.GetSvnStatus(dir2), Is.EqualTo('A'), "Subsubdirectory not added");
            Assert.That(this.GetSvnStatus(testFile1), Is.EqualTo('A'), "File in subdirectory not added");
            Assert.That(this.GetSvnStatus(testFile2), Is.EqualTo('A'), "File in subsubdirectory not added");
        }

        [Test]
        public void TestWithForce()
        {
            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Empty;

            string file = Path.Combine(this.WcPath, "AssemblyInfo.cs");
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

        [Test]
        public void TestAddFileWithNonAsciiFilename()
        {
            SvnAddArgs a = new SvnAddArgs();
            a.Depth = SvnDepth.Infinity;

            string newFile = Path.Combine(this.WcPath, "eia.");
            File.Create(newFile).Close();
            this.Client.Add(newFile, a);
        }

        [Test]
        public void TestMultiLevel()
        {
            string tmp = GetTempDir();
            SvnCheckOutArgs ca = new SvnCheckOutArgs();
            ca.Depth = SvnDepth.Empty;
            Client.CheckOut(CollabReposUri, tmp, ca); // Check out the root depth empty
            Client.Update(Path.Combine(tmp, "trunk")); // Make sure everything under trunk is available

            string subdir = Path.Combine(tmp, "trunk/a/b/c");
            Directory.CreateDirectory(Path.Combine(tmp, "trunk/a/b/c"));

            SvnAddArgs aa = new SvnAddArgs();
            aa.Force = true; // Continue
            aa.AddParents = true;

            Client.Add(subdir, aa);

            Client.Commit(tmp);
        }

        [Test]
        public void TestAddAndDelete()
        {
            string dir = GetTempDir();
            Uri uri = new Uri(CollabReposUri, "trunk/");

            Client.CheckOut(uri, dir);

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
            bool ok = Client.Delete(file, da);

            Assert.That(ok, "This should succeed in 1.7+");


            file += ".3";

            TouchFile(file);
            Client.Add(file);

            Client.Commit(WcPath);

            File.Delete(file);
            Assert.That(!File.Exists(file));

            da.ThrowOnError = true; // This works in 1.5
            da.KeepLocal = true;
            Client.Delete(file, da);

            Client.Status(dir,
                delegate(object sender, SvnStatusEventArgs e)
                {
                    Assert.That(false, "Nothing modified");
                });
        }

        private void CreateSubdirectories(out string dir1, out string dir2, out string testFile1, out string testFile2)
        {
            dir1 = Path.Combine(this.WcPath, "subdir");
            Directory.CreateDirectory(dir1);

            dir2 = Path.Combine(dir1, "subsubdir");
            Directory.CreateDirectory(dir2);

            testFile1 = this.CreateTextFile(@"subdir\testfile.txt");
            testFile2 = this.CreateTextFile(@"subdir\subsubdir\testfile2.txt");
        }

    }
}
