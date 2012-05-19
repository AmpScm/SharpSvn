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
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using SharpSvn;
using SharpSvn.Security;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests the Client.Commit method.
    /// </summary>
    [TestFixture]
    public class CommitTests : TestBase
    {
        /// <summary>
        /// Modifies a file in the working copy and commits the containing directory.
        /// </summary>
        [Test]
        public void TestBasicCommit()
        {
            string filepath = Path.Combine(this.WcPath, "Form.cs");
            using (StreamWriter w = new StreamWriter(filepath))
                w.Write("Moo");

            SvnCommitResult info;



            Assert.That(this.Client.Commit(this.WcPath, out info));

            Assert.That(info, Is.Not.Null);

            char status = this.GetSvnStatus(filepath);
            Assert.That(status, Is.EqualTo((char)0), "File not committed");
        }

        /// <summary>
        /// Locks and modifies a file in the working copy and commits the containing directory keeping, and not keeping locks
        /// </summary>
        [Test]
        public void TestCommitWithLocks()
        {
            string filepath = Path.Combine(this.WcPath, "Form.cs");
            using (StreamWriter w = new StreamWriter(filepath))
                w.Write("Moo");

            this.RunCommand("svn", "lock " + filepath);

            SvnCommitResult info;

            SvnCommitArgs a = new SvnCommitArgs();
            a.KeepLocks = true;

            Assert.That(this.Client.Commit(this.WcPath, a, out info));
            Assert.IsNotNull(info);

            char locked = this.RunCommand("svn", "status " + filepath)[5];
            Assert.That(locked == 'K', "File was unlocked while lock should be kept");

            using (StreamWriter w = new StreamWriter(filepath))
                w.Write("Bah");

            a.KeepLocks = false;
            Assert.That(Client.Commit(WcPath, a, out info));
            Assert.IsNotNull(info);

            string output = this.RunCommand("svn", "status " + filepath);
            Assert.That(output, Is.EqualTo(String.Empty));
        }

        [Test]
        public void TestSetCustomProps()
        {
            string fp = Path.Combine(WcPath, "NewFile.cs");
            Touch2(fp);
            Client.Add(fp);

            SvnCommitArgs ca = new SvnCommitArgs();
            ca.LogMessage = "Committed extra";
            ca.LogProperties.Add("my:prop", "PropValue");
            ca.RunTortoiseHooks = true;

            SvnCommitResult cr;
            Client.Commit(WcPath, ca, out cr);

            string value;
            Client.GetRevisionProperty(ReposUrl, cr.Revision, "my:prop", out value);

            Assert.That(value, Is.EqualTo("PropValue"));


            SvnLogArgs la = new SvnLogArgs();
            la.RetrieveProperties.Add("my:prop");
            la.Start = la.End = cr.Revision;

            Collection<SvnLogEventArgs> lc;
            Client.GetLog(WcPath, la, out lc);

            Assert.That(lc.Count, Is.EqualTo(1));
            Assert.That(lc[0].RevisionProperties.Contains("my:prop"));
            SvnLogEventArgs l = lc[0];
            Assert.That(l.RevisionProperties["my:prop"].StringValue, Is.EqualTo("PropValue"));
            Assert.That(l.Author, Is.EqualTo(Environment.UserName));
        }

        /// <summary>
        /// Commits a single file
        /// </summary>
        [Test]
        public void TestCommitFile()
        {
            string filepath = Path.Combine(this.WcPath, "Form.cs");
            using (StreamWriter w = new StreamWriter(filepath))
                w.Write("Moo");

            SvnCommitArgs a = new SvnCommitArgs();
            a.Depth = SvnDepth.Empty;

            Assert.That(this.Client.Commit(filepath, a));

            char status = this.GetSvnStatus(filepath);
            Assert.That(status, Is.EqualTo((char)0), "File not committed");

        }

        [Test]
        public void TestCommitWithNonAnsiCharsInLogMessage()
        {
            string filepath = Path.Combine(this.WcPath, "Form.cs");
            using (StreamWriter w = new StreamWriter(filepath))
                w.Write("Moo");

            SvnCommitArgs a = new SvnCommitArgs();
            a.LogMessage = " ¥ · £ · € · $ · ¢ · ₡ · ₢ · ₣ · ₤ · ₥ · ₦ · ₧ · ₨ · ₩ · ₪ · ₫ · ₭ · ₮ · ₯";

            Assert.That(Client.Commit(this.WcPath, a));

            SvnLogArgs la = new SvnLogArgs();
            la.Start = SvnRevision.Head;
            la.End = SvnRevision.Head;

            Collection<SvnLogEventArgs> logList;

            Client.GetLog(WcPath, la, out logList);

            Assert.That(logList, Is.Not.Null);
            Assert.That(logList.Count, Is.EqualTo(1));
            Assert.That(logList[0].LogMessage, Is.EqualTo(a.LogMessage));
        }

        private ArrayList logMessages = new ArrayList();
        private void GetLogMessage(object sender, SvnCommittingEventArgs e)
        {
            logMessages.Add(e.LogMessage);
        }

        /// <summary>
        /// Tests that a commit on an unmodified repos returns CommitInfo.Invalid.
        /// </summary>
        [Test]
        public void TestCommitWithNoModifications()
        {
            string filepath = Path.Combine(this.WcPath, "Form.cs");
            this.Client.Committing += delegate(object sender, SvnCommittingEventArgs e)
            {
                Assert.That(e.Items.Count, Is.EqualTo(1), "Wrong number of commit items");
                Assert.That(e.Items[0].Path, Is.EqualTo(filepath), "Wrong path");
                Assert.That(e.Items[0].NodeKind, Is.EqualTo(SvnNodeKind.File), "Wrong kind");
                Assert.That(e.Items[0].Revision, Is.EqualTo(6), "Wrong revision");
            };
            SvnCommitResult ci;

            Assert.That(this.Client.Commit(WcPath, out ci));
            Assert.That(ci, Is.Null);
        }

        /// <summary>
        /// Tests that you can cancel a commit.
        /// </summary>
        [Test]
        public void TestCancelledCommit()
        {
            string path = Path.Combine(this.WcPath, "Form.cs");
            string path2 = Path.Combine(this.WcPath, "Form2.cs");
            File.WriteAllText(path, "MOO");
            File.WriteAllText(path2, "MOO2");
            Client.Add(path2);

            this.Client.Committing += delegate(object sender, SvnCommittingEventArgs e)
            {
                e.LogMessage = null;
                e.Cancel = true;
            };

            SvnCommitResult info;
            SvnCommitArgs a = new SvnCommitArgs();
            a.ThrowOnCancel = false;
            a.RunTortoiseHooks = true;

            Assert.That(this.Client.Commit(new string[] { path, path2 }, a, out info), Is.False);

            Assert.That(info, Is.Null, "info should be Invalid for a cancelled commit");

            Collection<SvnStatusEventArgs> statee;
            Client.GetStatus(path, out statee);
            Assert.That(statee.Count, Is.EqualTo(1));
            Assert.That(statee[0].LocalNodeStatus, Is.EqualTo(SvnStatus.Modified), "File committed even for a cancelled log message");
        }

        void VerifyNotify(object sender, SvnNotifyEventArgs e)
        {
            Assert.That(File.Exists(e.FullPath), "{0} does exist; path was defined as {1}", e.FullPath, e.Path);
        }

        [Test]
        public void NonRecursiveDirDelete()
        {
            string dir = GetTempDir();
            Client.CheckOut(GetReposUri(TestReposType.Empty), dir);

            string name = Path.Combine(dir, "sd");

            Client.CreateDirectory(name);
            Client.Commit(name);

            Client.Delete(name);
            SvnCommitArgs ca = new SvnCommitArgs();
            ca.Depth = SvnDepth.Empty;
            Client.Commit(name, ca);
        }

        [Test]
        public void WithAlternateUser()
        {
            string user = Guid.NewGuid().ToString();

            string dir = GetTempDir();
            using (SvnClient client = new SvnClient())
            {
                client.CheckOut(GetReposUri(TestReposType.Empty), dir);

                client.Authentication.Clear();
                client.Configuration.LogMessageRequired = false;

                client.Authentication.UserNameHandlers +=
                    delegate(object sender, SvnUserNameEventArgs e)
                    {
                        e.UserName = user;
                    };

                client.SetProperty(dir, "a", "b");

                SvnCommitResult cr;
                client.Commit(dir, out cr);

                Collection<SvnLogEventArgs> la;
                client.GetLog(dir, out la);

                Assert.That(la.Count, Is.EqualTo(2));
                Assert.That(la[0].Revision, Is.EqualTo(cr.Revision));
                Assert.That(la[0].Author, Is.EqualTo(user));
                Assert.That(la[0].LogMessage, Is.EqualTo(""));
            }
        }

        [Test]
        public void NonRecursiveDepthEmpty()
        {
            string dir = GetTempDir();
            Client.CheckOut(GetReposUri(TestReposType.Empty), dir);

            string name = Path.Combine(dir, "sd");
            string f = Path.Combine(name, "f");

            Client.CreateDirectory(name);
            File.WriteAllText(f, "qq");
            Client.Add(f);

            Client.Commit(name);

            Collection<SvnStatusEventArgs> st;

            Client.CropWorkingCopy(name, SvnDepth.Empty);
            Client.Delete(name);

            Client.GetStatus(name, out st);
            Assert.That(st.Count, Is.EqualTo(1));

            using (SvnWorkingCopyClient wcc = new SvnWorkingCopyClient())
            {
                Collection<SvnWorkingCopyEntryEventArgs> lst;
                wcc.GetEntries(name, out lst);

                Assert.That(lst.Count, Is.EqualTo(1));
            }


            SvnCommitArgs ca = new SvnCommitArgs();
            ca.Depth = SvnDepth.Empty;
            Client.Commit(name, ca);
        }
    }
}
