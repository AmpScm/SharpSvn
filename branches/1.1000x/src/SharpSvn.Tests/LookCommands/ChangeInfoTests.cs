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
using SharpSvn.Tests.Commands;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using System.Collections.ObjectModel;
using System.IO;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests.LookCommands
{
    [TestClass]
    public class ChangeInfoTests : HookTestBase
    {
        [TestMethod]
        public void ChangeInfo_GetInfoHead()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoEventArgs r;
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();

                SvnLookOrigin lo = new SvnLookOrigin(sbox.CreateRepository(SandBoxRepository.MergeScenario).LocalPath);
                Assert.That(cl.GetChangeInfo(lo, ia, out r));

                Assert.That(r, Is.Not.Null);
                Assert.That(r.Author, Is.EqualTo("merger"));
                Assert.That(r.Revision, Is.EqualTo(17L));
                Assert.That(r.BaseRevision, Is.EqualTo(16L));
                Assert.That(r.LogMessage, Is.EqualTo("Merged branch c back to trunk"));

                // This is the exact time of the commit in microseconds
                DateTime shouldBe = new DateTime(2008, 1, 27, 15, 7, 57, 9, DateTimeKind.Utc).AddTicks(4750);

                Assert.That(r.Time, Is.EqualTo(shouldBe));

                foreach (SvnChangeItem i in r.ChangedPaths)
                {
                    if (i.NodeKind != SvnNodeKind.File)
                        continue;
                    using (MemoryStream ms = new MemoryStream())
                    {
                        cl.Write(lo, i.Path, ms);
                    }
                }
            }
        }

        void WriteBigFile(string path)
        {
            using (StreamWriter fs = File.CreateText(path))
            {
                for (int i = 0; i < 10000; i++)
                    fs.WriteLine("A short Line that might look a bit longer if you see it multiple times");
            }
        }

        [TestMethod]
        public void ChangeInfo_BigWrite()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri uri = sbox.CreateRepository(SandBoxRepository.Empty);
            string dir = sbox.Wc;
            Client.CheckOut(uri, dir);

            string file = Path.Combine(dir, "bigfile");
            WriteBigFile(file);
            Client.Add(file);
            Client.SetProperty(file, "a", "b");
            using (InstallHook(uri, SvnHookType.PreCommit, OnPreCommitBigFile))
            {
                Client.Commit(dir);
            }

            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();

                SvnLookOrigin lo = new SvnLookOrigin(uri.LocalPath);

                using (MemoryStream ms = new MemoryStream())
                {
                    cl.Write(lo, "/bigfile", ms);
                }

                string b_p;
                Assert.That(cl.GetProperty(lo, "/bigfile", "a", out b_p));
                Assert.That(b_p, Is.EqualTo("b"));

                Collection<SvnLookListEventArgs> lst;
                cl.GetList(lo, "", out lst);

                Assert.That(lst, Is.Not.Null);
                Assert.That(lst.Count, Is.EqualTo(1));
                SvnLookListEventArgs r = lst[0];

                Assert.That(r, Is.Not.Null);
                Assert.That(r.Name, Is.EqualTo("bigfile"));
                Assert.That(r.NodeKind, Is.EqualTo(SvnNodeKind.File));
                Assert.That(r.Path, Is.EqualTo("bigfile"));
            }
        }

        private void OnPreCommitBigFile(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient cl = new SvnLookClient())
            {
                using (MemoryStream ms = new MemoryStream())
                {
                    cl.Write(e.HookArgs.LookOrigin, "/bigfile", ms);
                }

                string b_p;
                Assert.That(cl.GetProperty(e.HookArgs.LookOrigin, "/bigfile", "a", out b_p));
                Assert.That(b_p, Is.EqualTo("b"));
            }
        }

        [TestMethod]
        public void ChangeInfo_GetInfo12()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoEventArgs r;
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();

                SvnLookOrigin origin = new SvnLookOrigin(sbox.CreateRepository(SandBoxRepository.MergeScenario).LocalPath, 12);
                //ia.RetrieveChangedPaths = false; // Will fail if true

                Assert.That(cl.GetChangeInfo(origin, ia, out r));

                Assert.That(r, Is.Not.Null);
                Assert.That(r.Author, Is.EqualTo("merger"));
                Assert.That(r.Revision, Is.EqualTo(12L));
                Assert.That(r.BaseRevision, Is.EqualTo(11L));
                Assert.That(r.LogMessage, Is.EqualTo("Merge branch a - product roadmap"));

                // This is the exact time of the commit in microseconds
                DateTime shouldBe = new DateTime(2008, 1, 27, 15, 2, 26, 567, DateTimeKind.Utc).AddTicks(6830);

                Assert.That(r.Time, Is.EqualTo(shouldBe));
            }
        }

        /// <summary>
        /// Compares the log informationj of r1-r17 of the collab repository
        /// </summary>
        [TestMethod]
        public void ChangeInfo_GetInfoCompare()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            string reposPath = reposUri.LocalPath;

            using (SvnClient cl = new SvnClient())
            {
                SvnSetPropertyArgs sa = new SvnSetPropertyArgs();
                sa.BaseRevision = 17;
                sa.LogMessage = "Message";
                cl.RemoteSetProperty(reposUri, "MyProp", "Value", sa);
            }

            for (long ii = 1; ii < 19; ii++)
            {
                using (SvnLookClient lcl = new SvnLookClient())
                using (SvnClient cl = new SvnClient())
                {
                    SvnChangeInfoEventArgs r;
                    SvnChangeInfoArgs ia = new SvnChangeInfoArgs();
                    SvnLookOrigin origin = new SvnLookOrigin(reposPath, ii);

                    SvnLogArgs la = new SvnLogArgs();
                    la.Start = la.End = ii;

                    Collection<SvnLogEventArgs> lrc;
                    //ia.RetrieveChangedPaths = false; // Will fail if true
                    Assert.That(lcl.GetChangeInfo(origin, ia, out r));
                    Assert.That(cl.GetLog(reposUri, la, out lrc));

                    Assert.That(r, Is.Not.Null);
                    Assert.That(lrc.Count, Is.EqualTo(1));

                    SvnLogEventArgs lr = lrc[0];

                    Assert.That(r.Author, Is.EqualTo(lr.Author));
                    Assert.That(r.Revision, Is.EqualTo(lr.Revision));
                    Assert.That(r.BaseRevision, Is.EqualTo(lr.Revision - 1));
                    Assert.That(r.LogMessage, Is.EqualTo(lr.LogMessage));
                    Assert.That(r.Time, Is.EqualTo(lr.Time));

                    Assert.That(r.ChangedPaths, Is.Not.Null, "r.ChangedPaths({0})", ii);
                    Assert.That(lr.ChangedPaths, Is.Not.Null, "lr.ChangedPaths({0})", ii);

                    Assert.That(r.ChangedPaths.Count, Is.EqualTo(lr.ChangedPaths.Count));

                    for (int i = 0; i < r.ChangedPaths.Count; i++)
                    {
                        SvnChangeItem c = r.ChangedPaths[i];
                        SvnChangeItem lc = lr.ChangedPaths[c.Path];

                        Assert.That(c.Path, Is.EqualTo(lc.Path));
                        Assert.That(c.Action, Is.EqualTo(lc.Action));
                        Assert.That(c.CopyFromPath, Is.EqualTo(lc.CopyFromPath));
                        Assert.That(c.CopyFromRevision, Is.EqualTo(lc.CopyFromRevision));
                    }
                }
            }
        }
    }
}
