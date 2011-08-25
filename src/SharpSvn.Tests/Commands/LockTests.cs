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
    /// Summary description for LockTest.
    /// </summary>
    [TestFixture]
    public class LockTests : TestBase
    {
        [SetUp]
        public override void SetUp()
        {
            base.SetUp();
        }

        [Test]
        public void TestBasicLock()
        {
            string filepath = Path.Combine(this.WcPath, "Form.cs");

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

            Client.Status(filepath, delegate(object sender, SvnStatusEventArgs e)
            {
                Assert.That(e.WorkingCopyInfo.LockOwner, Is.EqualTo(Environment.UserName));
                Assert.That(e.WorkingCopyInfo.LockToken, Is.Not.Null);
                Assert.That(e.WorkingCopyInfo.LockComment, Is.EqualTo(la.Comment));
                Assert.That(e.LocalLock, Is.Not.Null);
                Assert.That(e.LocalLock.Owner, Is.EqualTo(Environment.UserName));
                Assert.That(e.LocalLock.Token, Is.EqualTo(e.WorkingCopyInfo.LockToken));
                Assert.That(e.LocalLock.Comment, Is.EqualTo(comment.Replace("\n", "\r\n")));
                Assert.That(e.LocalLock.CreationTime, Is.EqualTo(e.WorkingCopyInfo.LockTime));
                gotIn = true;
            });

            Assert.That(gotIn);
        }

        void OnLockNotify(object sender, SvnNotifyEventArgs e)
        {
            GC.KeepAlive(e);
            //throw new NotImplementedException();
        }

        [Test]
        public void DualLockTest()
        {
            // Checks that sharpsvn sees a failed lock as an error (unlike the subversion c api)
            string wc1 = GetTempDir();
            string wc2 = GetTempDir();
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

        [Test, ExpectedException(typeof(SvnFileSystemLockException))]
        public void LockCommitTest()
        {
            string wc1 = GetTempDir();
            string wc2 = GetTempDir();
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
            }
            catch (SvnFileSystemException)
            {
                throw;
            }
        }

        [Test]
        public void ListLocks()
        {
            Uri trunk = new Uri(CollabReposUri, "trunk/");
            Uri about = new Uri(trunk, "about/");
            Uri aboutIndex = new Uri(about, "index.html");

            Client.RemoteLock(aboutIndex, "Mine!");

            Collection<SvnInfoEventArgs> lst;
            SvnInfoArgs ia = new SvnInfoArgs();
            ia.Depth = SvnDepth.Unknown;
            
            Assert.That(Client.GetInfo(aboutIndex, ia, out lst));
            Assert.That(lst.Count == 1);
            Assert.That(lst[0].Lock, Is.Not.Null, "Is locked - Unknown");

            ia.Depth = SvnDepth.Empty;
            Assert.That(Client.GetInfo(aboutIndex, ia, out lst));
            Assert.That(lst.Count == 1);
            Assert.That(lst[0].Lock, Is.Not.Null, "Is locked - Empty");

            ia.Depth = SvnDepth.Infinity;
            Assert.That(Client.GetInfo(aboutIndex, ia, out lst));
            Assert.That(lst.Count == 1);
            Assert.That(lst[0].Lock, Is.Not.Null, "Is locked - Infinity");

            ia.Depth = SvnDepth.Unknown;
            Assert.That(Client.GetInfo(about, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(1)); // Just the dir

            // And the next cases where failing because .Info() didn't pass
            // an explicit operational revision to svn_client_info2().

            ia.Depth = SvnDepth.Files;
            Assert.That(Client.GetInfo(about, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(2));
            Assert.That(lst[1].Path.EndsWith("index.html"));
            Assert.That(lst[1].Lock, Is.Not.Null, "Is locked - Dir - Files");

            ia.Depth = SvnDepth.Children;
            Assert.That(Client.GetInfo(about, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(2));
            Assert.That(lst[1].Path.EndsWith("index.html"));
            Assert.That(lst[1].Lock, Is.Not.Null, "Is locked - Dir - Children");

            ia.Depth = SvnDepth.Infinity;
            Assert.That(Client.GetInfo(about, ia, out lst));
            Assert.That(lst.Count, Is.EqualTo(2));
            Assert.That(lst[1].Path.EndsWith("index.html"));
            Assert.That(lst[1].Lock, Is.Not.Null, "Is locked - Dir - Infinity");
        }
    }
}
