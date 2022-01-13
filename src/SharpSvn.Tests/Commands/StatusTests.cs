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
using System.Text;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::Status
    /// </summary>
    [TestClass]
    public class StatusTests : TestBase
    {

        /// <summary>Compares the status from Client::Status with the output from
        /// commandline client</summary>
        [TestMethod]
        public void Status_LocalStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;
            string unversioned = CreateTextFile(WcPath, "unversioned.cs");
            string added = CreateTextFile(WcPath, "added.cs");
            this.RunCommand("svn", "add " + added);
            string changed = CreateTextFile(WcPath, "Form.cs");
            string ignored = CreateTextFile(WcPath, "foo.ignored");
            string propChange = Path.Combine(WcPath, "App.ico");
            this.RunCommand("svn", "ps foo bar " + propChange);
            this.RunCommand("svn", "ps svn:ignore *.ignored " + WcPath);

            SvnStatusArgs a = new SvnStatusArgs();
            a.Depth = SvnDepth.Empty;

            Client.Status(unversioned, a,
                delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(SvnStatus.NotVersioned, Is.EqualTo(e.LocalContentStatus),
                "Wrong text status on " + unversioned);
                    Assert.That(string.Compare(unversioned, e.Path, true) == 0, "Unversioned filenames don't match");
                });

            Client.Status(added, a,
                delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(SvnStatus.Added, Is.EqualTo(e.LocalContentStatus),
                        "Wrong text status on " + added);
                    Assert.That(string.Compare(added, e.Path, true) == 0, "Added filenames don't match");
                });


            Client.Status(changed, a,
                delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(SvnStatus.Modified, Is.EqualTo(e.LocalContentStatus),
                        "Wrong text status " + changed);
                    Assert.That(string.Compare(changed, e.Path, true) == 0, "Changed filenames don't match");
                });

            Client.Status(propChange, a,
                delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(SvnStatus.Modified, Is.EqualTo(e.LocalPropertyStatus),
                        "Wrong property status " + propChange);
                    Assert.That(string.Compare(propChange, e.Path, true) == 0, "Propchanged filenames don't match");
                });

            a.RetrieveAllEntries = true;

            Client.Status(ignored, a,
                delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Ignored),
                        "Wrong content status " + ignored);
                });
        }


        [TestMethod]
        public void TestEntry()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string form = Path.Combine(WcPath, "Form.cs");
            this.RunCommand("svn", "lock " + form);

            string output = this.RunCommand("svn", "info " + form);

            Collection<SvnStatusEventArgs> statuses;

            Client.GetStatus(form, out statuses);

            //Status s = SingleStatus(client, form);

            SvnStatusArgs a = new SvnStatusArgs();
            this.Client.Status(form, a, delegate (object sender, SvnStatusEventArgs e)
            { });
        }

        [TestMethod]
        public void CheckRemoteStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);
            string WcPath = sbox.Wc;

            string dir = WcPath;

            SvnStatusArgs sa = new SvnStatusArgs();
            sa.RetrieveRemoteStatus = true;

            Collection<SvnStatusEventArgs> r;
            Client.GetStatus(dir, sa, out r);
        }

        [TestMethod]
        public void CheckLocalRemoteStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);
            string WcPath = sbox.Wc;

            string dir = WcPath;

            SvnStatusArgs sa = new SvnStatusArgs();
            sa.RetrieveRemoteStatus = true;

            Collection<SvnStatusEventArgs> r;
            using (SvnClient c = new SvnClient())
            {
                c.GetStatus(dir, sa, out r);
            }
        }

        /// <summary>
        /// Tests an update where we contact the repository
        /// </summary>
        /*[TestMethod]
        public void TestRepositoryStatus()
        {
            // modify the file in another working copy and commit
            string wc2 = this.FindDirName(Path.Combine(this.WcPath, "wc2"));
            try
            {
                Zip.ExtractZipResource(wc2, this.GetType(), WC_FILE);
                this.RenameAdminDirs(wc2);
                using (StreamWriter w = new StreamWriter(
                                   Path.Combine(wc2, "Form.cs"), true))
                    w.Write("Hell worl");
                this.RunCommand("svn", "ci -m \"\" " + wc2);

                // find the status in our own wc
                int youngest;
                string form = Path.Combine(this.WcPath, "Form.cs");
                this.Client.Status(out youngest,
                    form, Revision.Head, new StatusCallback(this.StatusFunc),
                    false, false, true, true);

                Assert.AreEqual(e.RepositoryTextStatus,
                    SvnStatus.Modified, "Wrong status");
            }
            finally
            {
                PathUtils.RecursiveDelete(wc2);
            }
        }*/

        SvnStatusEventArgs SingleStatus(SvnClient client, string path)
        {
            SvnStatusArgs sa = new SvnStatusArgs();
            sa.Depth = SvnDepth.Empty;
            sa.RetrieveAllEntries = true;

            Collection<SvnStatusEventArgs> rslt;
            client.GetStatus(path, sa, out rslt);

            if (rslt.Count != 1)
                return null;

            SvnStatusEventArgs r = rslt[0];

            if (!r.Versioned
                && !r.Conflicted
                && r.LocalNodeStatus == SvnStatus.None
                && r.LocalTextStatus == SvnStatus.None
                && r.LocalPropertyStatus == SvnStatus.None)
            {
                return null;
            }

            return r;
        }

        [TestMethod]
        public void TestSingleStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string unversioned = CreateTextFile(WcPath, "unversioned.cs");
            string added = CreateTextFile(WcPath, "added.cs");
            Client.Add(added);

            string changed = CreateTextFile(WcPath, "Form.cs");

            string propChange = Path.Combine(WcPath, "App.ico");

            SvnStatusEventArgs status = SingleStatus(Client, unversioned);
            Assert.That(status.LocalContentStatus, Is.EqualTo(SvnStatus.NotVersioned),
                    "Wrong text status on " + unversioned);

            status = SingleStatus(Client, added);
            Assert.That(status.LocalContentStatus, Is.EqualTo(SvnStatus.Added),
                    "Wrong text status on " + added);

            status = SingleStatus(Client, changed);
            Assert.That(status.LocalContentStatus, Is.EqualTo(SvnStatus.Modified),
                    "Wrong text status " + changed);

            this.RunCommand("svn", "ps foo bar " + propChange);
            status = SingleStatus(Client, propChange);
            Assert.AreEqual(
                    SvnStatus.Modified, status.LocalPropertyStatus,
                    "Wrong property status " + propChange);
        }

        [TestMethod]
        public void TestSingleStatusNonExistentPath()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;

            string doesntExist = Path.Combine(WcPath, "doesnt.exist");
            SvnStatusEventArgs status = SingleStatus(Client, doesntExist);
            Assert.That(status, Is.Null);
        }

        /*[TestMethod]
        public void TestSingleStatusUnversionedPath()
        {
            string dir = Path.Combine(this.WcPath, "Unversioned");
            string file = Path.Combine(dir, "file.txt");
            SvnStatusEventArgs status = SingleStatus(Client, file);
            Assert.That( status, Is.EqualTo(SvnStatusEventArgs.None));

        }*/

        [TestMethod]
        public void TestSingleStatusNodeKind()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string file = Path.Combine(WcPath, "Form.cs");
            Assert.That(SingleStatus(Client, file).NodeKind, Is.EqualTo(SvnNodeKind.File));
            Assert.That(Path.GetFileName(SingleStatus(Client, file).Path), Is.EqualTo("Form.cs"));

            SvnStatusEventArgs dir = SingleStatus(Client, WcPath);
            Assert.That(SingleStatus(Client, WcPath).NodeKind, Is.EqualTo(SvnNodeKind.Directory));
        }



        [TestMethod]
        public void LockSingleStatusIsNullForUnlocked()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string form = Path.Combine(WcPath, "Form.cs");
            SvnStatusEventArgs status1 = SingleStatus(Client, form);
            Assert.IsNull(status1.LocalLock);
        }

        [TestMethod]
        public void LocalLockSingleStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string form = Path.Combine(WcPath, "Form.cs");
            this.RunCommand("svn", "lock -m test " + form);

            SvnStatusEventArgs s = SingleStatus(Client, form);
            Assert.That(s.LocalLock, Is.Not.Null);
            Assert.That(s.LocalLock.Owner, Is.EqualTo(Environment.UserName));
            Assert.That(s.LocalLock.CreationTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
            Assert.That(s.LocalLock.Comment, Is.EqualTo("test"));
        }

        [TestMethod]
        public void LocalLockStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string form = Path.Combine(WcPath, "Form.cs");
            this.RunCommand("svn", "lock -m test " + form);

            SvnStatusArgs sa = new SvnStatusArgs();

            this.Client.Status(form, delegate (object sender, SvnStatusEventArgs e)
            {
                Assert.IsNotNull(e.LocalLock);
                Assert.That(e.LocalLock.Token, Is.Not.Null);
                Assert.That(e.LocalLock.Owner, Is.EqualTo(Environment.UserName));
                Assert.That(e.LocalLock.CreationTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
                Assert.That(e.LocalLock.Comment, Is.EqualTo("test"));
            });
        }

        [TestMethod]
        public void WcStatusTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;

            using (SvnClient client = NewSvnClient(false, false))
            {
                string file = Path.Combine(WcPath, "WcStatusTest");
                TouchFile(file);

                client.Add(file);

                Collection<SvnStatusEventArgs> items;
                client.GetStatus(WcPath, out items);

                Assert.That(items, Is.Not.Null, "Status retrieved");
                Assert.That(items.Count, Is.GreaterThan(0), "More than 0 items");
            }
        }

        [TestMethod, Obsolete]
        [Ignore]
        public void MoreStatusTests_WorkingCopyInfo()
        {
            // This method is marked Obsolete as it uses the Obsolete 'SvnClientStatusEventArgs.WorkingCopyInfo' property
            // The new variant uses the new properties on the status object itself.
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases, false);
            string wcPath = sbox.Wc;
            Uri reposUrl = sbox.RepositoryUri;

            using (SvnClient client = NewSvnClient(true, false))
            {
                client.Update(wcPath);
                client.CreateDirectory(Path.Combine(wcPath, "statTst"));
                string local = Path.Combine(wcPath, "statTst/LocalStatBase1");
                string local2 = Path.Combine(wcPath, "statTst/LocalStatBase2");
                string local3 = Path.Combine(wcPath, "statTst/LocalStatBase3");
                string local4 = Path.Combine(wcPath, "statTst/LocalStatBase4");
                string local5 = Path.Combine(wcPath, "statTst/LocalStatBase5");
                string local6 = Path.Combine(wcPath, "statTst/LocalStatBase6");

                SvnCommitResult ci, ci9;

                Touch2(local);
                client.Add(local);
                client.Commit(wcPath, out ci);

                client.Copy(local, local2);
                client.Commit(wcPath);

                client.SetProperty(local, "test-q", "value");

                client.Copy(local, local3);
                client.Copy(local, local6);
                client.Copy(local, local5);
                client.Commit(wcPath, out ci9);

                client.Copy(local, local4);
                client.Delete(local5);

                client.SetProperty(local, "test-q", "value2");
                client.RemoteDelete(new Uri(reposUrl, "statTst/LocalStatBase2"));
                client.AddToChangeList(local6, "MyList");
                Touch2(local6);

                Guid reposGuid;

                client.TryGetRepositoryId(reposUrl, out reposGuid);

                for (int mode = 0; mode < 4; mode++)
                {
                    SvnStatusArgs a = new SvnStatusArgs();

                    a.RetrieveRemoteStatus = mode % 2 == 1;
                    a.RetrieveAllEntries = mode > 1;

                    int n = 0;

                    client.Status(Path.Combine(wcPath, "statTst"), a, delegate (object sender, SvnStatusEventArgs e)
                    {
                        n++;
                        string p = e.Path;
                        int nn = -1;
                        switch (e.Path[e.Path.Length - 1])
                        {
                            case '1':
                                nn = 1;
                                break;
                            case '2':
                                nn = 2;
                                break;
                            case '3':
                                nn = 3;
                                break;
                            case '4':
                                nn = 4;
                                break;
                            case '5':
                                nn = 5;
                                break;
                            case '6':
                                nn = 6;
                                break;
                            default:
                                Assert.That(e.FullPath, Is.EqualTo(Path.GetDirectoryName(local)));
                                break;
                        }

                        if (nn >= 0)
                            Assert.That(Path.GetDirectoryName(e.FullPath), Is.EqualTo(Path.Combine(wcPath, "statTst")));
                        Assert.That(Path.GetFileName(e.Path), Is.EqualTo(Path.GetFileName(e.FullPath)));

                        switch (nn)
                        {
                            case 1:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Modified));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Modified));
                                break;
                            case 2:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            case 3:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                                break;
                            case 4:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Added));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                                break;
                            case 5:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Deleted));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            case 6:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Modified));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Modified));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                                break;
                            default:
                                break;
                        }

                        Assert.That(e.LocalCopied, Is.EqualTo(nn == 4));
                        Assert.That(e.Wedged, Is.False);
                        if (a.RetrieveRemoteStatus)
                        {
                            Assert.That(e.RemoteContentStatus, Is.EqualTo(nn == 2 ? SvnStatus.Deleted : SvnStatus.None));
                        }
                        else
                        {
                            Assert.That(e.RemoteContentStatus, Is.EqualTo(SvnStatus.None));
                        }

                        Assert.That(e.RemoteLock, Is.Null);
                        Assert.That(e.RemotePropertyStatus, Is.EqualTo(SvnStatus.None));

                        if (nn == 2 && a.RetrieveRemoteStatus)
                        {
                            Assert.That(e.RemoteUpdateCommitAuthor, Is.Null);
                            Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.File));
                            Assert.That(e.RemoteUpdateCommitTime, Is.EqualTo(DateTime.MinValue));
                            Assert.That(e.RemoteUpdateRevision, Is.EqualTo(ci.Revision + 3));
                        }
                        else if (nn == -1 && a.RetrieveRemoteStatus)
                        {
                            Assert.That(e.RemoteUpdateCommitAuthor, Is.EqualTo(Environment.UserName));
                            Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.Directory));
                            Assert.That(e.RemoteUpdateCommitTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 45, 0)));
                            Assert.That(e.RemoteUpdateRevision, Is.EqualTo(ci.Revision + 3));
                        }
                        else
                        {
                            Assert.That(e.RemoteUpdateCommitAuthor, Is.Null);
                            Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.None));
                            Assert.That(e.RemoteUpdateCommitTime, Is.EqualTo(DateTime.MinValue));
                            Assert.That(e.RemoteUpdateRevision, Is.LessThan(0L));
                        }
                        Assert.That(e.Switched, Is.False);
                        if (nn >= 0)
                            Assert.That(e.Uri, Is.EqualTo(new Uri(reposUrl, "statTst/localStatBase" + nn.ToString())));
                        else
                            Assert.That(e.Uri, Is.EqualTo(new Uri(reposUrl, "statTst/")));


                        Assert.That(e.WorkingCopyInfo, Is.Not.Null);

                        if (nn >= 0)
                            Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(Path.GetFileName(e.Path)));
                        else
                            Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(""));

                        Assert.That(e.WorkingCopyInfo.ChangeList, Is.EqualTo((nn == 6) ? "MyList" : null));
                        if (nn >= 0)
                            Assert.That(e.WorkingCopyInfo.Checksum, Is.Not.Null);

                        Assert.That(e.WorkingCopyInfo.Uri, Is.EqualTo(e.Uri));
                        Assert.That(e.WorkingCopyInfo.RepositoryUri, Is.EqualTo(reposUrl));
                        /*if(a.ContactRepository)
                            Assert.That(e.WorkingCopyInfo.RepositoryId, Is.EqualTo(reposGuid));
                        else*/

                        Assert.That(e.WorkingCopyInfo.RepositoryId, Is.Not.EqualTo(Guid.Empty), "Expected guid for {0} / nn={1}", e.Path, nn);

                        Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                        Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                        Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);

                        //
                        Assert.That(e.WorkingCopyInfo.ConflictNewFile, Is.Null);
                        Assert.That(e.WorkingCopyInfo.ConflictOldFile, Is.Null);
                        Assert.That(e.WorkingCopyInfo.ConflictWorkFile, Is.Null);

                        if (nn == 4)
                        {
                            Assert.That(e.WorkingCopyInfo.IsCopy, Is.True);
                            Assert.That(e.WorkingCopyInfo.CopiedFrom, Is.EqualTo(new Uri(reposUrl, "statTst/localStatBase1")));
                            Assert.That(e.WorkingCopyInfo.LastChangeAuthor, Is.Not.Null);
                            Assert.That(e.WorkingCopyInfo.CopiedFromRevision, Is.EqualTo(ci.Revision + 2));
                        }
                        else
                        {
                            Assert.That(e.WorkingCopyInfo.IsCopy, Is.False);
                            Assert.That(e.WorkingCopyInfo.CopiedFrom, Is.Null);
                            Assert.That(e.WorkingCopyInfo.LastChangeAuthor, Is.EqualTo(Environment.UserName));
                            Assert.That(e.WorkingCopyInfo.CopiedFromRevision, Is.EqualTo(-1L));
                        }

                        Assert.That(e.WorkingCopyInfo.Depth, Is.EqualTo(SvnDepth.Infinity));
                        Assert.That(e.WorkingCopyInfo.HasProperties, Is.EqualTo(nn >= 0 && nn != 2));
                        Assert.That(e.WorkingCopyInfo.HasPropertyChanges, Is.EqualTo(nn == 1), "Expected property changes (nn={0})", nn);
                        Assert.That(e.WorkingCopyInfo.KeepLocal, Is.False);

                        if (nn == 4)
                        {
                            Assert.That(e.WorkingCopyInfo.LastChangeRevision, Is.EqualTo(9));
                            Assert.That(e.WorkingCopyInfo.LastChangeTime, Is.GreaterThan(DateTime.MinValue));
                            Assert.That(e.WorkingCopyInfo.Schedule, Is.EqualTo(SvnSchedule.Add));
                            Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.GreaterThan(DateTime.MinValue));
                            Assert.That(e.WorkingCopyInfo.Revision, Is.EqualTo(ci.Revision));
                            Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(36));
                        }
                        else
                        {
                            switch (nn)
                            {
                                case 2:
                                    Assert.That(e.WorkingCopyInfo.LastChangeRevision, Is.EqualTo(ci.Revision + 1));
                                    Assert.That(e.WorkingCopyInfo.Revision, Is.EqualTo(ci.Revision + 1));
                                    break;
                                default:
                                    Assert.That(e.WorkingCopyInfo.LastChangeRevision, Is.EqualTo((nn >= 0) ? (ci.Revision + 2) : ci.Revision));
                                    Assert.That(e.WorkingCopyInfo.Revision, Is.EqualTo((nn >= 0) ? (ci.Revision + 2) : ci.Revision));
                                    break;
                            }

                            Assert.That(e.WorkingCopyInfo.LastChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 45)));
                            Assert.That(e.WorkingCopyInfo.Schedule, Is.EqualTo((nn == 5) ? SvnSchedule.Delete : SvnSchedule.Normal));

                            if (nn == 5)
                            {
                                // Deleted node
                            }
                            else if (nn >= 0)
                            {
                                Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 45)));
                                Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(36L), "WCSize = 36");
                            }
                            else
                            {
                                Assert.That(e.WorkingCopyInfo.NodeKind, Is.EqualTo(SvnNodeKind.Directory));
                                Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.EqualTo(DateTime.MinValue));
                                Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(-1L), "WCSize = -1");
                            }
                        }
                        Assert.That(e.WorkingCopyInfo.LockComment, Is.Null);
                        Assert.That(e.WorkingCopyInfo.LockOwner, Is.Null);
                        Assert.That(e.WorkingCopyInfo.LockTime, Is.EqualTo(DateTime.MinValue));
                        Assert.That(e.WorkingCopyInfo.LockToken, Is.Null);
                        Assert.That(e.WorkingCopyInfo.NodeKind, Is.EqualTo(nn >= 0 ? SvnNodeKind.File : SvnNodeKind.Directory));
                        Assert.That(e.WorkingCopyInfo.PropertyRejectFile, Is.Null);
                    });

                    if (a.RetrieveAllEntries)
                        Assert.That(n, Is.EqualTo(7));
                    else if (a.RetrieveRemoteStatus)
                        Assert.That(n, Is.EqualTo(5));
                    else
                        Assert.That(n, Is.EqualTo(4));
                }

                File.Delete(local);

                client.Status(local, delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                    Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Missing));
                });
                SvnDeleteArgs da = new SvnDeleteArgs();
                da.Force = true;
                client.Delete(local, da);
                client.Status(local, delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                    Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Deleted));
                });

            }
        }

        [TestMethod]
        [Ignore]
        public void MoreStatusTests()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty, false);
            string wcPath = sbox.Wc;
            Uri reposUrl = sbox.RepositoryUri;

            using (SvnClient client = NewSvnClient(true, false))
            {
                client.Update(wcPath);
                client.CreateDirectory(Path.Combine(wcPath, "statTst"));
                string local = Path.Combine(wcPath, "statTst/LocalStatBase1");
                string local2 = Path.Combine(wcPath, "statTst/LocalStatBase2");
                string local3 = Path.Combine(wcPath, "statTst/LocalStatBase3");
                string local4 = Path.Combine(wcPath, "statTst/LocalStatBase4");
                string local5 = Path.Combine(wcPath, "statTst/LocalStatBase5");
                string local6 = Path.Combine(wcPath, "statTst/LocalStatBase6");

                SvnCommitResult ci, ci8, ci9;

                Touch2(local);
                client.Add(local);
                client.Commit(wcPath, out ci);

                client.Copy(local, local2);
                client.Commit(wcPath, out ci8);

                client.SetProperty(local, "test-q", "value");

                client.Copy(local, local3);
                client.Copy(local, local6);
                client.Copy(local, local5);
                client.Commit(wcPath, out ci9);

                client.Copy(local, local4);
                client.Delete(local5);

                client.SetProperty(local, "test-q", "value2");
                client.RemoteDelete(new Uri(reposUrl, "statTst/LocalStatBase2"));
                client.AddToChangeList(local6, "MyList");
                Touch2(local6);

                Guid reposGuid;

                client.TryGetRepositoryId(reposUrl, out reposGuid);

                for (int mode = 0; mode < 4; mode++)
                {
                    SvnStatusArgs a = new SvnStatusArgs();

                    a.RetrieveRemoteStatus = mode % 2 == 1;
                    a.RetrieveAllEntries = mode > 1;

                    int n = 0;

                    client.Status(Path.Combine(wcPath, "statTst"), a, delegate (object sender, SvnStatusEventArgs e)
                    {
                        n++;
                        string p = e.Path;
                        int nn = -1;
                        switch (e.Path[e.Path.Length - 1])
                        {
                            case '1':
                                nn = 1;
                                break;
                            case '2':
                                nn = 2;
                                break;
                            case '3':
                                nn = 3;
                                break;
                            case '4':
                                nn = 4;
                                break;
                            case '5':
                                nn = 5;
                                break;
                            case '6':
                                nn = 6;
                                break;
                            default:
                                Assert.That(e.FullPath, Is.EqualTo(Path.GetDirectoryName(local)));
                                break;
                        }

                        if (nn >= 0)
                            Assert.That(Path.GetDirectoryName(e.FullPath), Is.EqualTo(Path.Combine(wcPath, "statTst")));
                        Assert.That(Path.GetFileName(e.Path), Is.EqualTo(Path.GetFileName(e.FullPath)));

                        switch (nn)
                        {
                            case 1:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Modified));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Modified));
                                break;
                            case 2:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            case 3:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                                break;
                            case 4:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Added));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                                break;
                            case 5:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Deleted));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            case 6:
                                Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Modified));
                                Assert.That(e.LocalTextStatus, Is.EqualTo(SvnStatus.Modified));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                                break;
                            default:
                                break;
                        }

                        Assert.That(e.LocalCopied, Is.EqualTo(nn == 4));
                        Assert.That(e.Wedged, Is.False);
                        if (a.RetrieveRemoteStatus)
                        {
                            Assert.That(e.RemoteContentStatus, Is.EqualTo(nn == 2 ? SvnStatus.Deleted : SvnStatus.None));
                        }
                        else
                        {
                            Assert.That(e.RemoteContentStatus, Is.EqualTo(SvnStatus.None));
                        }

                        Assert.That(e.RemoteLock, Is.Null);
                        Assert.That(e.RemotePropertyStatus, Is.EqualTo(SvnStatus.None));

                        if (nn == 2 && a.RetrieveRemoteStatus)
                        {
                            Assert.That(e.RemoteUpdateCommitAuthor, Is.Null);
                            Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.File));
                            Assert.That(e.RemoteUpdateCommitTime, Is.EqualTo(DateTime.MinValue));
                            Assert.That(e.RemoteUpdateRevision, Is.EqualTo(ci.Revision + 3));
                        }
                        else if (nn == -1 && a.RetrieveRemoteStatus)
                        {
                            Assert.That(e.RemoteUpdateCommitAuthor, Is.EqualTo(Environment.UserName));
                            Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.Directory));
                            Assert.That(e.RemoteUpdateCommitTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 45, 0)));
                            Assert.That(e.RemoteUpdateRevision, Is.EqualTo(ci.Revision + 3));
                        }
                        else
                        {
                            Assert.That(e.RemoteUpdateCommitAuthor, Is.Null);
                            Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.None));
                            Assert.That(e.RemoteUpdateCommitTime, Is.EqualTo(DateTime.MinValue));
                            Assert.That(e.RemoteUpdateRevision, Is.LessThan(0L));
                        }
                        Assert.That(e.Switched, Is.False);
                        if (nn >= 0)
                            Assert.That(e.Uri, Is.EqualTo(new Uri(reposUrl, "statTst/localStatBase" + nn.ToString())));
                        else
                            Assert.That(e.Uri, Is.EqualTo(new Uri(reposUrl, "statTst/")));


                        Assert.That(e.WorkingCopyInfo, Is.Not.Null);

                        Assert.That(e.ChangeList, Is.EqualTo((nn == 6) ? "MyList" : null));
                        if (nn >= 0)
                            Assert.That(e.WorkingCopyInfo.Checksum, Is.Not.Null);

                        Assert.That(e.RepositoryId, Is.EqualTo(reposGuid), "Expected guid for {0} / nn={1}", e.Path, nn);

                        Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                        Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                        Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);

                        //
                        Assert.That(e.WorkingCopyInfo.ConflictNewFile, Is.Null);
                        Assert.That(e.WorkingCopyInfo.ConflictOldFile, Is.Null);
                        Assert.That(e.WorkingCopyInfo.ConflictWorkFile, Is.Null);

                        if (nn == 4)
                        {
                            Assert.That(e.LocalCopied, Is.True);
                            Assert.That(e.WorkingCopyInfo.CopiedFrom, Is.EqualTo(new Uri(reposUrl, "statTst/localStatBase1")));
                            Assert.That(e.LastChangeAuthor, Is.Not.Null);
                            Assert.That(e.WorkingCopyInfo.CopiedFromRevision, Is.EqualTo(ci.Revision + 2));
                        }
                        else if (nn == 5)
                        {
                            Assert.That(e.LocalCopied, Is.False);
                            Assert.That(e.LastChangeAuthor, Is.Null);
                        }
                        else
                        {
                            Assert.That(e.LocalCopied, Is.False);
                            Assert.That(e.LastChangeAuthor, Is.EqualTo(Environment.UserName));
                            Assert.That(e.WorkingCopyInfo.CopiedFromRevision, Is.EqualTo(-1L));
                        }

                        if (e.NodeKind != SvnNodeKind.Directory)
                            Assert.That(e.Depth, Is.EqualTo(SvnDepth.Unknown));
                        else
                            Assert.That(e.Depth, Is.EqualTo(SvnDepth.Infinity));

                        if (nn == 1)
                            Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Modified), "Expected property changes (nn={0})", nn);
                        else if (nn >= 0 && nn != 2 && nn != 5)
                            Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                        else
                            Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));

                        if (nn == 4)
                        {
                            Assert.That(e.LastChangeRevision, Is.EqualTo(ci9.Revision));
                            Assert.That(e.LastChangeTime, Is.EqualTo(ci9.Time));
                            Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Added));
                            Assert.That(e.Revision, Is.EqualTo(-1L));
                            Assert.That(e.FileLength, Is.EqualTo(36));
                        }
                        else
                        {
                            switch (nn)
                            {
                                case 2:
                                    Assert.That(e.LastChangeRevision, Is.EqualTo(ci8.Revision));
                                    Assert.That(e.LastChangeAuthor, Is.EqualTo(ci8.Author));
                                    Assert.That(e.LastChangeTime, Is.EqualTo(ci8.Time));
                                    Assert.That(e.Revision, Is.EqualTo(ci8.Revision));
                                    Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Normal));
                                    break;
                                case 5: // deleted
                                    Assert.That(e.LastChangeRevision, Is.EqualTo(-1L));
                                    Assert.That(e.Revision, Is.EqualTo(-1L));
                                    Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Deleted));
                                    break;
                                default:
                                    Assert.That(e.LastChangeRevision, Is.EqualTo((nn >= 0) ? (ci9.Revision) : ci.Revision));
                                    Assert.That(e.Revision, Is.EqualTo((nn >= 0) ? (ci9.Revision) : ci.Revision));
                                    Assert.That(e.LastChangeAuthor, Is.EqualTo((nn >= 0) ? (ci9.Author) : ci.Author));
                                    Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Normal).Or.EqualTo(SvnStatus.Modified));
                                    break;
                            }

                            if (nn == 5)
                            {
                                // Deleted node
                            }
                            else if (nn >= 0)
                            {
                                Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 45)));
                                Assert.That(e.FileLength, Is.EqualTo(36L), "WCSize = 36");
                            }
                            else
                            {
                                Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.Directory));
                                Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.EqualTo(DateTime.MinValue));
                                Assert.That(e.FileLength, Is.EqualTo(-1L), "WCSize = -1");
                            }
                        }
                        Assert.That(e.IsFileExternal, Is.False);
                        Assert.That(e.LocalLock, Is.Null);
                        Assert.That(e.NodeKind, Is.EqualTo(nn >= 0 ? SvnNodeKind.File : SvnNodeKind.Directory));
                        Assert.That(e.Conflicted, Is.False);
                    });

                    if (a.RetrieveAllEntries)
                        Assert.That(n, Is.EqualTo(7));
                    else if (a.RetrieveRemoteStatus)
                        Assert.That(n, Is.EqualTo(5));
                    else
                        Assert.That(n, Is.EqualTo(4));
                }

                File.Delete(local);

                client.Status(local, delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                    Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Missing));
                });
                SvnDeleteArgs da = new SvnDeleteArgs();
                da.Force = true;
                client.Delete(local, da);
                client.Status(local, delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                    Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Deleted));
                });

            }
        }

        [TestMethod]
        public void StatusDelayTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            string tmp1Dir = sbox.GetTempDir();
            string tmp2Dir = sbox.GetTempDir();
            string tmp3Dir = sbox.GetTempDir();

            Uri emptyUri = sbox.CreateRepository(SandBoxRepository.Empty);

            Client.CheckOut(emptyUri, tmp1Dir);
            Client.CheckOut(emptyUri, tmp2Dir);
            Client.CheckOut(emptyUri, tmp3Dir);

            StringBuilder lotOfData = new StringBuilder();
            lotOfData.Append('Y', 1024 * 1024);

            string file1 = Path.Combine(tmp1Dir, "File-1");
            string file2 = Path.Combine(tmp2Dir, "File-2");
            string file3 = Path.Combine(tmp3Dir, "File-3");

            File.WriteAllText(file1, lotOfData.ToString());
            File.WriteAllText(file2, lotOfData.ToString());
            File.WriteAllText(file3, lotOfData.ToString());

            Client.Add(file1);
            Client.Add(file2);
            Client.Add(file3);

            Client.Configuration.LogMessageRequired = false;
            Client.Commit(file1);
            Client.Commit(file2);
            Client.Commit(file3);

            lotOfData = new StringBuilder();
            lotOfData.Append('Y', 512 * 1024);
            lotOfData.Append('Z', 512 * 1024);

            File.WriteAllText(file1, lotOfData.ToString());
            File.WriteAllText(file2, lotOfData.ToString());
            File.WriteAllText(file3, lotOfData.ToString());

            using (FileStream fs2 = new FileStream(file2, FileMode.Open, FileAccess.ReadWrite, FileShare.None))
            using (FileStream fs3 = new FileStream(file3, FileMode.Open, FileAccess.ReadWrite, FileShare.None))
            {
                SvnStatusArgs sa = new SvnStatusArgs();
                sa.ThrowOnError = false;

                TimeSpan t1, t2, t3;

                DateTime start = DateTime.Now;
                Client.Status(file1, sa, null);
                t1 = DateTime.Now - start;
                Assert.That(sa.LastException, Is.Null);


                using (new SharpSvn.Implementation.SvnFsOperationRetryOverride(0))
                {
                    start = DateTime.Now;
                    Client.Status(file2, sa, null);
                    t2 = DateTime.Now - start;
                    //Assert.That(sa.LastException, Is.Not.Null); // Fails in 1.5-1.6.4
                }

                start = DateTime.Now;
                Client.Status(file3, sa, null);
                t3 = DateTime.Now - start;
                //Assert.That(sa.LastException, Is.Not.Null); // Fails in 1.5-1.6.4

                Assert.That(t3, Is.GreaterThan(t2 + new TimeSpan(0, 0, 2)));

                System.Diagnostics.Trace.WriteLine(string.Format("t1={0}, t2={1}, t3={2}", t1, t2, t3));
            }
        }

        class MyFsOverride : SharpSvn.Implementation.SvnFsOperationRetryOverride
        {
            public MyFsOverride()
                : base(2)
            {
            }
            protected override bool OnRetryLoopInvocation(int nr, SvnAprErrorCode error, SvnWindowsErrorCode osError, string expr)
            {
                Assert.That(osError, Is.Not.EqualTo(0));
                Assert.That(expr, Is.Not.Null);
                Assert.That(expr.Contains("apr_"), "'{0}' contains apr_");
                Assert.That(nr, Is.GreaterThan(1));

                return base.OnRetryLoopInvocation(nr, error, osError, expr);
            }
        }

        [TestMethod]
        public void CheckOverrideWorking()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string tmp2Dir = sbox.Wc;

            StringBuilder lotOfData = new StringBuilder();
            lotOfData.Append('Y', 1024 * 1024);

            string file2 = Path.Combine(tmp2Dir, "File-2");

            File.WriteAllText(file2, lotOfData.ToString());

            Client.Add(file2);

            Client.Configuration.LogMessageRequired = false;
            Client.Commit(file2);

            lotOfData = new StringBuilder();
            lotOfData.Append('Y', 512 * 1024);
            lotOfData.Append('Z', 512 * 1024);

            File.WriteAllText(file2, lotOfData.ToString());

            using (FileStream fs2 = new FileStream(file2, FileMode.Open, FileAccess.ReadWrite, FileShare.None))
            {
                SvnStatusArgs sa = new SvnStatusArgs();
                sa.ThrowOnError = false;

                TimeSpan t2;
                DateTime start;

                using (new MyFsOverride())
                {
                    start = DateTime.Now;
                    Client.Status(file2, sa, null);
                    t2 = DateTime.Now - start;
                    //Assert.That(sa.LastException, Is.Not.Null); // Fails in 1.5-1.6.4
                }
            }
        }

        [TestMethod]
        public void RepositoryLockStatus()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string form = Path.Combine(WcPath, "Form.cs");
            Client.Lock(form, "test");

            SvnStatusArgs sa = new SvnStatusArgs() { RetrieveRemoteStatus = true };
            this.Client.Status(form, sa,
                delegate (object sender, SvnStatusEventArgs e)
                {
                    Assert.IsNotNull(e.RemoteLock);
                    Assert.That(e.RemoteLock.Owner, Is.EqualTo(Environment.UserName));
                    Assert.That(DateTime.Now.Date, Is.EqualTo(e.RemoteLock.CreationTime.ToLocalTime().Date));
                    Assert.That(e.RemoteLock.Comment, Is.EqualTo("test"));
                    Assert.IsFalse(e.RemoteLock.IsRawNetworkComment);
                });
        }

        [TestMethod]
        public void TestStatusResult()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            Uri repos = sbox.CreateRepository(SandBoxRepository.Empty);
            string dir = sbox.GetTempDir();

            SvnStatusArgs sa = new SvnStatusArgs { Depth = SvnDepth.Children, ThrowOnError = false };

            Assert.That(Client.Status(dir, sa, null), Is.False);
            Assert.That(sa.LastException, Is.Not.Null);
            Assert.That(sa.LastException.SvnErrorCode, Is.EqualTo(SvnErrorCode.SVN_ERR_WC_NOT_WORKING_COPY));

            Client.CheckOut(repos, dir);
            Assert.That(Client.Status(dir, sa, null), Is.True);
            Assert.That(sa.LastException, Is.Null);

            Assert.That(Client.Status(Path.Combine(dir, "subdir"), sa, null), Is.True);
            Assert.That(sa.LastException, Is.Null);

            Assert.That(Client.Status(Path.Combine(dir, "subdir\\subsubdir"), sa, null), Is.False);
            Assert.That(sa.LastException.SvnErrorCode, Is.EqualTo(SvnErrorCode.SVN_ERR_WC_PATH_NOT_FOUND));
        }
    }
}
