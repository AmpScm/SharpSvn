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
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class InfoTests : TestBase
    {
        [TestMethod]
        public void InfoTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string dir = sbox.Wc;
            SvnUpdateResult r;

            Client.CheckOut(CollabReposUri, dir, out r);

            int i = 0;
            Assert.That(Client.Info(dir,
                delegate(object sender, SvnInfoEventArgs e)
                {
                    i++;
                    Assert.That(e.Revision, Is.EqualTo(r.Revision));
                    Assert.That(e.LastChangeRevision, Is.EqualTo(r.Revision));
                }), Is.True);


            Assert.That(i, Is.EqualTo(1));

            i = 0;
            SvnInfoArgs aa = new SvnInfoArgs();
            aa.Depth = SvnDepth.Children;

            Assert.That(Client.Info(dir, aa,
                delegate(object sender, SvnInfoEventArgs e)
                {
                    i++;
                    Assert.That(e.Revision, Is.EqualTo(r.Revision));
                    Assert.That(e.LastChangeRevision, Is.LessThanOrEqualTo(r.Revision));
                }), Is.True);

            Assert.That(i, Is.EqualTo(4)); // trunk branches tags
        }

        [TestMethod]
        public void WcCasing()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            string dir = sbox.Wc;
            SvnUpdateResult r;

            Assert.That(Client.CheckOut(CollabReposUri, dir, out r));
            Collection<SvnInfoEventArgs> items;
            SvnInfoArgs aa = new SvnInfoArgs();
            aa.Depth = SvnDepth.Children;

            Assert.That(Client.GetInfo(Path.Combine(dir, "trunk"), aa, out items));

            foreach (SvnInfoEventArgs ia in items)
            {
                bool reached = false;
                Assert.That(Client.Info(ia.FullPath,
                    delegate(object sender, SvnInfoEventArgs e)
                    {
                        reached = true;
                    }));

                Assert.That(reached);

                reached = false;
                SvnInfoArgs a = new SvnInfoArgs();
                a.ThrowOnError = false;
                Client.Info(ia.FullPath.ToUpperInvariant(), a,
                    delegate(object sender, SvnInfoEventArgs e)
                    {
                        reached = true;
                    });

                if (ia.NodeKind != SvnNodeKind.Directory)
                    Assert.That(reached, Is.False);
            }
        }

        [TestMethod]
        public void TestSpace()
        {
            using (SvnClient client = new SvnClient())
            {
                Uri uri = new Uri("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with%20spaces/test.txt#aaa");

                string s = uri.GetComponents(UriComponents.SchemeAndServer | UriComponents.UserInfo | UriComponents.Path, UriFormat.UriEscaped);
                Assert.That(s, Is.EqualTo("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with%20spaces/test.txt"));

                SvnUriTarget ut = new SvnUriTarget(uri);

                Assert.That(ut.TargetName, Is.EqualTo("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with%20spaces/test.txt"));

                uri = new Uri("http://sharpsvn.googlecode.com/svn/trunk/tests/folder with spaces/test.txt#aaa");

                s = uri.GetComponents(UriComponents.SchemeAndServer | UriComponents.UserInfo | UriComponents.Path, UriFormat.UriEscaped);
                Assert.That(s, Is.EqualTo("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with%20spaces/test.txt"));

                /*SvnInfoEventArgs ie;
                client.GetInfo(new Uri("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with%20spaces/test.txt"), out ie);

                Assert.That(ie, Is.Not.Null);*/
            }
        }

        public void TestDash()
        {
            using (SvnClient client = new SvnClient())
            {
                Uri uri = new Uri("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with%20spaces/test%23.txt");

                string txt = "file/a/%23ABC";

                Assert.That(Uri.UnescapeDataString(txt), Is.EqualTo("file/a/#ABC"));

                SvnInfoEventArgs ie;
                client.GetInfo(uri, out ie);

                Assert.That(ie, Is.Not.Null);
            }
        }

        [TestMethod]
        public void WcDirMissing()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            string dir = sbox.Wc;
            SvnUpdateResult r;

            Assert.That(Client.CheckOut(CollabReposUri, dir, out r));

            Directory.Move(Path.Combine(dir, "trunk"), Path.Combine(dir, "banaan"));

            SvnInfoEventArgs iaParent;
            SvnInfoEventArgs iaTrunk;

            Client.GetInfo(dir, out iaParent);
            Client.GetInfo(Path.Combine(dir, "trunk"), out iaTrunk);

            Assert.That(iaParent.FullPath, Is.EqualTo(dir));
            Assert.That(iaTrunk.FullPath, Is.Not.EqualTo(dir));
            //Assert.That(iaParent.Uri, Is.EqualTo(iaTrunk.Uri));

            SvnWorkingCopyVersion ver;
            SvnWorkingCopyClient wcC = new SvnWorkingCopyClient();

            Assert.That(wcC.GetVersion(dir, out ver));
            Assert.That(ver, Is.Not.Null);

            Assert.That(ver.Modified, Is.True);
            Assert.That(ver.Switched, Is.False);
            Assert.That(ver.Start, Is.EqualTo(17));
            Assert.That(ver.End, Is.EqualTo(17));
            Assert.That(ver.IncompleteWorkingCopy, Is.False);
        }

        [TestMethod]
        public void TestInfo()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.Empty);
            Uri WcUri = reposUri;
            string wc = sbox.Wc;
            using (SvnClient client = NewSvnClient(true, false))
            {
                client.CheckOut(reposUri, wc);

                string file = Path.Combine(wc, "InfoFile");
                TouchFile(file);
                client.Add(file);

                bool visited = false;
                client.Info(file, delegate(object sender, SvnInfoEventArgs e)
                {
                    Assert.That(e.ChangeList, Is.Null);
                    Assert.That(e.Checksum, Is.Null);
                    Assert.That(e.ConflictNew, Is.Null);
                    Assert.That(e.ConflictOld, Is.Null);
                    Assert.That(e.ConflictWork, Is.Null);
                    Assert.That(e.ContentTime, Is.EqualTo(DateTime.MinValue));
                    Assert.That(e.CopyFromRevision, Is.EqualTo(-1L));
                    Assert.That(e.CopyFromUri, Is.Null);
                    Assert.That(e.Depth, Is.EqualTo(SvnDepth.Unknown));
                    Assert.That(e.FullPath, Is.EqualTo(file));
                    Assert.That(e.HasLocalInfo, Is.True);
                    Assert.That(e.LastChangeAuthor, Is.Null);
                    Assert.That(e.LastChangeTime, Is.EqualTo(DateTime.MinValue));
                    Assert.That(e.LastChangeRevision, Is.EqualTo(-1L), "Not committed yet; LastChangeRevision = -1"); // Not committed yet
                    Assert.That(e.Lock, Is.Null);
                    Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
                    Assert.That(e.Path, Is.Not.Null);
                    Assert.That(e.PropertyEditFile, Is.Null);
                    //Assert.That(e.PropertyTime, Is.EqualTo(DateTime.MinValue));
                    Assert.That(e.ContentTime, Is.EqualTo(DateTime.MinValue));
                    Assert.That(e.RepositorySize, Is.EqualTo(-1L));
                    Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri), "Repository valid");
                    Assert.That(e.Revision, Is.LessThanOrEqualTo(0L), "Not committed yet");
                    Assert.That(e.Schedule, Is.EqualTo(SvnSchedule.Add));
                    Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "InfoFile")));
                    Assert.That(e.WorkingCopySize, Is.EqualTo(-1L));
                    visited = true;
                });
                Assert.That(visited);

                SvnCommitResult commitData;
                client.Commit(wc, out commitData);
                visited = false;
                client.Info(file, delegate(object sender, SvnInfoEventArgs e)
                    {
                        Assert.That(e.ChangeList, Is.Null);
                        Assert.That(e.Checksum, Is.EqualTo("da39a3ee5e6b4b0d3255bfef95601890afd80709"));
                        Assert.That(e.ConflictNew, Is.Null);
                        Assert.That(e.ConflictOld, Is.Null);
                        Assert.That(e.ConflictWork, Is.Null);
                        Assert.That(e.ContentTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                        Assert.That(e.CopyFromRevision, Is.EqualTo(-1L));
                        Assert.That(e.CopyFromUri, Is.Null);
                        Assert.That(e.Depth, Is.EqualTo(SvnDepth.Unknown));
                        Assert.That(e.FullPath, Is.EqualTo(file));
                        Assert.That(e.HasLocalInfo, Is.True);
                        Assert.That(e.LastChangeAuthor, Is.EqualTo(Environment.UserName));
                        Assert.That(e.LastChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                        Assert.That(e.LastChangeRevision, Is.EqualTo(commitData.Revision));
                        Assert.That(e.Lock, Is.Null);
                        Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
                        Assert.That(e.Path, Is.Not.Null);
                        Assert.That(e.PropertyEditFile, Is.Null);
                        //Assert.That(e.PropertyTime, Is.EqualTo(e.ContentTime)); // Not static, might change
                        Assert.That(e.RepositorySize, Is.EqualTo(-1L));
                        Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                        Assert.That(e.Revision, Is.EqualTo(commitData.Revision));
                        Assert.That(e.Schedule, Is.EqualTo(SvnSchedule.Normal));
                        Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "InfoFile")));
                        Assert.That(e.WorkingCopySize, Is.EqualTo(0L));
                        visited = true;
                    });
                Assert.That(visited);

                visited = false;
                client.Info(new Uri(WcUri, "InfoFile"), delegate(object sender, SvnInfoEventArgs e)
                {
                    Assert.That(e.ChangeList, Is.Null);
                    Assert.That(e.Checksum, Is.Null);
                    Assert.That(e.ConflictNew, Is.Null);
                    Assert.That(e.ConflictOld, Is.Null);
                    Assert.That(e.ConflictWork, Is.Null);
                    Assert.That(e.ContentTime, Is.EqualTo(DateTime.MinValue));
                    Assert.That(e.CopyFromRevision, Is.LessThanOrEqualTo(0L));
                    Assert.That(e.CopyFromUri, Is.Null);
                    Assert.That(e.Depth, Is.EqualTo(SvnDepth.Unknown));
                    Assert.That(e.FullPath, Is.Null);
                    Assert.That(e.HasLocalInfo, Is.False);
                    Assert.That(e.LastChangeAuthor, Is.EqualTo(Environment.UserName));
                    Assert.That(e.LastChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                    Assert.That(e.LastChangeTime, Is.LessThan(DateTime.UtcNow + new TimeSpan(0, 5, 0)));
                    Assert.That(e.LastChangeRevision, Is.EqualTo(commitData.Revision));
                    Assert.That(e.Lock, Is.Null);
                    Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
                    Assert.That(e.Path, Is.Not.Null);
                    Assert.That(e.PropertyEditFile, Is.Null);
                    //Assert.That(e.PropertyTime, Is.EqualTo(e.ContentTime));
                    Assert.That(e.RepositorySize, Is.EqualTo(0L));
                    Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                    Assert.That(e.Revision, Is.EqualTo(commitData.Revision));
                    Assert.That(e.Schedule, Is.EqualTo(SvnSchedule.Normal));
                    Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "InfoFile")));
                    Assert.That(e.WorkingCopySize, Is.EqualTo(-1L));
                    visited = true;
                });
                Assert.That(visited);
            }
        }

        [TestMethod]
        public void InfoTest2()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Default);

            using (SvnClient client = NewSvnClient(false, false))
            {
                Collection<SvnInfoEventArgs> items;
                client.GetInfo(ReposUrl, new SvnInfoArgs(), out items);

                Assert.That(items, Is.Not.Null, "Items retrieved");
                Assert.That(items.Count, Is.EqualTo(1), "1 info item");

                string fileName = SvnTools.GetFileName(ReposUrl);

                SvnInfoEventArgs info = items[0];
                Assert.That(info.Uri.AbsoluteUri, Is.EqualTo(ReposUrl.AbsoluteUri), "Repository uri matches");
                Assert.That(info.HasLocalInfo, Is.False, "No WC info");
                Assert.That(info.Path, Is.EqualTo(Path.GetFileName(ReposUrl.LocalPath.TrimEnd('\\'))), "Path is end of folder name");
            }
        }
    }
}
