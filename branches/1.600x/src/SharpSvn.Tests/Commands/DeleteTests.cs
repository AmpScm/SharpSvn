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

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests for the Client::Checkout method
    /// </summary>
    [TestFixture]
    public class DeleteTests : TestBase
    {
        [SetUp]
        public override void SetUp()
        {
            base.SetUp();
        }

        [TearDown]
        public override void TearDown()
        {
            base.TearDown();
        }

        [Test]
        public void DeleteAllFiles()
        {
            Uri uri = ReposUrl;
            string wc = GetTempDir();

            Client.CheckOut(uri, wc);

            for (int i = 0; i < 10; i++)
            {
                string file = Path.Combine(wc, string.Format("file{0}.txt", i));

                File.WriteAllText(file, "");

                Client.Add(file);
            }
            SvnCommitArgs ca = new SvnCommitArgs();
            ca.LogMessage = "Message";
            Client.Commit(wc, ca);

            foreach (FileInfo f in new DirectoryInfo(wc).GetFiles())
            {
                Client.Delete(f.FullName);
            }

            Client.Commit(wc, ca);
            Client.Update(wc);

            foreach (FileInfo f in new DirectoryInfo(wc).GetFiles())
            {
                Assert.That(false, "No files should exist at this point");
            }
        }


        /// <summary>
        /// Tests deleting files in a working copy
        /// </summary>
        [Test]
        public void TestDeleteWCFiles()
        {
            string path1 = Path.Combine(this.WcPath, "Form.cs");
            string path2 = Path.Combine(this.WcPath, "AssemblyInfo.cs");

            SvnDeleteArgs a = new SvnDeleteArgs();

            Assert.That(Client.Delete(new string[] { path1, path2 }, a));

            Assert.That(!File.Exists(path1), "File not deleted");
            Assert.That(!File.Exists(path2), "File not deleted");

            Assert.That(this.GetSvnStatus(path1), Is.EqualTo('D'), "File not deleted");
            Assert.That(this.GetSvnStatus(path2), Is.EqualTo('D'), "File not deleted");

            Assert.That(Client.Commit(WcPath));
        }

        [Test]
        public void TestReplacedStatus()
        {
            string file = CreateTextFile("ToBeReplaced.vb");

            Client.Add(file);
            Assert.That(Client.Commit(WcPath));
            Client.Delete(file);

            file = CreateTextFile("ToBeReplaced.vb");
            Assert.That(Client.Add(file));

            bool reached = false;
            Assert.That(Client.Status(file, delegate(object sender, SvnStatusEventArgs e)
            {
                reached = true;
                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Replaced));
            }));
            Assert.That(reached);

            Assert.That(Client.Commit(WcPath));
        }

        /// <summary>
        /// Tests deleting a directory in the repository
        /// </summary>
        //TODO: Implement the variable admAccessBaton
        [Test]
        public void TestDeleteFromRepos()
        {
            Uri path1 = new Uri(this.ReposUrl, "doc");
            Uri path2 = new Uri(this.ReposUrl, "Form.cs");

            SvnCommitResult ci;
            SvnDeleteArgs a = new SvnDeleteArgs();

            Assert.That(Client.RemoteDelete(new Uri[] { path1, path2 }, a, out ci));

            String cmd = this.RunCommand("svn", "list " + this.ReposUrl);
            Assert.That(cmd.IndexOf("doc") == -1, "Directory wasn't deleted ");
            Assert.That(cmd.IndexOf("Form.cs") == -1, "Directory wasn't deleted");

            Assert.That(ci, Is.Not.Null, "CommitInfo is invalid");
        }

        [Test]
        public void TestForceDelete()
        {
            string path = Path.Combine(this.WcPath, "Form.cs");

            // modify the file
            using (StreamWriter writer = new StreamWriter(path, true))
            {
                writer.WriteLine("Hi ho");
            }

            // this will throw if force doesn't work
            SvnDeleteArgs a = new SvnDeleteArgs();
            a.ThrowOnError = false;

            Assert.That(Client.Delete(path, a), Is.False);

            a.ThrowOnError = true;
            a.Force = true;

            Assert.That(Client.Delete(path, a), Is.True, "Delete failed");
        }

    }
}
