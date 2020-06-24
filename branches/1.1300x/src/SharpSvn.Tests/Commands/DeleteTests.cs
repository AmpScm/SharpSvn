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
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests for the Client::Checkout method
    /// </summary>
    [TestClass]
    public class DeleteTests : TestBase
    {
        [TestMethod]
        public void Delete_AllFiles()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);
            Uri uri = sbox.RepositoryUri;
            string wc = sbox.Wc;

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
        [TestMethod]
        public void Delete_WCFiles()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string path1 = Path.Combine(WcPath, "Form.cs");
            string path2 = Path.Combine(WcPath, "AssemblyInfo.cs");

            SvnDeleteArgs a = new SvnDeleteArgs();

            Assert.That(Client.Delete(new string[] { path1, path2 }, a));

            Assert.That(!File.Exists(path1), "File not deleted");
            Assert.That(!File.Exists(path2), "File not deleted");

            Assert.That(this.GetSvnStatus(path1), Is.EqualTo(SvnStatus.Deleted), "File not deleted");
            Assert.That(this.GetSvnStatus(path2), Is.EqualTo(SvnStatus.Deleted), "File not deleted");

            Assert.That(Client.Commit(WcPath));
        }

        [TestMethod]
        public void Delete_TestReplacedStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;

            string file = CreateTextFile(WcPath, "ToBeReplaced.vb");

            Client.Add(file);
            Assert.That(Client.Commit(WcPath));
            Client.Delete(file);

            file = CreateTextFile(WcPath, "ToBeReplaced.vb");
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
        [TestMethod]
        public void Delete_TestDeleteFromRepos()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.AnkhSvnCases);
            Uri path1 = new Uri(ReposUrl, "trunk/doc");
            Uri path2 = new Uri(ReposUrl, "trunk/Form.cs");

            SvnCommitResult ci;
            SvnDeleteArgs a = new SvnDeleteArgs();

            Assert.That(Client.RemoteDelete(new Uri[] { path1, path2 }, a, out ci));

            String cmd = this.RunCommand("svn", "list " + ReposUrl);
            Assert.That(cmd.IndexOf("doc") == -1, "Directory wasn't deleted ");
            Assert.That(cmd.IndexOf("Form.cs") == -1, "Directory wasn't deleted");

            Assert.That(ci, Is.Not.Null, "CommitInfo is invalid");
        }

        [TestMethod]
        public void Delete_ForceDelete()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string path = Path.Combine(WcPath, "Form.cs");

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
