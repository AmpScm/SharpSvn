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
using System.Diagnostics;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::Update
    /// </summary>
    [TestFixture]
    public class UpdateTests : TestBase
    {
        private string _wc;

        [SetUp]
        public override void SetUp()
        {
            base.SetUp();

            _wc = GetTempDir();

            UnzipToFolder(Path.Combine(ProjectBase, "Zips/wc.zip"), _wc);
            this.RenameAdminDirs(_wc);

            RawRelocate(_wc, new Uri("file:///tmp/repos/"), ReposUrl);

            SvnClient cl = new SvnClient(); // Fix working copy to real location
            cl.Upgrade(_wc);
        }

        [Test]
        public void RevTests()
        {
            string dir = GetTempDir();

            SvnUpdateResult result;
            Assert.That(Client.CheckOut(new SvnUriTarget(new Uri(CollabReposUri, "trunk")), dir, out result));

            long head = result.Revision;

            Assert.That(Client.Update(dir, out result));
            Assert.That(result.Revision, Is.EqualTo(head));

            SvnUpdateArgs ua = new SvnUpdateArgs();

            ua.Revision = head - 5;

            Assert.That(Client.Update(dir, ua, out result));
            Assert.That(result.Revision, Is.EqualTo(head - 5));
        }

        /// <summary>
        /// Deletes a file, then calls update on the working copy to restore it
        /// from the text-base
        /// </summary>
        [Test]
        public void TestDeletedFile()
        {
            string filePath = Path.Combine(this.WcPath, "Form.cs");
            File.Delete(filePath);
            this.Client.Update(this.WcPath);

            Assert.That(File.Exists(filePath), "File not restored after update");
        }

        /// <summary>
        /// Changes a file in a secondary working copy and commits. Updates the
        /// primary wc and compares
        /// </summary>
        [Test]
        public void TestChangedFile()
        {
            using (StreamWriter w = new StreamWriter(Path.Combine(_wc, "Form.cs")))
                w.Write("Moo");
            this.RunCommand("svn", "ci -m \"\" " + _wc);

            this.Client.Update(this.WcPath);

            string s = File.ReadAllText(Path.Combine(this.WcPath, "Form.cs"));

            Assert.That(s, Is.EqualTo("Moo"), "File not updated");
        }

        [Test]
        public void TestObstruction()
        {
            string tmp = GetTempDir();
            Client.CheckOut(new SvnUriTarget(new Uri(CollabReposUri, "trunk/"), 2), tmp);
            File.WriteAllText(Path.Combine(tmp, "products/medium.html"), "q");
            SvnUpdateArgs ua = new SvnUpdateArgs();
            bool obstructionFound = false;
            ua.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                if (e.Action == SvnNotifyAction.TreeConflict)
                    obstructionFound = true;
            };
            ua.AddExpectedError(SvnErrorCode.SVN_ERR_WC_OBSTRUCTED_UPDATE);
            Assert.That(Client.Update(tmp, ua), "Update ok");

            Assert.That(obstructionFound, "Obstruction found");
        }

        [Test]
        public void TestNotify()
        {
            int n = 0; ;
            SvnUpdateArgs ua = new SvnUpdateArgs();
            ua.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                Assert.That(e.FullPath, Is.EqualTo(WcPath));
                switch (n++)
                {
                    case 0:
                        Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.UpdateStarted));
                        break;
                    case 1:
                        Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.UpdateCompleted));
                        break;
                }
            };

            Client.Update(WcPath, ua);
            Assert.That(n, Is.EqualTo(2));

            n = 0;

            Client.Update(new string[] { WcPath }, ua);
            Assert.That(n, Is.EqualTo(2));
        }

        [Test]
        public void TestUpdateMultipleFiles()
        {
            using (StreamWriter w = new StreamWriter(Path.Combine(_wc, "Form.cs")))
                w.Write("Moo");
            using (StreamWriter w = new StreamWriter(Path.Combine(_wc, "AssemblyInfo.cs")))
                w.Write("Moo");
            this.RunCommand("svn", "ci -m \"\" " + _wc);

            SvnUpdateArgs a = new SvnUpdateArgs();
            a.Depth = SvnDepth.Empty;

            SvnUpdateResult result;

            Assert.That(Client.Update(new string[]{
                                                             Path.Combine( this.WcPath, "Form.cs" ),
                                                             Path.Combine( this.WcPath, "AssemblyInfo.cs" )
                                                         }, a, out result));
            Assert.That(result.ResultMap.Count, Is.EqualTo(2));

            string s;
            using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "Form.cs")))
                s = r.ReadToEnd();
            Assert.That(s, Is.EqualTo("Moo"), "File not updated");

            using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "AssemblyInfo.cs")))
                s = r.ReadToEnd();
            Assert.That(s, Is.EqualTo("Moo"), "File not updated");
        }

        [Test]
        public void TestDirObstruction()
        {
            string dir = GetTempDir();
            string sd = Path.Combine(dir, "NewDir");
            SvnUpdateResult ur;
            Client.CheckOut(GetReposUri(TestReposType.Empty), dir, out ur);

            Client.CreateDirectory(sd);
            Client.Commit(dir);
            SvnUpdateArgs ua = new SvnUpdateArgs();
            ua.Revision = ur.Revision;
            Client.Update(dir, ua);
            Directory.CreateDirectory(sd);

            ua = new SvnUpdateArgs();
            bool gotIt = false;
            bool noConflict = true;
            ua.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                if (e.FullPath != sd)
                    return;

                gotIt = true;
                Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.TreeConflict));
                Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.Directory));
            };
            ua.ThrowOnError = false;
            ua.AddExpectedError(SvnErrorCode.SVN_ERR_WC_OBSTRUCTED_UPDATE);
            ua.Conflict += delegate(object sender, SvnConflictEventArgs e)
            {
                noConflict = false;
            };

            Assert.That(Client.Update(dir, ua), "Update failed");
            Assert.That(gotIt, "Got obstruction notification");
            Assert.That(noConflict, "No conflict event");

            Client.Status(sd,
                delegate(object sender, SvnStatusEventArgs sa)
                {
                    Assert.That(sa.Conflicted, Is.True, "Has tree conflict");
                    Assert.That(sa.LocalNodeStatus, Is.EqualTo(SvnStatus.Deleted));
                    Assert.That(sa.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                });
        }

        [Test]
        public void TestUpdateExternal()
        {
            Uri root = GetReposUri(TestReposType.Empty);
            Uri trunk = new Uri(root, "trunk/");
            Uri alt = new Uri(root, "alt/");
            Client.RemoteCreateDirectory(trunk);
            Client.RemoteCreateDirectory(alt);

            string dir = GetTempDir();
            Client.CheckOut(trunk, dir);
            Client.SetProperty(dir, SvnPropertyNames.SvnExternals,
                                string.Format(  "alt1 {0}\r\n" +
                                "alt2 {0}\n" +
                                "alt3 {0}", alt));

            SvnUpdateArgs ua = new SvnUpdateArgs();

            SortedList<string, string> paths = new SortedList<string, string>(StringComparer.OrdinalIgnoreCase);
            ua.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                if (e.Action != SvnNotifyAction.UpdateExternal)
                    return;

                paths.Add(e.FullPath, e.FullPath);
            };
            Client.Update(dir, ua);

            Assert.That(paths.Count, Is.EqualTo(3));

            Assert.That(paths.ContainsKey(Path.Combine(dir,"alt2")));
        }

        [Test, ExpectedException(typeof(SvnSystemException), ExpectedMessage = "Can't move", MatchType = MessageMatch.Contains)]
        public void UpdateInUse()
        {
            string dir = GetTempDir();
            Client.CheckOut(new SvnUriTarget(new Uri(CollabReposUri, "trunk"), 1), dir);

            using (File.OpenRead(Path.Combine(dir, "index.html")))
            using (new Implementation.SvnFsOperationRetryOverride(0))
            {
                Client.Update(Path.Combine(dir, "index.html"));
            }
        }

        [Test]
        public void UpdateInUseWrite()
        {
            string dir = GetTempDir();
            bool skippedDenied = false;
            Client.CheckOut(new SvnUriTarget(new Uri(CollabReposUri, "trunk"), 1), dir);
            Client.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                if (e.Action == SvnNotifyAction.UpdateSkipAccessDenied)
                    skippedDenied = true;
            };

            using (File.Create(Path.Combine(dir, "index.html")))
            using (new Implementation.SvnFsOperationRetryOverride(0))
            {
                Client.Update(Path.Combine(dir, "index.html"));
            }

            Assert.That(skippedDenied);
        }

        [Test]
        public void StatusReportsSparse()
        {
            string dir = GetTempDir();
            Client.CheckOut(new Uri(CollabReposUri, "trunk"), dir);

            Client.Update(Path.Combine(dir, "products"), new SvnUpdateArgs { Depth = SvnDepth.Empty, KeepDepth = true });
            Client.Update(Path.Combine(dir, "about"), new SvnUpdateArgs { Depth = SvnDepth.Exclude, KeepDepth = true });
            Client.Update(Path.Combine(dir, "jobs\\index.html"), new SvnUpdateArgs { Depth = SvnDepth.Exclude, KeepDepth = true });

            Collection<SvnStatusEventArgs> list;
            
            Client.GetStatus(dir, out list);
            Assert.That(list.Count, Is.EqualTo(0));

            Client.GetStatus(dir, new SvnStatusArgs { RetrieveRemoteStatus=true}, out list);
            Assert.That(list.Count, Is.EqualTo(0));

            Client.GetStatus(dir, new SvnStatusArgs { RetrieveRemoteStatus = true, KeepDepth=true }, out list);
            Assert.That(list.Count, Is.EqualTo(11));

            int nAdded = 0;
            foreach (SvnStatusEventArgs ee in list)
            {
                if (ee.RemoteNodeStatus == SvnStatus.Added)
                    nAdded++;
            }

            Assert.That(nAdded, Is.EqualTo(8));
        }
    }
}
