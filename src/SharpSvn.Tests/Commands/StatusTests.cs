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
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::Status
	/// </summary>
	[TestFixture]
	public class StatusTests : TestBase
	{
		/// <summary>Compares the status from Client::Status with the output from 
		/// commandline client</summary>
		[Test]
		public void TestLocalStatus()
		{
			string unversioned = this.CreateTextFile("unversioned.cs");
			string added = this.CreateTextFile("added.cs");
			this.RunCommand("svn", "add " + added);
			string changed = this.CreateTextFile("Form.cs");
			string ignored = this.CreateTextFile("foo.ignored");
			string propChange = Path.Combine(this.WcPath, "App.ico");
			this.RunCommand("svn", "ps foo bar " + propChange);
			this.RunCommand("svn", "ps svn:ignore *.ignored " + this.WcPath);

			SvnStatusArgs a = new SvnStatusArgs();

			Client.Status(unversioned, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.NotVersioned, Is.EqualTo(this.currentStatus.LocalContentStatus),
				"Wrong text status on " + unversioned);
			Assert.That(string.Compare(unversioned, currentStatus.Path, true) == 0, "Unversioned filenames don't match");


			Client.Status(added, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.Added, Is.EqualTo(this.currentStatus.LocalContentStatus),
				"Wrong text status on " + added);
			Assert.That(string.Compare(added, currentStatus.Path, true) == 0, "Added filenames don't match");


			Client.Status(changed, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.Modified, Is.EqualTo(this.currentStatus.LocalContentStatus),
				"Wrong text status " + changed);
			Assert.That(string.Compare(changed, this.currentStatus.Path, true) == 0, "Changed filenames don't match");


			Client.Status(propChange, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.Modified, Is.EqualTo(this.currentStatus.LocalPropertyStatus),
				"Wrong property status " + propChange);
			Assert.That(string.Compare(propChange, currentStatus.Path, true) == 0, "Propchanged filenames don't match");

			a.RetrieveAllEntries = true;
			currentStatus = null;

			Client.Status(ignored, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(this.currentStatus.LocalContentStatus, Is.EqualTo(SvnStatus.Ignored),
				"Wrong content status " + ignored);
		}


		[Test]
		public void TestEntry()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock " + form);

			string output = this.RunCommand("svn", "info " + form);
			Info info = new Info(output);

			Collection<SvnStatusEventArgs> statuses;

			Client.GetStatus(form, out statuses);

			info.CheckEquals(statuses[0].WorkingCopyInfo);

			//Status s = SingleStatus(client, form);

			SvnStatusArgs a = new SvnStatusArgs();
			this.Client.Status(form, a, new EventHandler<SvnStatusEventArgs>(this.StatusFunc));

			info.CheckEquals(this.currentStatus.WorkingCopyInfo);


		}

        [Test]
        public void CheckRemoteStatus()
        {
            string dir = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, "..\\.."));

            SvnStatusArgs sa = new SvnStatusArgs();
            sa.RetrieveRemoteStatus = true;

            Collection<SvnStatusEventArgs> r;
            Client.GetStatus(dir, sa, out r);
        }

        [Test]
        public void CheckLocalRemoteStatus()
        {
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
		/*[Test]
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

				Assert.AreEqual(this.currentStatus.RepositoryTextStatus,
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

            if(rslt.Count != 1)
                return null;
            return rslt[0];
        }

		[Test]
		public void TestSingleStatus()
		{
			string unversioned = this.CreateTextFile("unversioned.cs");
			string added = this.CreateTextFile("added.cs");
			this.RunCommand("svn", "add " + added);

			string changed = this.CreateTextFile("Form.cs");

			string propChange = Path.Combine(this.WcPath, "App.ico");

			SvnStatusEventArgs status = SingleStatus(Client,unversioned);
			Assert.That( status.LocalContentStatus, Is.EqualTo(SvnStatus.NotVersioned),
				"Wrong text status on " + unversioned);

			status = SingleStatus(Client, added);
			Assert.That( status.LocalContentStatus, Is.EqualTo(SvnStatus.Added),
				"Wrong text status on " + added);

			status = SingleStatus(Client, changed);
			Assert.That( status.LocalContentStatus, Is.EqualTo(SvnStatus.Modified),
				"Wrong text status " + changed);

			this.RunCommand("svn", "ps foo bar " + propChange);
			status = SingleStatus(Client, propChange);
			Assert.AreEqual(
				SvnStatus.Modified, status.LocalPropertyStatus,
				"Wrong property status " + propChange);
		}

		[Test]
		public void TestSingleStatusNonExistentPath()
		{
			string doesntExist = Path.Combine(this.WcPath, "doesnt.exist");
			SvnStatusEventArgs status = SingleStatus(Client, doesntExist);
			Assert.That( status, Is.Null);
		}

		/*[Test]
		public void TestSingleStatusUnversionedPath()
		{
			string dir = Path.Combine(this.WcPath, "Unversioned");
			string file = Path.Combine(dir, "file.txt");
			SvnStatusEventArgs status = SingleStatus(Client, file);
			Assert.That( status, Is.EqualTo(SvnStatusEventArgs.None));

		}*/

		[Test]
		public void TestSingleStatusNodeKind()
		{
			string file = Path.Combine(this.WcPath, "Form.cs");
			Assert.That( SingleStatus(Client, file).NodeKind, Is.EqualTo(SvnNodeKind.File));
			Assert.That( SingleStatus(Client, file).WorkingCopyInfo.Name, Is.EqualTo("Form.cs"));

			SvnStatusEventArgs dir = SingleStatus(Client, this.WcPath);
			Assert.That( SingleStatus(Client, this.WcPath).WorkingCopyInfo.NodeKind, Is.EqualTo(SvnNodeKind.Directory));
		}



		[Test]
		public void LockSingleStatusIsNullForUnlocked()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			SvnStatusEventArgs status1 = SingleStatus(Client, form);
			Assert.IsNull(status1.WorkingCopyInfo.LockToken);
		}

		[Test]
		public void LocalLockSingleStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock -m test " + form);

			SvnStatusEventArgs s = SingleStatus(Client, form);
			Assert.IsNotNull(s.WorkingCopyInfo.LockToken);
			Assert.That( s.WorkingCopyInfo.LockOwner, Is.EqualTo(Environment.UserName));
			Assert.That( s.WorkingCopyInfo.LockTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
			Assert.That( s.WorkingCopyInfo.LockComment, Is.EqualTo("test"));
		}

		[Test]
		public void LocalLockStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock -m test " + form);

            SvnStatusArgs sa = new SvnStatusArgs();

            this.Client.Status(form, StatusFunc);
			SvnStatusEventArgs s = this.currentStatus;

			Assert.IsNotNull(s.WorkingCopyInfo.LockToken);
			Assert.That( s.WorkingCopyInfo.LockOwner, Is.EqualTo(Environment.UserName));
			Assert.That( s.WorkingCopyInfo.LockTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
			Assert.That( s.WorkingCopyInfo.LockComment, Is.EqualTo("test"));

		}

        [Test]
        public void WcStatusTest()
        {
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

        [Test]
        public void MoreStatusTests()
        {
            using (SvnClient client = NewSvnClient(true, false))
            {
                client.Update(WcPath);
                client.CreateDirectory(Path.Combine(WcPath, "statTst"));
                string local = Path.Combine(WcPath, "statTst/LocalStatBase1");
                string local2 = Path.Combine(WcPath, "statTst/LocalStatBase2");
                string local3 = Path.Combine(WcPath, "statTst/LocalStatBase3");
                string local4 = Path.Combine(WcPath, "statTst/LocalStatBase4");
                string local5 = Path.Combine(WcPath, "statTst/LocalStatBase5");
                string local6 = Path.Combine(WcPath, "statTst/LocalStatBase6");

                SvnCommitResult ci;

                Touch2(local);
                client.Add(local);
                client.Commit(WcPath, out ci);

                client.Copy(local, local2);
                client.Commit(WcPath);

                client.SetProperty(local, "test-q", "value");

                client.Copy(local, local3);
                client.Copy(local, local6);
                client.Copy(local, local5);
                client.Commit(WcPath);

                client.Copy(local, local4);
                client.Delete(local5);

                client.SetProperty(local, "test-q", "value2");
                client.RemoteDelete(new Uri(ReposUrl, "statTst/LocalStatBase2"));
                client.AddToChangeList(local6, "MyList");
                Touch2(local6);

                Guid reposGuid;

                client.TryGetRepositoryId(ReposUrl, out reposGuid);

                for (int mode = 0; mode < 4; mode++)
                {
                    SvnStatusArgs a = new SvnStatusArgs();

                    a.RetrieveRemoteStatus = mode % 2 == 1;
                    a.RetrieveAllEntries = mode > 1;

                    int n = 0;

                    client.Status(Path.Combine(WcPath, "statTst"), a, delegate(object sender, SvnStatusEventArgs e)
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
                            Assert.That(Path.GetDirectoryName(e.FullPath), Is.EqualTo(Path.Combine(WcPath, "statTst")));
                        Assert.That(Path.GetFileName(e.Path), Is.EqualTo(Path.GetFileName(e.FullPath)));

                        switch (nn)
                        {
                            case 1:
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Modified));
                                break;
                            case 2:
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            case 3:
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
                                break;
                            case 4:
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Added));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            case 5:
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Deleted));
                                Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            case 6:
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Modified));
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
                            Assert.That(e.Uri, Is.EqualTo(new Uri(ReposUrl, "statTst/localStatBase" + nn.ToString())));
                        else
                            Assert.That(e.Uri, Is.EqualTo(new Uri(ReposUrl, "statTst/")));


                        Assert.That(e.WorkingCopyInfo, Is.Not.Null);

                        if (nn >= 0)
                            Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(Path.GetFileName(e.Path)));
                        else
                            Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(""));

                        Assert.That(e.WorkingCopyInfo.AvailableProperties, Is.Null);
                        Assert.That(e.WorkingCopyInfo.CacheableProperties, Is.All.Not.EqualTo(""));
                        Assert.That(e.WorkingCopyInfo.ChangeList, Is.EqualTo((nn == 6) ? "MyList" : null));
                        if (nn >= 0)
                            Assert.That(e.WorkingCopyInfo.Checksum, Is.Not.Null);

                        Assert.That(e.WorkingCopyInfo.Uri, Is.EqualTo(e.Uri));
                        Assert.That(e.WorkingCopyInfo.RepositoryUri, Is.EqualTo(ReposUrl));
                        /*if(a.ContactRepository)
                            Assert.That(e.WorkingCopyInfo.RepositoryId, Is.EqualTo(reposGuid));
                        else*/

                        if(nn == 1 || nn == 2 || nn == 3 || nn == 5 || nn == 6)
                            Assert.That(e.WorkingCopyInfo.RepositoryId, Is.Not.EqualTo(Guid.Empty), "Expected set guid for {0} / nn={1}", e.Path, nn);
                        else if (nn >= 0)
                            Assert.That(e.WorkingCopyInfo.RepositoryId, Is.EqualTo(Guid.Empty), "Expected empty guid for {0} / nn={1}", e.Path, nn);

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
                            Assert.That(e.WorkingCopyInfo.CopiedFrom, Is.EqualTo(new Uri(ReposUrl, "statTst/localStatBase1")));
                            Assert.That(e.WorkingCopyInfo.LastChangeAuthor, Is.Null);
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
                            Assert.That(e.WorkingCopyInfo.LastChangeRevision, Is.EqualTo(-1L));
                            Assert.That(e.WorkingCopyInfo.LastChangeTime, Is.EqualTo(DateTime.MinValue));
                            Assert.That(e.WorkingCopyInfo.Schedule, Is.EqualTo(SvnSchedule.Add));
                            Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.EqualTo(DateTime.MinValue));
                            Assert.That(e.WorkingCopyInfo.Revision, Is.EqualTo(ci.Revision));
                            Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(-1L));
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

                            if (nn >= 0)
                            {
                                Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 45)));
                                Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(36L));
                            }
                            else
                            {
                                Assert.That(e.WorkingCopyInfo.ContentChangeTime, Is.EqualTo(DateTime.MinValue));
                                Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(0L));
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

                client.Status(local, delegate(object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                    Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Missing));
                });
                SvnDeleteArgs da = new SvnDeleteArgs();
                da.Force = true;
                client.Delete(local, da);
                client.Status(local, delegate(object sender, SvnStatusEventArgs e)
                {
                    Assert.That(e.WorkingCopyInfo.IsAbsent, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsIncomplete, Is.False);
                    Assert.That(e.WorkingCopyInfo.IsDeleted, Is.False);
                    Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Deleted));
                });

            }
        }

		/*[Test]
		public void RepositoryLockStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock -m test " + form);

			int youngest;
			this.Client.Status(out youngest, form, Revision.Unspecified, new StatusCallback(this.StatusFunc),
				false, true, true, false);
			Status status = this.currentStatus;
			Assert.IsNotNull(status.ReposLock);
			Assert.That( status.ReposLock.Owner, Is.EqualTo(Environment.UserName));
			Assert.That( DateTime.Now.Date, Is.EqualTo(status.ReposLock.CreationDate.ToLocalTime().Date));
			Assert.That( status.ReposLock.Comment, Is.EqualTo("test"));
			Assert.IsFalse(status.ReposLock.IsDavComment);
		}*/

		private void StatusFunc(object sender, SvnStatusEventArgs e)
		{
			e.Detach();
			this.currentStatus = e;
		}

		private class Info
		{
			private static readonly Regex INFO = new Regex(@"Path:\s(?'path'.+?)\s+Name:\s(?'name'\S+)\s+" +
				@"Url:\s(?'url'\S+)\s+Repository UUID:\s(?'reposuuid'\S+)\s+Revision:\s(?'revision'\S+)\s+" +
				@"Node Kind:\s(?'nodekind'\S+)\s+Schedule:\s(?'schedule'\S+)\s+Last Changed Author:\s+" +
				@"(?'lastchangedauthor'\S+).*Lock Token:\s+(?'locktoken'\S+)\s+" +
				@"Lock Owner:\s+(?'lockowner'\S+)\s+Lock Created:\s+(?'lockcreated'\S+)",
				RegexOptions.IgnoreCase | RegexOptions.Singleline);

			public Info(string output)
			{
				this.output = output;
			}

			public void CheckEquals(SvnWorkingCopyInfo entry)
			{
				if (!INFO.IsMatch(this.output))
					throw new Exception("No match");

				Match match = INFO.Match(this.output);

				Assert.That(entry.Uri.ToString(), Is.EqualTo(match.Groups["url"].Value), "Url differs");
				Assert.That(entry.Name, Is.EqualTo(match.Groups["name"].Value), "Name differs");
				Assert.That(entry.Revision, Is.EqualTo(int.Parse(match.Groups["revision"].Value)),
					"Revision differs");
				Assert.That(entry.NodeKind.ToString().ToLower(), Is.EqualTo(match.Groups["nodekind"].Value.ToLower()),
					"Node kind differs");
				Assert.That(entry.RepositoryId.ToString(), Is.EqualTo(match.Groups["reposuuid"].Value),
					"Repository UUID differs");
				Assert.That(entry.Schedule.ToString().ToLower(), Is.EqualTo(match.Groups["schedule"].Value.ToLower()),
					"Schedule differs");
				Assert.That(entry.LastChangeAuthor, Is.EqualTo(match.Groups["lastchangedauthor"].Value),
					"Last changed author differs");
				Assert.That(entry.LockToken, Is.EqualTo(match.Groups["locktoken"].Value), "Lock token differs");
				Assert.That(entry.LockOwner, Is.EqualTo(match.Groups["lockowner"].Value), "Lock owner differs");
			}

			private string output;
		}

		private SvnStatusEventArgs currentStatus;
	}
}
