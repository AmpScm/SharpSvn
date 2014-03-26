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
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Summary description for LockTest.
    /// </summary>
    [TestClass]
    public class LockTests : TestBase
    {
        [TestMethod]
        public void Lock_BasicLock()
        {
            string filepath = Path.Combine(this.WcPath, "Form.cs");
            TouchFile(filepath);
            Client.Add(filepath);
            Client.Commit(filepath);

            Client.Status(filepath, delegate(object sender, SvnStatusEventArgs e)
            {
                Assert.That(e.LocalLock, Is.Null);
            });

            string comment = "Moo\nMa";
            bool gotIn = false;
            SvnLockArgs la = new SvnLockArgs();
            la.Comment = comment;
            la.Notify +=
                delegate(object sender, SvnNotifyEventArgs e)
                {
                    Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.LockLocked));
                    gotIn = true;
                };
            Client.Lock(new string[] { filepath }, la);
            Assert.That(gotIn);

            gotIn = false;
            comment = comment.Replace("\n", "\r\n");

            Client.Status(filepath, delegate(object sender, SvnStatusEventArgs e)
            {
                Assert.That(e.LocalLock, Is.Not.Null);
                Assert.That(e.LocalLock.Owner, Is.EqualTo(Environment.UserName));
                Assert.That(e.LocalLock.Token, Is.Not.Null);
                Assert.That(e.LocalLock.Comment, Is.EqualTo(comment));
                Assert.That(e.LocalLock.CreationTime.ToLocalTime(), Is.LessThanOrEqualTo(DateTime.Now).And.GreaterThan(DateTime.Now - new TimeSpan(0,0,20)));
                gotIn = true;
            });

            Assert.That(gotIn);
        }

        void OnLockNotify(object sender, SvnNotifyEventArgs e)
        {
            GC.KeepAlive(e);
            //throw new NotImplementedException();
        }

        [TestMethod]
        public void Lock_DualLock()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            // Checks that sharpsvn sees a failed lock as an error (unlike the subversion c api)
            string wc1 = sbox.Wc;
            string wc2 = sbox.GetTempDir("2");
            Uri repos = new Uri(CollabReposUri, "trunk/");

            Client.CheckOut(repos, wc1);
            Client.CheckOut(repos, wc2);

            string index1 = Path.Combine(wc1, "index.html");
            string index2 = Path.Combine(wc2, "index.html");

            SvnInfoEventArgs ii;
            Client.GetInfo(index1, out ii);

            Assert.That(ii.Lock, Is.Null, "Not locked");

            Client.Lock(Path.Combine(wc1, "index.html"), "First");

            Client.GetInfo(index1, out ii);
            Assert.That(ii.Lock, Is.Not.Null, "Locked");

            try
            {
                Client.Lock(index2, "Second");

                Assert.That(false, "Lock just didn't do anything");
            }
            catch (SvnFileSystemLockException ex)
            {
                Assert.That(ex.Message.Contains("index.html"));
            }
        }

        [TestMethod]
        public void Lock_CommitTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string wc1 = sbox.Wc;
            string wc2 = sbox.GetTempDir("WC2");
            Uri repos = new Uri(CollabReposUri, "trunk/");

            Client.CheckOut(repos, wc1);
            Client.CheckOut(repos, wc2);

            string index1 = Path.Combine(wc1, "index.html");
            string index2 = Path.Combine(wc2, "index.html");

            Client.Lock(index1, "q!");
            File.WriteAllText(index2, "QQQQQ");

            try
            {
                Client.Commit(index2);

                Assert.Fail("Commit should have failed");
            }
            catch (SvnException e)
            {
                Assert.That(e.GetCause<SvnFileSystemLockException>(), Is.Not.Null, "Caused by lock error");
            }
        }

        [TestMethod]
        public void Lock_ListLocks()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.DefaultBranched);

            Uri trunk = sbox.Uri;
            Uri src = new Uri(sbox.Uri, "src/");
            Uri srcFile1 = new Uri(src, "file1.cs");

            Client.RemoteLock(srcFile1, "Mine!");

            Collection<SvnInfoEventArgs> lst;
            SvnInfoArgs ia = new SvnInfoArgs();
            ia.Depth = SvnDepth.Unknown;
            
            Assert.That(Client.GetInfo(srcFile1, ia, out lst));
            Assert.That(lst.Count == 1);
            Assert.That(lst[0].Lock, Is.Not.Null, "Is locked - Unknown");

            ia.Depth = SvnDepth.Empty;
            Assert.That(Client.GetInfo(srcFile1, ia, out lst));
            Assert.That(lst.Count == 1);
            Assert.That(lst[0].Lock, Is.Not.Null, "Is locked - Empty");

            ia.Depth = SvnDepth.Infinity;
            Assert.That(Client.GetInfo(srcFile1, ia, out lst));
            Assert.That(lst.Count == 1);
            Assert.That(lst[0].Lock, Is.Not.Null, "Is locked - Infinity");

            ia.Depth = SvnDepth.Unknown;
            Assert.That(Client.GetInfo(src, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(1)); // Just the dir

            // And the next cases where failing because .Info() didn't pass
            // an explicit operational revision to svn_client_info2().

            ia.Depth = SvnDepth.Files;
            Assert.That(Client.GetInfo(src, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(5));
            bool found = false;
            foreach(SvnInfoEventArgs e in lst)
            {
                if (e.Path.EndsWith("file1.cs"))
                {
                    found = true;
                    Assert.That(e.Lock, Is.Not.Null, "Is locked - Dir - Files");
                }
            }
            Assert.That(found);

            ia.Depth = SvnDepth.Children;
            Assert.That(Client.GetInfo(src, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(5));
            found = false;
            foreach (SvnInfoEventArgs e in lst)
            {
                if (e.Path.EndsWith("file1.cs"))
                {
                    found = true;
                    Assert.That(e.Lock, Is.Not.Null, "Is locked - Dir - Children");
                }
            }
            Assert.That(found);

            ia.Depth = SvnDepth.Infinity;
            Assert.That(Client.GetInfo(src, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(5));
            found = false;
            foreach (SvnInfoEventArgs e in lst)
            {
                if (e.Path.EndsWith("file1.cs"))
                {
                    found = true;
                    Assert.That(e.Lock, Is.Not.Null, "Is locked - Dir - Infinity");
                }
            }
            Assert.That(found);

        }
    }
}
