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
using System.Text;

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

			Client.Status(unversioned, a,
				delegate(object sender, SvnStatusEventArgs e)
				{
					Assert.That(SvnStatus.NotVersioned, Is.EqualTo(e.LocalContentStatus),
				"Wrong text status on " + unversioned);
					Assert.That(string.Compare(unversioned, e.Path, true) == 0, "Unversioned filenames don't match");
				});

			Client.Status(added, a,
		delegate(object sender, SvnStatusEventArgs e)
		{
					Assert.That(SvnStatus.Added, Is.EqualTo(e.LocalContentStatus),
						"Wrong text status on " + added);
					Assert.That(string.Compare(added, e.Path, true) == 0, "Added filenames don't match");
		});


					Client.Status(changed, a,
				delegate(object sender, SvnStatusEventArgs e)
				{
					Assert.That(SvnStatus.Modified, Is.EqualTo(e.LocalContentStatus),
						"Wrong text status " + changed);
					Assert.That(string.Compare(changed, e.Path, true) == 0, "Changed filenames don't match");
				});

					Client.Status(propChange, a,
				delegate(object sender, SvnStatusEventArgs e)
				{
					Assert.That(SvnStatus.Modified, Is.EqualTo(e.LocalPropertyStatus),
						"Wrong property status " + propChange);
					Assert.That(string.Compare(propChange, e.Path, true) == 0, "Propchanged filenames don't match");
				});

					a.RetrieveAllEntries = true;

					Client.Status(ignored, a,
				delegate(object sender, SvnStatusEventArgs e)
				{
					Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Ignored),
						"Wrong content status " + ignored);
				});
		}


		[Test]
		public void TestEntry()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock " + form);

			string output = this.RunCommand("svn", "info " + form);

			Collection<SvnStatusEventArgs> statuses;

			Client.GetStatus(form, out statuses);

			//Status s = SingleStatus(client, form);

			SvnStatusArgs a = new SvnStatusArgs();
			this.Client.Status(form, a, delegate(object sender, SvnStatusEventArgs e)
			{ });
		}

		[Test]
		public void CheckRemoteStatus()
		{
			string dir = WcPath;

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

		[Test]
		public void TestSingleStatusNonExistentPath()
		{
			string doesntExist = Path.Combine(this.WcPath, "doesnt.exist");
			SvnStatusEventArgs status = SingleStatus(Client, doesntExist);
			Assert.That(status, Is.Null);
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
			Assert.That(SingleStatus(Client, file).NodeKind, Is.EqualTo(SvnNodeKind.File));
			Assert.That(SingleStatus(Client, file).WorkingCopyInfo.Name, Is.EqualTo("Form.cs"));

			SvnStatusEventArgs dir = SingleStatus(Client, this.WcPath);
			Assert.That(SingleStatus(Client, this.WcPath).WorkingCopyInfo.NodeKind, Is.EqualTo(SvnNodeKind.Directory));
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
			Assert.That(s.WorkingCopyInfo.LockOwner, Is.EqualTo(Environment.UserName));
			Assert.That(s.WorkingCopyInfo.LockTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
			Assert.That(s.WorkingCopyInfo.LockComment, Is.EqualTo("test"));
		}

		[Test]
		public void LocalLockStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock -m test " + form);

			SvnStatusArgs sa = new SvnStatusArgs();

			this.Client.Status(form, delegate(object sender, SvnStatusEventArgs e)
			{
				Assert.IsNotNull(e.WorkingCopyInfo.LockToken);
				Assert.That(e.WorkingCopyInfo.LockOwner, Is.EqualTo(Environment.UserName));
				Assert.That(e.WorkingCopyInfo.LockTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
				Assert.That(e.WorkingCopyInfo.LockComment, Is.EqualTo("test"));
			});
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
							Assert.That(e.Uri, Is.EqualTo(new Uri(ReposUrl, "statTst/localStatBase" + nn.ToString())));
						else
							Assert.That(e.Uri, Is.EqualTo(new Uri(ReposUrl, "statTst/")));


						Assert.That(e.WorkingCopyInfo, Is.Not.Null);

						if (nn >= 0)
							Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(Path.GetFileName(e.Path)));
						else
							Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(""));

						Assert.That(e.WorkingCopyInfo.ChangeList, Is.EqualTo((nn == 6) ? "MyList" : null));
						if (nn >= 0)
							Assert.That(e.WorkingCopyInfo.Checksum, Is.Not.Null);

						Assert.That(e.WorkingCopyInfo.Uri, Is.EqualTo(e.Uri));
						Assert.That(e.WorkingCopyInfo.RepositoryUri, Is.EqualTo(ReposUrl));
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
							Assert.That(e.WorkingCopyInfo.CopiedFrom, Is.EqualTo(new Uri(ReposUrl, "statTst/localStatBase1")));
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

		[Test]
		public void StatusDelayTest()
		{
			string tmp1Dir = GetTempDir();
			string tmp2Dir = GetTempDir();
			string tmp3Dir = GetTempDir();

			Client.CheckOut(GetReposUri(TestReposType.Empty), tmp1Dir);
			Client.CheckOut(GetReposUri(TestReposType.Empty), tmp2Dir);
			Client.CheckOut(GetReposUri(TestReposType.Empty), tmp3Dir);

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

		[Test]
		public void CheckOverrideWorking()
		{
			string tmp2Dir = GetTempDir();

			Client.CheckOut(GetReposUri(TestReposType.Empty), tmp2Dir);

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

		[Test]
		public void RepositoryLockStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			Client.Lock(form, "test");

			SvnStatusArgs sa = new SvnStatusArgs() { RetrieveRemoteStatus = true };
			this.Client.Status(form, sa,
				delegate(object sender, SvnStatusEventArgs e)
				{
					Assert.IsNotNull(e.RemoteLock);
					Assert.That(e.RemoteLock.Owner, Is.EqualTo(Environment.UserName));
					Assert.That(DateTime.Now.Date, Is.EqualTo(e.RemoteLock.CreationTime.ToLocalTime().Date));
					Assert.That(e.RemoteLock.Comment, Is.EqualTo("test"));
					Assert.IsFalse(e.RemoteLock.IsRawNetworkComment);
				});
		}
	}
}
