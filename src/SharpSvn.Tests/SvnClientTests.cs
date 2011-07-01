// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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
using System.Collections.Generic;
using System.IO;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class SvnClientTests : Commands.TestBase
    {

		string _testPath;
		public string TestPath
		{
			get
			{
				if (_testPath == null)
				{
					_testPath = Path.Combine(Path.GetTempPath(), "SvnTest\\" + GetType().FullName);

					if (!Directory.Exists(_testPath))
						Directory.CreateDirectory(_testPath);
				}
				return _testPath;
			}
		}

        string _repos;
        string _wc;
        string RepositoryPath
        {
            get
            {
                if (_repos == null)
                    _repos = Path.Combine(TestPath, "repos");

                return _repos;
            }
        }

        Uri _reposUri;
        Uri ReposUri
        {
            get
            {
                if (_reposUri == null)
                    _reposUri = new Uri("file:///" + RepositoryPath.Replace(Path.DirectorySeparatorChar, '/') + "/");

                return _reposUri;
            }
        }

        Uri WcUri
        {
            get
            {
                return new Uri(ReposUri, "folder/");
            }
        }

        new string WcPath
        {
            get
            {
                if (_wc == null)
                    _wc = Path.Combine(TestPath, "wc");

                return _wc;
            }
        }

        [TestFixtureSetUp]
        public void SetupRepository()
        {
            if (Directory.Exists(RepositoryPath))
            {
                ForcedDeleteDirectory(RepositoryPath);
            }
            if (Directory.Exists(WcPath))
            {
                ForcedDeleteDirectory(WcPath);
            }
            Directory.CreateDirectory(WcPath);

            using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
            {
                SvnCreateRepositoryArgs cra = new SvnCreateRepositoryArgs();
                cra.RepositoryCompatibility = SvnRepositoryCompatibility.Default;
                reposClient.CreateRepository(RepositoryPath, cra);
            }

            using (SvnClient client = new SvnClient())
            {
                client.Configuration.LogMessageRequired = false;

                client.RemoteCreateDirectory(new Uri(ReposUri, "folder"));

                client.CheckOut(new Uri(ReposUri, "folder"), WcPath);
            }
        }

        [TestFixtureTearDown]
        public void DestroyRepository()
        {
#if !DEBUG
			using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
			{
				reposClient.DeleteRepository(RepositoryPath);
			}
#endif
        }

        [Test]
        public void SomeGlobalTests()
        {
            using (SvnClient client = NewSvnClient(false, false))
            {
                Assert.That(SvnClient.AdministrativeDirectoryName, Is.EqualTo(".svn"));
                Assert.That(SvnClient.Version, Is.GreaterThanOrEqualTo(new Version(1, 5, 0)));
                Assert.That(SvnClient.SharpSvnVersion, Is.GreaterThan(new Version(1, 5, 0)));
                Assert.That(client.GetRepositoryRoot(ReposUri), Is.EqualTo(ReposUri));
                Assert.That(client.GetRepositoryRoot(WcPath), Is.EqualTo(ReposUri));
            }
        }

        [Test]
        public void TestChangeLists()
        {
            string file1 = Path.Combine(WcPath, "ChangeListFile1");
            string file2 = Path.Combine(WcPath, "ChangeListFile2");
            string file3 = Path.Combine(WcPath, "ChangeListFile3");
            string file4 = Path.Combine(WcPath, "ChangeListFile4");
            using (SvnClient client = NewSvnClient(true, false))
            {
                SvnInfoArgs ia = new SvnInfoArgs();
                ia.ThrowOnError = false;
                SvnInfoEventArgs iea;
                Collection<SvnInfoEventArgs> ee;
                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile1"), ia, out ee), Is.False);
                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile2"), ia, out ee), Is.False);
                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile3"), ia, out ee), Is.False);
                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile4"), ia, out ee), Is.False);

                TouchFile(file1);
                TouchFile(file2);
                TouchFile(file3);
                TouchFile(file4);

                client.Add(file1);
                client.Commit(WcPath);

                client.GetInfo(new Uri(WcUri, "ChangeListFile1"), out iea);
                Assert.That(iea, Is.Not.Null);

                client.Add(file2);
                client.Add(file3);
                client.Add(file4);

                client.AddToChangeList(file2, "ChangeListTest-2");
                client.AddToChangeList(file3, "ChangeListTest-3");

                Collection<SvnListChangeListEventArgs> paths;
                SvnListChangeListArgs a = new SvnListChangeListArgs();
                a.ChangeLists.Add("ChangeListTest-2");

                client.GetChangeList(WcPath, a, out paths);

                Assert.That(paths, Is.Not.Null);
                Assert.That(paths.Count, Is.EqualTo(1));
                Assert.That(paths[0].Path, Is.EqualTo(Path.GetFullPath(file2)));

                a = new SvnListChangeListArgs();
                a.ChangeLists.Add("ChangeListTest-4");
                client.GetChangeList(WcPath, a, out paths);

                Assert.That(paths, Is.Not.Null);
                Assert.That(paths.Count, Is.EqualTo(0));

                SvnCommitArgs ca = new SvnCommitArgs();
                ca.ChangeLists.Add("ChangeListTest-2");

                client.Commit(WcPath, ca);

                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile2"), ia, out ee), Is.True);
                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile3"), ia, out ee), Is.False);
                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile4"), ia, out ee), Is.False);

                bool visited = false;
                a = new SvnListChangeListArgs();
                a.ChangeLists.Add("ChangeListTest-3");
                client.ListChangeList(WcPath, a, delegate(object sender, SvnListChangeListEventArgs e)
                {
                    Assert.That(e.Path, Is.EqualTo(file3));
                    visited = true;
                });
                Assert.That(visited, Is.True);

                visited = false;
                client.Status(file4, delegate(object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.FullPath, Is.EqualTo(file4));
                    Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Added));
                    Assert.That(e.IsRemoteUpdated, Is.False);
                    Assert.That(e.Path, Is.EqualTo(file4));
                    visited = true;
                });
                Assert.That(visited, Is.True);

                client.Commit(WcPath);

                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile3"), ia, out ee), Is.True);
                Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile4"), ia, out ee), Is.True);
            }
        }

        [Test]
        public void CopyTest()
        {
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
                            Assert.That(i.Path.EndsWith("folder/RemoteCopyBase"), "Path ends with folder/RemoteCopyBase");
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.CopyFromPath.EndsWith("folder/CopyBase"), "CopyFromPath ends with folder/CopyBase");
                            Assert.That(i.CopyFromRevision, Is.GreaterThan(0L));
                            Assert.That(i.NodeKind, Is.EqualTo(SvnNodeKind.File));
                        }
                    }
                    else
                    {
                        foreach (SvnChangeItem i in e.ChangedPaths)
                        {
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.Path.EndsWith("folder/CopyBase"), "Path ends with folder/CopyBase");
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
                            Assert.That(i.Path.EndsWith("folder/LocalCopy"), "Path ends with folder/LocalCopy");
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.CopyFromPath.EndsWith("folder/CopyBase"), "CopyFromPath ensd with folder/CopyBase");
                            Assert.That(i.CopyFromRevision, Is.GreaterThan(0L));
                        }
                        first = false;
                    }
                    else
                        foreach (SvnChangeItem i in e.ChangedPaths)
                        {
                            Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
                            Assert.That(i.Path.EndsWith("folder/CopyBase"), "Path ends with folder/CopyBase");
                            visited = true;
                        }

                });
                Assert.That(visited, "Visited local log item");
            }
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

        [Test]
        public void DeleteTest()
        {
            using (SvnClient client = NewSvnClient(true, false))
            {
                string local = Path.Combine(WcPath, "LocalDeleteBase");
                string remote = Path.Combine(WcPath, "RemoteDeleteBase");

                TouchFile(local);
                client.Add(local);
                TouchFile(remote);
                client.Add(remote);

                SvnCommitResult ci;
                client.Commit(WcPath, out ci);

                Assert.That(ItemExists(new Uri(WcUri, "LocalDeleteBase")), Is.True, "Remote base does exist");
                Assert.That(ItemExists(new Uri(WcUri, "RemoteDeleteBase")), Is.True, "Local base does exist");

                client.Delete(local);
                client.RemoteDelete(new Uri(WcUri, "RemoteDeleteBase"));

                bool visited = false;
                int n = 0;
                client.Status(local, delegate(object sender, SvnStatusEventArgs e)
                {
                    Assert.That(n++, Is.EqualTo(0));
                    Assert.That(e.FullPath, Is.EqualTo(local));
                    Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Deleted));
                    Assert.That(e.Switched, Is.False);
                    Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
                    Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "LocalDeleteBase")));
                    Assert.That(e.IsRemoteUpdated, Is.EqualTo(false));
                    Assert.That(e.LocalNodeStatus, Is.EqualTo(SvnStatus.Deleted));
                    Assert.That(e.LocalCopied, Is.EqualTo(false));
                    Assert.That(e.Wedged, Is.EqualTo(false));
                    Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                    Assert.That(e.Path, Is.EqualTo(local));
                    Assert.That(e.RemoteNodeStatus, Is.EqualTo(SvnStatus.None));
                    Assert.That(e.RemoteLock, Is.Null);
                    Assert.That(e.RemotePropertyStatus, Is.EqualTo(SvnStatus.None));
                    Assert.That(e.RemoteUpdateCommitAuthor, Is.Null);
                    Assert.That(e.RemoteUpdateCommitTime, Is.EqualTo(DateTime.MinValue));
                    Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.None));
                    Assert.That(e.RemoteUpdateRevision, Is.EqualTo(-1L));
                    Assert.That(e.Switched, Is.EqualTo(false));

#pragma warning disable 0618
                    Assert.That(e.WorkingCopyInfo, Is.Not.Null);
                    Assert.That(e.WorkingCopyInfo.ChangeList, Is.Null);
                    Assert.That(e.WorkingCopyInfo.Checksum, Is.Not.Null);
                    Assert.That(e.WorkingCopyInfo.ConflictNewFile, Is.Null);
                    Assert.That(e.WorkingCopyInfo.ConflictOldFile, Is.Null);
                    Assert.That(e.WorkingCopyInfo.ConflictWorkFile, Is.Null);
                    Assert.That(e.WorkingCopyInfo.CopiedFrom, Is.Null);
                    Assert.That(e.WorkingCopyInfo.CopiedFromRevision, Is.EqualTo(-1L));
                    Assert.That(e.WorkingCopyInfo.Depth, Is.EqualTo(SvnDepth.Infinity));
                    Assert.That(e.WorkingCopyInfo.HasProperties, Is.False);
                    Assert.That(e.WorkingCopyInfo.HasPropertyChanges, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsCopy, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);
                    Assert.That(e.WorkingCopyInfo.KeepLocal, Is.False);
                    Assert.That(e.WorkingCopyInfo.LastChangeAuthor, Is.EqualTo(Environment.UserName));
                    Assert.That(e.WorkingCopyInfo.LastChangeRevision, Is.EqualTo(ci.Revision));
                    Assert.That(e.WorkingCopyInfo.LastChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 45)));
                    Assert.That(e.WorkingCopyInfo.LockComment, Is.Null);
                    Assert.That(e.WorkingCopyInfo.LockOwner, Is.Null);
                    Assert.That(e.WorkingCopyInfo.LockTime, Is.EqualTo(DateTime.MinValue));
                    Assert.That(e.WorkingCopyInfo.LockToken, Is.Null);
                    Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo("LocalDeleteBase"));
                    Assert.That(e.WorkingCopyInfo.NodeKind, Is.EqualTo(SvnNodeKind.File));
                    Assert.That(e.WorkingCopyInfo.PropertyRejectFile, Is.Null);
                    Assert.That(e.WorkingCopyInfo.RepositoryId, Is.Not.Null);
                    Assert.That(e.WorkingCopyInfo.RepositoryId, Is.Not.EqualTo(Guid.Empty));
                    Assert.That(e.WorkingCopyInfo.RepositoryUri, Is.EqualTo(ReposUri));
                    Assert.That(e.WorkingCopyInfo.Revision, Is.EqualTo(ci.Revision));
                    Assert.That(e.WorkingCopyInfo.Schedule, Is.EqualTo(SvnSchedule.Delete));
                    Assert.That(e.WorkingCopyInfo.Uri, Is.EqualTo(new Uri(WcUri, "LocalDeleteBase")));
#pragma warning restore 0618
                    visited = true;
                });
                Assert.That(visited, "Visited handler");

                client.Commit(WcPath);

                Assert.That(ItemExists(new Uri(WcUri, "LocalDeleteBase")), Is.False, "Remote base does not exist");
                Assert.That(ItemExists(new Uri(WcUri, "RemoteDeleteBase")), Is.False, "Local base does not exist");
            }
        }

        [Test]
        public void TestExport()
        {
            string exportDir = Path.Combine(TestPath, "ExportTests");
            if (Directory.Exists(exportDir))
                ForcedDeleteDirectory(exportDir);

            using (SvnClient client = NewSvnClient(true, false))
            {
                string file = Path.Combine(WcPath, "ExportFile");
                TouchFile(file);
                client.Add(file);

                client.Commit(WcPath);

                Assert.That(Directory.Exists(exportDir), Is.False);

                client.Export(WcUri, exportDir);
                Assert.That(Directory.Exists(exportDir), Is.True);
                Assert.That(File.Exists(Path.Combine(exportDir, "ExportFile")));
                Assert.That(!Directory.Exists(Path.Combine(exportDir, ".svn")));

                ForcedDeleteDirectory(exportDir);

                Assert.That(Directory.Exists(exportDir), Is.False);

                client.Export(new SvnPathTarget(WcPath), exportDir);
                Assert.That(Directory.Exists(exportDir), Is.True);
                Assert.That(File.Exists(Path.Combine(exportDir, "ExportFile")));

                ForcedDeleteDirectory(exportDir);
            }
        }


        [Test]
        public void TestList()
        {
            using (SvnClient client = NewSvnClient(true, false))
            {

                string oneFile = Path.Combine(WcPath, "LocalFileForTestList");
                TouchFile(oneFile);
                client.Add(oneFile);

                SvnCommitResult ci;
                client.Commit(WcPath, out ci);
                SvnUpdateResult r;
                client.Update(WcPath, out r);

                Assert.That(r, Is.Not.Null);
                Assert.That(r.HasRevision);
                Assert.That(r.HasResultMap);
                Assert.That(r.Revision, Is.EqualTo(ci.Revision));

                bool visited = false;
                SvnListArgs a = new SvnListArgs();
                a.RetrieveEntries = SvnDirEntryItems.AllFieldsV15;

                client.List(new SvnPathTarget(WcPath), a, delegate(object sender, SvnListEventArgs e)
                    {
                        Assert.That(e.Entry, Is.Not.Null, "Entry set");
                        Assert.That(e.RepositoryRoot, Is.Null, "Only valid when listing a Uri");

                        if (e.Path == "LocalFileForTestList")
                        {
                            Assert.That(e.BasePath, Is.EqualTo("/folder"), "Basepath = '/folder'");
                            Assert.That(e.Lock, Is.Null);
                            Assert.That(e.Entry.Author, Is.EqualTo(Environment.UserName));
                            Assert.That(e.Entry.FileSize, Is.EqualTo(0));
                            Assert.That(e.Entry.NodeKind, Is.EqualTo(SvnNodeKind.File));
                            Assert.That(e.Entry.Revision, Is.EqualTo(ci.Revision));
                            Assert.That(e.Entry.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                            visited = true;
                        }
                    });
                Assert.That(visited, Is.True, "Visited is true");


                visited = false;
                client.List(WcUri, a, delegate(object sender, SvnListEventArgs e)
                    {
                        Assert.That(e.Entry, Is.Not.Null, "Entry set");

                        if (e.Path == "LocalFileForTestList")
                        {
                            Assert.That(e.BasePath, Is.EqualTo("/folder"), "Basepath = '/folder'");
                            Assert.That(e.Lock, Is.Null);
                            Assert.That(e.Entry.Author, Is.EqualTo(Environment.UserName));
                            Assert.That(e.Entry.FileSize, Is.EqualTo(0));
                            Assert.That(e.Entry.NodeKind, Is.EqualTo(SvnNodeKind.File));
                            Assert.That(e.Entry.Revision, Is.EqualTo(ci.Revision));
                            Assert.That(e.Entry.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                            visited = true;
                        }
                    });
                Assert.That(visited, Is.True, "Visited is true");

                SvnWorkingCopyClient wcC = new SvnWorkingCopyClient();
                SvnWorkingCopyState state;
                Assert.That(wcC.GetState(oneFile, out state));

                Assert.That(state, Is.Not.Null);
                Assert.That(state.IsTextFile, Is.True);

                client.SetProperty(oneFile, "svn:mime-type", "application/binary");

                Assert.That(wcC.GetState(oneFile, out state));

                Assert.That(state, Is.Not.Null);
                Assert.That(state.IsTextFile, Is.False);
            }
        }

		[Test]
		public void HasIgnorePatterns()
		{
			SvnClient cl = new SvnClient();
			cl.LoadConfigurationDefault();

			List<string> items = new List<string>(cl.Configuration.GlobalIgnorePattern);

			Assert.That(new string[] {"*.a", ".DS_Store", "*.lo"}, Is.SubsetOf(items));
		}

        [Test]
        public void DontCanonicalizeToDotSlash()
        {
            SvnPathTarget path = new SvnPathTarget(".\\bladiebla.txt");

            Assert.That(path.TargetName, Is.EqualTo("bladiebla.txt"));

            Assert.That(Path.GetFullPath("c:\\windows\\"), Is.EqualTo("c:\\windows\\"));
            Assert.That(SvnTools.GetNormalizedFullPath("c:\\windows\\"), Is.EqualTo("C:\\windows"));

            Assert.That(SvnRevision.Base == SvnRevision.Base);
            Assert.That(SvnRevision.Base.Equals(SvnRevision.Base));
            Assert.That(SvnRevision.Base.Equals(SvnRevisionType.Base));
            Assert.That(SvnRevision.Base == SvnRevisionType.Base);

            Assert.That(SvnRevision.Working == SvnRevision.Working);
            Assert.That(SvnRevision.Working.Equals(SvnRevision.Working));
            Assert.That(SvnRevision.Working.Equals(SvnRevisionType.Working));
            Assert.That(SvnRevision.Working == SvnRevisionType.Working);

            DirectoryInfo dir = new DirectoryInfo("c:\\");

            foreach (FileInfo file in dir.GetFiles())
            {
                Assert.That(file.FullName.StartsWith("c:\\"));
            }

            dir = new DirectoryInfo("C:\\");

            foreach (FileInfo file in dir.GetFiles())
            {
                Assert.That(file.FullName.StartsWith("C:\\"));
            }
        }

        [Test]
        public void FetchWcroot()
        {
            Assert.That(Client.GetWorkingCopyRoot(WcPath), Is.EqualTo(WcPath));
        }
    }
}
