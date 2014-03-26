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
    /// Tests the NSvn.Client.MoveFile method
    /// </summary>
    [TestClass]
    public class MoveTests : TestBase
    {
        public MoveTests()
        {
            UseEmptyRepositoryForWc = false;
        }

        /// <summary>
        /// Tests moving a file in WC
        /// </summary>
        [TestMethod]
        public void Move_MoveWCFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string srcPath = Path.Combine(WcPath, "Form.cs");
            string dstPath = Path.Combine(WcPath, "renamedForm.cs");

            Assert.That(Client.Move(srcPath, dstPath));

            Assert.That(File.Exists(dstPath), "File wasn't moved");
            Assert.That(!File.Exists(srcPath), "Source File still exists");

        }

        /// <summary>
        /// Tests moving a directory in a WC
        /// </summary>
        [TestMethod]
        public void Move_MoveWCDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string srcPath = Path.Combine(WcPath, @"bin\Debug");
            string dstPath = Path.Combine(WcPath, @"renamedDebug");

            Assert.That(Client.Move(srcPath, dstPath));

            Assert.That(Directory.Exists(dstPath), "Directory wasn't moved");
            Assert.That(this.GetSvnStatus(srcPath), Is.EqualTo(SvnStatus.Deleted), "Status is not 'D'");
        }

        /// <summary>
        /// Tests moving a file in a Repos
        /// </summary>
        [TestMethod]
        public void Move_MoveReposFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = new Uri(sbox.CreateRepository(SandBoxRepository.AnkhSvnCases), "trunk/");
            Uri srcPath = new Uri(ReposUrl, "Form.cs");
            Uri dstPath = new Uri(ReposUrl, "renamedForm");

            SvnCommitResult ci;

            Assert.That(Client.RemoteMove(srcPath, dstPath, out ci));

            Assert.That(ci, Is.Not.Null);

            String cmd = this.RunCommand("svn", "list " + ReposUrl);
            Assert.That(cmd.IndexOf("Form.cs") == -1, "File wasn't moved");
            Assert.That(cmd.IndexOf("renamedForm") >= 0, "Moved file doens't exist");
        }

        bool ItemExists(Uri target)
        {
            bool found = false;
            using (SvnClient client = NewSvnClient(false, false))
            {
                SvnInfoArgs args = new SvnInfoArgs();
                args.ThrowOnError = false;
                args.Depth = SvnDepth.Empty;
                return client.Info(target, args, delegate(object sender, SvnInfoEventArgs e)
                {
                    found = true;
                }) && found;
            }
        }

        [TestMethod]
        public void Move_BasicMove()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;
            Uri ReposUrl = sbox.RepositoryUri;

            string TestPath = sbox.GetTempDir();

            using (SvnClient client = NewSvnClient(true, false))
            {
                string file = Path.Combine(WcPath, "LocalMoveBase");

                TouchFile(file);
                client.Add(file);

                Directory.CreateDirectory(Path.Combine(TestPath, "MoveTestImport"));
                TouchFile(Path.Combine(TestPath, "MoveTestImport/RemoteMoveBase"));

                client.RemoteImport(Path.Combine(TestPath, "MoveTestImport"), new Uri(ReposUrl, "RemoteMoveBase"));

                client.Commit(WcPath);

                Assert.That(ItemExists(new Uri(ReposUrl, "RemoteMoveBase")), Is.True, "Remote base does exist");
                Assert.That(ItemExists(new Uri(ReposUrl, "LocalMoveBase")), Is.True, "Local base does exist");

                client.RemoteMove(new Uri(ReposUrl, "RemoteMoveBase"), new Uri(ReposUrl, "RemoteMoveTarget"));
                client.Move(file, Path.Combine(WcPath, "LocalMoveTarget"));

                client.Commit(WcPath);

                Assert.That(ItemExists(new Uri(ReposUrl, "RemoteMoveTarget")), Is.True, "Remote target exists");
                Assert.That(ItemExists(new Uri(ReposUrl, "LocalMoveTarget")), Is.True, "Local target exists");
                Assert.That(ItemExists(new Uri(ReposUrl, "RemoteMoveBase")), Is.False, "Remote base does not exist");
                Assert.That(ItemExists(new Uri(ReposUrl, "LocalMoveBase")), Is.False, "Local base does not exist");
            }
        }

        [TestMethod]
        public void Move_MoveAndEdit()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;

            using (SvnClient client = NewSvnClient(true, false))
            {
                string file = Path.Combine(WcPath, "LMB");

                TouchFile(file);
                client.Add(file);

                File.AppendAllText(Path.Combine(WcPath, "LMB"), "Banana" + Environment.NewLine);

                client.Move(file, Path.Combine(WcPath, "LMB2"));

                File.AppendAllText(Path.Combine(WcPath, "LMB2"), "Banana" + Environment.NewLine);

                client.Commit(WcPath);
            }
        }

        [TestMethod]
        public void Move_MultiMove()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;

            using (SvnClient client = NewSvnClient(true, false))
            {
                string ren1 = Path.Combine(WcPath, "ren-1");

                using (StreamWriter sw = File.CreateText(ren1))
                {
                    sw.WriteLine("ToRename");
                }
                client.Add(ren1);
                client.Commit(WcPath);
                client.Update(WcPath);

                using (StreamWriter sw = File.AppendText(ren1))
                {
                    sw.WriteLine("AddedLine");
                }
                client.Move(ren1, ren1 + ".ren1");

                client.Commit(WcPath);

                client.Move(ren1 + ".ren1", ren1 + ".ren2");
                SvnMoveArgs ma = new SvnMoveArgs();
                ma.Force = true;
                client.Move(ren1 + ".ren2", ren1 + ".ren3", ma);
                client.Commit(WcPath);
            }
        }
    }
}
