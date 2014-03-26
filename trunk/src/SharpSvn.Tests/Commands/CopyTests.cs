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
    /// Tests the NSvn.Client.MoveFile method
    /// </summary>
    [TestClass]
    public class CopyTests : TestBase
    {
        public CopyTests()
        {
            UseEmptyRepositoryForWc = false;
        }

        /// <summary>
        /// Tests copying a file in WC -> WC
        /// </summary>
        [TestMethod]
        public void Copy_WCWCFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string srcPath = Path.Combine(WcPath, "Form.cs");
            string dstPath = Path.Combine(WcPath, "renamedForm.cs");

            Assert.That(Client.Copy(new SvnPathTarget(srcPath, SvnRevision.Head), dstPath));

            Assert.That(File.Exists(dstPath), "File wasn't copied");
            Assert.That(File.Exists(srcPath), "Source File don't exists");

            Assert.That(Client.Commit(WcPath));
        }

        /// <summary>
        /// Tests copying a directory in a WC -> WC
        /// </summary>
        [TestMethod]
        public void Copy_WCWCDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string srcPath = Path.Combine(WcPath, @"bin\Debug");
            string dstPath = Path.Combine(WcPath, @"copyDebug");

            Assert.That(this.Client.Copy(new SvnPathTarget(srcPath), dstPath));

            Assert.That(Directory.Exists(dstPath), "Directory don't exist ");
            Assert.That(this.GetSvnStatus(dstPath), Is.EqualTo(SvnStatus.Added), " Status is not 'A'  ");

            Assert.That(Client.Commit(WcPath));
        }

        /// <summary>
        /// Tests copying a directory in a WC -> URL (repository)
        /// </summary>
        [TestMethod]
        public void Copy_WCReposDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string srcPath = Path.Combine(WcPath, @"bin\Debug");
            Uri dstPath = new Uri(sbox.RepositoryUri, "copyDebug/");

            SvnCommitResult ci;

            Assert.That(Client.RemoteCopy(new SvnPathTarget(srcPath, SvnRevision.Head), dstPath, out ci));

            Assert.That(ci, Is.Not.Null);

            String cmd = this.RunCommand("svn", "list " + sbox.RepositoryUri.AbsoluteUri);
            Assert.That(cmd.IndexOf("copyDebug") >= 0, "File wasn't copied");
        }

        /// <summary>
        /// Tests copying a from a Repository to WC: URL -> WC
        /// </summary>
        [TestMethod]
        public void Copy_ReposWCFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases, false);
            string WcPath = sbox.Wc;

            Uri srcUri = new Uri(sbox.RepositoryUri, "trunk/Form.cs");
            string dstPath = Path.Combine(WcPath, "copyForm");

            this.Client.Copy(new SvnUriTarget(srcUri), dstPath);

            Assert.That(this.GetSvnStatus(dstPath), Is.EqualTo(SvnStatus.Added), " File is not copied  ");

            Assert.That(Client.Commit(WcPath));
        }

        [TestMethod]
        public void Copy_ReposToWcWithParents()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            Uri srcUri = new Uri(sbox.RepositoryUri, "trunk/Form.cs");
            SvnCopyArgs ca = new SvnCopyArgs();
            ca.CreateParents = true;
            Client.Copy(srcUri, Path.Combine(WcPath, "dir/sub/with/more/levels"), ca);
        }

        /// <summary>
        /// Tests copying a file within a Repos: URL -> URL
        /// </summary>
        [TestMethod]
        public void Copy_ReposReposFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = new Uri(sbox.CreateRepository(SandBoxRepository.AnkhSvnCases), "trunk/");

            Uri srcUri = new Uri(ReposUrl, "Form.cs");
            Uri dstUri = new Uri(ReposUrl, "copyForm");

            SvnCommitResult ci;

            Assert.That(Client.RemoteCopy(srcUri, dstUri, out ci));
            Assert.That(ci, Is.Not.Null);

            String cmd = this.RunCommand("svn", "list " + ReposUrl);
            Assert.That(cmd.IndexOf("Form.cs") >= 0, "File wasn't copied");
            Assert.That(cmd.IndexOf("copyForm") >= 0, "Copied file doesn't exist");
        }

        [TestMethod]
        public void Copy_SimpleBranch0()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.Default);
            Uri trunk = new Uri(reposUri, "trunk/");

            SvnCopyArgs ca = new SvnCopyArgs();
            ca.CreateParents = true;

            // Subversion 1.5.4 throws an AccessViolationException here
            Client.RemoteCopy(trunk, new Uri(reposUri, "branch/"), ca);
        }

        [TestMethod, ExpectedException(typeof(SvnFileSystemException))]
        public void Copy_SimpleBranch1()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.Default);
            Uri trunk = new Uri(reposUri, "trunk/");

            Client.RemoteCopy(trunk, new Uri(reposUri, "branch/"));

            SvnCopyArgs ca = new SvnCopyArgs();
            ca.CreateParents = true;

            // Subversion 1.5.4 throws an AccessViolationException here
            Client.RemoteCopy(trunk, new Uri(reposUri, "branch/"), ca);
        }

        [TestMethod]
        public void Copy_ReposCopy()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.Default);
            Uri trunk = new Uri(reposUri, "trunk/");
            Uri branch = new Uri(reposUri, "my-branches/new-branch");

            SvnCopyArgs ca = new SvnCopyArgs();
            ca.LogMessage = "Message";
            ca.CreateParents = true;
            Client.RemoteCopy(trunk, branch, ca);

            int n = 0;
            Client.List(branch, delegate(object sender, SvnListEventArgs e)
            {
                if (e.Entry.NodeKind == SvnNodeKind.File)
                    n++;
            });

            Assert.That(n, Is.GreaterThan(0), "Copied files");
        }

        [TestMethod]
        public void Copy_CopyTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            string WcPath = sbox.Wc;
            Uri WcUri = sbox.Uri;

            using (SvnClient client = NewSvnClient(true, false))
            {
                string file = Path.Combine(WcPath, "CopyBase");

                TouchFile(file);
                client.Add(file);

                client.Commit(WcPath);

                client.RemoteCopy(new Uri(WcUri, "CopyBase"), new Uri(WcUri, "RemoteCopyBase"));
                bool visited = false;
                bool first = true;
                client.Log(new Uri(WcUri, "RemoteCopyBase"), delegate(object sender, SvnLogEventArgs e)
                {
                    if (first)
                    {
                        first = false;
                        foreach (SvnChangeItem i in e.ChangedPaths)
                        {
                            Assert.That(i.Path, Is.StringEnding("trunk/RemoteCopyBase"), "Path ends with folder/RemoteCopyBase");
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.CopyFromPath, Is.StringEnding("trunk/CopyBase"), "CopyFromPath ends with folder/CopyBase");
                            Assert.That(i.CopyFromRevision, Is.GreaterThan(0L));
                            Assert.That(i.NodeKind, Is.EqualTo(SvnNodeKind.File));
                        }
                    }
                    else
                    {
                        foreach (SvnChangeItem i in e.ChangedPaths)
                        {
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.Path, Is.StringEnding("trunk/CopyBase"), "Path ends with folder/CopyBase");
                            Assert.That(i.NodeKind, Is.EqualTo(SvnNodeKind.File));
                            visited = true;
                        }
                    }
                });
                Assert.That(visited, "Visited log item");

                client.Copy(new SvnPathTarget(file), Path.Combine(WcPath, "LocalCopy"));
                client.Commit(WcPath);
                visited = false;
                first = true;
                client.Log(new Uri(WcUri, "LocalCopy"), delegate(object sender, SvnLogEventArgs e)
                {
                    if (first)
                    {
                        foreach (SvnChangeItem i in e.ChangedPaths)
                        {
                            Assert.That(i.Path, Is.StringEnding("trunk/LocalCopy"), "Path ends with folder/LocalCopy");
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.CopyFromPath, Is.StringEnding("trunk/CopyBase"), "CopyFromPath ensd with folder/CopyBase");
                            Assert.That(i.CopyFromRevision, Is.GreaterThan(0L));
                        }
                        first = false;
                    }
                    else
                        foreach (SvnChangeItem i in e.ChangedPaths)
                        {
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.Path, Is.StringEnding("trunk/CopyBase"), "Path ends with folder/CopyBase");
                            visited = true;
                        }

                });
                Assert.That(visited, "Visited local log item");
            }
        }
    }
}
