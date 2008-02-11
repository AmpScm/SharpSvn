// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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
	public class SvnClientTests : SvnTestBase
	{
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

		string WcPath
		{
			get
			{
				if (_wc == null)
					_wc = Path.Combine(TestPath, "wc");

				return _wc;
			}
		}

		static void TouchFile(string filename)
		{
			TouchFile(filename, false);
		}

		static void Touch2(string filename)
		{
			File.WriteAllText(filename, Guid.NewGuid().ToString());
		}

		static void TouchFile(string filename, bool createDir)
		{
			string dir = Path.GetDirectoryName(filename);
			if (createDir && !Directory.Exists(dir))
				Directory.CreateDirectory(dir);

			using (FileStream fs = File.Create(filename))
			{
				fs.Write(new byte[0], 0, 0);
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
				cra.RepositoryCompatibility = SvnRepositoryCompatibility.SubversionPre15;
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

		SvnClient NewSvnClient(bool expectCommit, bool expectConflict)
		{
			SvnClient client = new SvnClient();

			client.Conflict += delegate(object sender, SvnConflictEventArgs e)
			{
				Assert.That(true, Is.EqualTo(expectConflict), "Conflict expected");
			};

			client.Committing += delegate(object sender, SvnCommittingEventArgs e)
			{
				Assert.That(true, Is.EqualTo(expectCommit), "Commit expected");
				if (e.LogMessage == null)
					e.LogMessage = "";
			};

			return client;
		}

		[Test]
		public void CreateTrunk()
		{
			using (SvnClient client = NewSvnClient(true, false))
			{
				Uri trunkUri = new Uri(ReposUri, "trunk/");
				client.RemoteCreateDirectory(trunkUri);

				string trunkPath = Path.Combine(TestPath, "trunk");

				if (Directory.Exists(trunkPath))
					ForcedDeleteDirectory(trunkPath);

				client.CheckOut(trunkUri, trunkPath);

				TouchFile(Path.Combine(trunkPath, "test.txt"));

				client.Add(Path.Combine(trunkPath, "test.txt"));

				TouchFile(Path.Combine(trunkPath, "dir/test.txt"), true);

				SvnAddArgs aa = new SvnAddArgs();
				aa.AddParents = true;
				client.Add(Path.Combine(trunkPath, "dir/test.txt"), aa);

				client.Commit(trunkPath);

				client.RemoteDelete(trunkUri, new SvnDeleteArgs());
			}
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
		public void RemoteListTest()
		{
			using (SvnClient client = NewSvnClient(false, false))
			{
				Collection<SvnListEventArgs> items;
				client.GetList(ReposUri, out items);

				Assert.That(items, Is.Not.Null, "Items retrieved");
				Assert.That(items.Count, Is.GreaterThan(0), "More than 0 items");
			}
		}

		public void InfoTest()
		{
			using (SvnClient client = NewSvnClient(false, false))
			{
				Collection<SvnInfoEventArgs> items;
				client.GetInfo(ReposUri, new SvnInfoArgs(), out items);

				Assert.That(items, Is.Not.Null, "Items retrieved");
				Assert.That(items.Count, Is.EqualTo(1), "1 info item");

				SvnInfoEventArgs info = items[0];
				Assert.That(info.Uri.ToString() + "/", Is.EqualTo(ReposUri.ToString()), "Repository uri matches");
				Assert.That(info.HasLocalInfo, Is.False, "No WC info");
				Assert.That(info.Path, Is.EqualTo(""), "Path is empty");

				client.GetInfo(new Uri(ReposUri, "folder"), out info);

				Assert.That(info.Uri.ToString() + "/", Is.EqualTo(ReposUri.ToString()), "Repository uri matches");
				Assert.That(info.HasLocalInfo, Is.False, "No WC info");
				Assert.That(info.Path, Is.EqualTo("folder"), "Path is empty");
			}
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
		public void WriteTest()
		{
			string data = Guid.NewGuid().ToString();
			using (SvnClient client = NewSvnClient(true, false))
			{
				string file = Path.Combine(WcPath, "WriteTest");
				using (StreamWriter sw = File.CreateText(file))
				{
					sw.WriteLine(data);
				}

				client.Add(file);
				client.AddToChangeList(file, "WriteTest-Items");

				SvnCommitArgs ca = new SvnCommitArgs();
				ca.ChangeLists.Add("WriteTest-Items");
				client.Commit(WcPath);

				using (MemoryStream ms = new MemoryStream())
				{
					client.Write(new SvnPathTarget(file), ms);

					ms.Position = 0;

					using (StreamReader sr = new StreamReader(ms))
					{
						Assert.That(sr.ReadLine(), Is.EqualTo(data));
						Assert.That(sr.ReadToEnd(), Is.EqualTo(""));
					}
				}

				using (MemoryStream ms = new MemoryStream())
				{
					client.Write(new Uri(WcUri, "WriteTest"), ms);

					ms.Position = 0;

					using (StreamReader sr = new StreamReader(ms))
					{
						Assert.That(sr.ReadLine(), Is.EqualTo(data));
						Assert.That(sr.ReadToEnd(), Is.EqualTo(""));
					}
				}
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
					Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Added));
					Assert.That(e.IsRemoteUpdated, Is.False);
					Assert.That(e.Path.Replace('/', Path.DirectorySeparatorChar), Is.EqualTo(file4));
					visited = true;
				});
				Assert.That(visited, Is.True);

				client.Commit(WcPath);

				Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile3"), ia, out ee), Is.True);
				Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile4"), ia, out ee), Is.True);
			}
		}

		[Test]
		public void TestCleanup()
		{
			using (SvnClient client = NewSvnClient(false, false))
			{
				client.CleanUp(WcPath);
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
							Assert.That(i.CopyFromPath.EndsWith("folder/CopyBase"), "CopyFromPath ensd with folder/CopyBase");
							Assert.That(i.CopyFromRevision, Is.GreaterThan(0L));
						}
					}
					else
					{
						foreach (SvnChangeItem i in e.ChangedPaths)
						{
							Assert.That(i.Action, Is.EqualTo(SvnChangeAction.Add));
							Assert.That(i.Path.EndsWith("folder/CopyBase"), "Path ends with folder/CopyBase");
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
		public void MoveTest()
		{
			using (SvnClient client = NewSvnClient(true, false))
			{
				string file = Path.Combine(WcPath, "LocalMoveBase");

				TouchFile(file);
				client.Add(file);

				TouchFile(Path.Combine(TestPath, "MoveTestImport/RemoteMoveBase"), true);

				client.RemoteImport(Path.Combine(TestPath, "MoveTestImport"), new Uri(WcUri, "RemoteMoveBase"));

				client.Commit(WcPath);

				Assert.That(ItemExists(new Uri(WcUri, "RemoteMoveBase")), Is.True, "Remote base does exist");
				Assert.That(ItemExists(new Uri(WcUri, "LocalMoveBase")), Is.True, "Local base does exist");

				client.RemoteMove(new Uri(WcUri, "RemoteMoveBase"), new Uri(WcUri, "RemoteMoveTarget"));
				client.Move(file, Path.Combine(WcPath, "LocalMoveTarget"));

				client.Commit(WcPath);

				Assert.That(ItemExists(new Uri(WcUri, "RemoteMoveTarget")), Is.True, "Remote target exists");
				Assert.That(ItemExists(new Uri(WcUri, "LocalMoveTarget")), Is.True, "Local target exists");
				Assert.That(ItemExists(new Uri(WcUri, "RemoteMoveBase")), Is.False, "Remote base does not exist");
				Assert.That(ItemExists(new Uri(WcUri, "LocalMoveBase")), Is.False, "Local base does not exist");
			}
		}

		[Test]
		public void MoveAndEditTest()
		{
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

				SvnCommitInfo ci;
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
					Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Deleted));
					Assert.That(e.Switched, Is.False);
					Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "LocalDeleteBase")));
					Assert.That(e.IsRemoteUpdated, Is.EqualTo(false));
					Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Deleted));
					Assert.That(e.LocalCopied, Is.EqualTo(false));
					Assert.That(e.LocalLocked, Is.EqualTo(false));
					Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.None));
					Assert.That(e.Path, Is.EqualTo(local));
					Assert.That(e.RemoteContentStatus, Is.EqualTo(SvnStatus.None));
					Assert.That(e.RemoteLock, Is.Null);
					Assert.That(e.RemotePropertyStatus, Is.EqualTo(SvnStatus.None));
					Assert.That(e.RemoteUpdateCommitAuthor, Is.Null);
					Assert.That(e.RemoteUpdateCommitTime, Is.EqualTo(DateTime.MinValue));
					Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.None));
					Assert.That(e.RemoteUpdateRevision, Is.EqualTo(-1L));
					Assert.That(e.Switched, Is.EqualTo(false));

					Assert.That(e.WorkingCopyInfo, Is.Not.Null);
					Assert.That(e.WorkingCopyInfo.AvailableProperties, Is.Null);
					Assert.That(e.WorkingCopyInfo.CacheableProperties, Is.Not.Null);
					Assert.That(e.WorkingCopyInfo.CacheableProperties.Contains("svn:externals"));
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
					Assert.That(e.WorkingCopyInfo.PropertyChangeTime, Is.EqualTo(DateTime.MinValue));
					Assert.That(e.WorkingCopyInfo.PropertyRejectFile, Is.Null);
					Assert.That(e.WorkingCopyInfo.RepositoryId, Is.Not.Null);
					Assert.That(e.WorkingCopyInfo.RepositoryUri, Is.EqualTo(ReposUri));
					Assert.That(e.WorkingCopyInfo.Revision, Is.EqualTo(ci.Revision));
					Assert.That(e.WorkingCopyInfo.Schedule, Is.EqualTo(SvnSchedule.Delete));
					Assert.That(e.WorkingCopyInfo.TextChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 45)));
					Assert.That(e.WorkingCopyInfo.Uri, Is.EqualTo(new Uri(WcUri, "LocalDeleteBase")));
					Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(0L));

					visited = true;
				});
				Assert.That(visited, "Visited handler");

				client.Commit(WcPath);

				Assert.That(ItemExists(new Uri(WcUri, "LocalDeleteBase")), Is.False, "Remote base does not exist");
				Assert.That(ItemExists(new Uri(WcUri, "RemoteDeleteBase")), Is.False, "Local base does not exist");
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

				SvnCommitInfo ci;

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
				client.RemoteDelete(new Uri(WcUri, "statTst/LocalStatBase2"));
				client.AddToChangeList(local6, "MyList");
				Touch2(local6);

				Guid reposGuid;
				
				client.GetRepositoryIdFromUri(WcUri, out reposGuid);

				for (int mode = 0; mode < 4; mode++)
				{
					SvnStatusArgs a = new SvnStatusArgs();

					a.ContactRepository = mode % 2 == 1;
					a.GetAll = mode > 1;

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
								Assert.That(e.LocalPropertyStatus, Is.EqualTo(SvnStatus.Normal));
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
						Assert.That(e.LocalLocked, Is.False);
						if (a.ContactRepository)
						{
							Assert.That(e.RemoteContentStatus, Is.EqualTo(nn == 2 ? SvnStatus.Deleted : SvnStatus.None));
						}
						else
						{
							Assert.That(e.RemoteContentStatus, Is.EqualTo(SvnStatus.None));
						}

						Assert.That(e.RemoteLock, Is.Null);
						Assert.That(e.RemotePropertyStatus, Is.EqualTo(SvnStatus.None));

						if (nn == 2 && a.ContactRepository)
						{
							Assert.That(e.RemoteUpdateCommitAuthor, Is.Null);
							Assert.That(e.RemoteUpdateNodeKind, Is.EqualTo(SvnNodeKind.File));
							Assert.That(e.RemoteUpdateCommitTime, Is.EqualTo(DateTime.MinValue));
							Assert.That(e.RemoteUpdateRevision, Is.EqualTo(ci.Revision + 3));
						}
						else if (nn == -1 && a.ContactRepository)
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
							Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "statTst/localStatBase" + nn.ToString())));
						else
							Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "statTst/")));


						Assert.That(e.WorkingCopyInfo, Is.Not.Null);

						if(nn >= 0)
							Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(Path.GetFileName(e.Path)));
						else
							Assert.That(e.WorkingCopyInfo.Name, Is.EqualTo(""));

						Assert.That(e.WorkingCopyInfo.AvailableProperties, Is.Null);
						Assert.That(e.WorkingCopyInfo.CacheableProperties, Is.All.Not.EqualTo(""));
						Assert.That(e.WorkingCopyInfo.ChangeList, Is.EqualTo((nn == 6) ? "MyList" : null));
						if(nn >= 0)
							Assert.That(e.WorkingCopyInfo.Checksum, Is.Not.Null);

						Assert.That(e.WorkingCopyInfo.Uri, Is.EqualTo(e.Uri));
						Assert.That(e.WorkingCopyInfo.RepositoryUri, Is.EqualTo(ReposUri));
						/*if(a.ContactRepository)
							Assert.That(e.WorkingCopyInfo.RepositoryId, Is.EqualTo(reposGuid));
						else*/
							Assert.That(e.WorkingCopyInfo.RepositoryId, Is.EqualTo(Guid.Empty));

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
							Assert.That(e.WorkingCopyInfo.CopiedFrom, Is.EqualTo(new Uri(WcUri, "statTst/localStatBase1")));
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
						Assert.That(e.WorkingCopyInfo.HasProperties, Is.EqualTo(nn >= 0));
						Assert.That(e.WorkingCopyInfo.HasPropertyChanges, Is.EqualTo((nn == 1) || (nn == 4)));
						Assert.That(e.WorkingCopyInfo.KeepLocal, Is.False);

						if (nn == 4)
						{
							Assert.That(e.WorkingCopyInfo.LastChangeRevision, Is.EqualTo(-1L));
							Assert.That(e.WorkingCopyInfo.LastChangeTime, Is.EqualTo(DateTime.MinValue));
							Assert.That(e.WorkingCopyInfo.Schedule, Is.EqualTo(SvnSchedule.Add));
							Assert.That(e.WorkingCopyInfo.TextChangeTime, Is.EqualTo(DateTime.MinValue));
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
								Assert.That(e.WorkingCopyInfo.TextChangeTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 45)));
								Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(36L));
							}
							else
							{
								Assert.That(e.WorkingCopyInfo.TextChangeTime, Is.EqualTo(DateTime.MinValue));
								Assert.That(e.WorkingCopyInfo.WorkingCopySize, Is.EqualTo(0L));
							}
						}
						Assert.That(e.WorkingCopyInfo.LockComment, Is.Null);
						Assert.That(e.WorkingCopyInfo.LockOwner, Is.Null);
						Assert.That(e.WorkingCopyInfo.LockTime, Is.EqualTo(DateTime.MinValue));
						Assert.That(e.WorkingCopyInfo.LockToken, Is.Null);						
						Assert.That(e.WorkingCopyInfo.NodeKind, Is.EqualTo(nn >= 0 ? SvnNodeKind.File : SvnNodeKind.Directory));
						Assert.That(e.WorkingCopyInfo.PropertyChangeTime, Is.EqualTo(DateTime.MinValue));
						Assert.That(e.WorkingCopyInfo.PropertyRejectFile, Is.Null);
					});

					if (a.GetAll)
						Assert.That(n, Is.EqualTo(7));
					else if (a.ContactRepository)
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
		public void DiffTests()
		{
			string start = Guid.NewGuid().ToString() + Environment.NewLine + Guid.NewGuid().ToString();
			string end = Guid.NewGuid().ToString() + Environment.NewLine + Guid.NewGuid().ToString();
			string origLine = Guid.NewGuid().ToString();
			string newLine = Guid.NewGuid().ToString();
			using (SvnClient client = NewSvnClient(true, false))
			{
				string diffFile = Path.Combine(WcPath, "DiffTest");

				using (StreamWriter sw = File.CreateText(diffFile))
				{
					sw.WriteLine(start);
					sw.WriteLine(origLine);
					sw.WriteLine(end);
				}

				client.Add(diffFile);
				SvnCommitInfo ci;
				client.Commit(diffFile, out ci);

				using (StreamWriter sw = File.CreateText(diffFile))
				{
					sw.WriteLine(start);
					sw.WriteLine(newLine);
					sw.WriteLine(end);
				}

				MemoryStream diffOutput = new MemoryStream();

				client.Diff(new SvnPathTarget(diffFile, SvnRevisionType.Working), new SvnPathTarget(diffFile, SvnRevisionType.Head), diffOutput);
				VerifyDiffOutput(origLine, newLine, diffOutput);

				diffOutput = new MemoryStream();

				client.Diff(new SvnPathTarget(diffFile, SvnRevisionType.Working), new SvnPathTarget(diffFile, SvnRevisionType.Committed), diffOutput);
				VerifyDiffOutput(origLine, newLine, diffOutput);

				diffOutput = new MemoryStream();

				client.Diff(new SvnPathTarget(diffFile, SvnRevisionType.Working), new Uri(WcUri, "DiffTest"), diffOutput);
				VerifyDiffOutput(origLine, newLine, diffOutput);

				SvnCommitInfo info;
				client.Commit(diffFile, out info);

				bool visited = false;
				client.DiffSummary(new SvnUriTarget(WcUri, info.Revision - 1), WcUri,
					delegate(object sender, SvnDiffSummaryEventArgs e)
					{
						if (e.Path == "DiffTest")
						{
							Assert.That(e.DiffKind, Is.EqualTo(SvnDiffKind.Modified));
							Assert.That(e.PropertiesChanged, Is.False);
							visited = true;
						}
					});

				Assert.That(visited);

				int n = 0;

				client.Blame(new SvnPathTarget(diffFile), delegate(object sender, SvnBlameEventArgs e)
				{
					Assert.That(e.Author, Is.EqualTo(Environment.UserName));
					Assert.That(e.LineNumber, Is.EqualTo((long)n));
					switch (n)
					{
						case 0:
						case 1:
						case 3:
						case 4:
							Assert.That(e.Revision, Is.EqualTo(ci.Revision));
							break;
						case 2:
							Assert.That(e.Revision, Is.EqualTo(info.Revision));
							break;
						default:
							Assert.That(false, "EOF");
							break;
					}
					Assert.That(e.Line, Is.Not.Null);
					n++;
				});

				Assert.That(n, Is.EqualTo(5), "Blame receiver received 5 lines");
			}
		}

		private static void VerifyDiffOutput(string origLine, string newLine, Stream diffOutput)
		{
			diffOutput.Position = 0;
			using (StreamReader sr = new StreamReader(diffOutput))
			{
				bool foundMin = false;
				bool foundPlus = false;

				string line;

				while (null != (line = sr.ReadLine()))
				{
					if (line.Contains(newLine))
					{
						Assert.That(line.StartsWith("+"));
						foundPlus = true;
					}
					else if (line.Contains(origLine))
					{
						Assert.That(line.StartsWith("-"));
						foundMin = true;
					}
				}
				Assert.That(foundMin, "Found -");
				Assert.That(foundPlus, "Found +");
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
		public void TestInfo()
		{
			using (SvnClient client = NewSvnClient(true, false))
			{
				string file = Path.Combine(WcPath, "InfoFile");
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
					Assert.That(e.CopyFromRev, Is.EqualTo(-1L));
					Assert.That(e.CopyFromUri, Is.Null);
					Assert.That(e.Depth, Is.EqualTo(SvnDepth.Infinity));
					Assert.That(e.FullPath, Is.EqualTo(file));
					Assert.That(e.HasLocalInfo, Is.True);
					Assert.That(e.LastChangeAuthor, Is.Null);
					Assert.That(e.LastChangeTime, Is.EqualTo(DateTime.MinValue));
					Assert.That(e.LastChangeRevision, Is.EqualTo(0)); // Not committed yet
					Assert.That(e.Lock, Is.Null);
					Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
					Assert.That(e.Path, Is.Not.Null);
					Assert.That(e.PropertyEditFile, Is.Null);
					Assert.That(e.PropertyTime, Is.EqualTo(e.ContentTime));
					Assert.That(e.RepositorySize, Is.EqualTo(-1L));
					Assert.That(e.RepositoryRoot, Is.EqualTo(ReposUri));
					Assert.That(e.Revision, Is.EqualTo(0L)); // Not committed yet
					Assert.That(e.Schedule, Is.EqualTo(SvnSchedule.Add));
					Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "InfoFile")));
					Assert.That(e.WorkingCopySize, Is.EqualTo(-1L));
					visited = true;
				});
				Assert.That(visited);

				SvnCommitInfo commitData;
				client.Commit(WcPath, out commitData);
				visited = false;
				client.Info(file, delegate(object sender, SvnInfoEventArgs e)
					{
						Assert.That(e.ChangeList, Is.Null);
						Assert.That(e.Checksum, Is.EqualTo("d41d8cd98f00b204e9800998ecf8427e"));
						Assert.That(e.ConflictNew, Is.Null);
						Assert.That(e.ConflictOld, Is.Null);
						Assert.That(e.ConflictWork, Is.Null);
						Assert.That(e.ContentTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
						Assert.That(e.CopyFromRev, Is.EqualTo(-1L));
						Assert.That(e.CopyFromUri, Is.Null);
						Assert.That(e.Depth, Is.EqualTo(SvnDepth.Infinity));
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
						Assert.That(e.RepositoryRoot, Is.EqualTo(ReposUri));
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
					Assert.That(e.CopyFromRev, Is.EqualTo(0L));
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
					Assert.That(e.PropertyTime, Is.EqualTo(e.ContentTime));
					Assert.That(e.RepositorySize, Is.EqualTo(0L));
					Assert.That(e.RepositoryRoot, Is.EqualTo(ReposUri));
					Assert.That(e.Revision, Is.EqualTo(commitData.Revision));
					Assert.That(e.Schedule, Is.EqualTo(SvnSchedule.Normal));
					Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "InfoFile")));
					Assert.That(e.WorkingCopySize, Is.EqualTo(-1L));
					visited = true;
				});
				Assert.That(visited);
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

				SvnCommitInfo ci;
				client.Commit(WcPath, out ci);
				SvnUpdateResult r;
				client.Update(WcPath, out r);

				Assert.That(r, Is.Not.Null);
				Assert.That(r.HasRevision);
				Assert.That(r.HasResultMap);
				Assert.That(r.Revision, Is.EqualTo(ci.Revision));

				bool visited = false;
				SvnListArgs a = new SvnListArgs();
				a.EntryItems = SvnDirEntryItems.AllFieldsV15;

				client.List(new SvnPathTarget(WcPath), a, delegate(object sender, SvnListEventArgs e)
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

				SvnWorkingCopyState state;
				client.GetWorkingCopyState(oneFile, out state);

				Assert.That(state, Is.Not.Null);
				Assert.That(state.IsTextFile, Is.True);
				Assert.That(state.WorkingCopyBasePath, Is.Not.Null);

				client.SetProperty(oneFile, "svn:mime-type", "application/binary");

				client.GetWorkingCopyState(oneFile, out state);

				Assert.That(state, Is.Not.Null);
				Assert.That(state.IsTextFile, Is.False);
			}
		}

		[Test]
		public void TestLog()
		{
			using (SvnClient client = NewSvnClient(true, false))
			{
				string logFile = Path.Combine(WcPath, "LogTestFileBase");
				TouchFile(logFile);
				client.Add(logFile);
				SvnCommitArgs a = new SvnCommitArgs();
				a.LogMessage = "Commit 1\rWith\nSome\r\nRandom\n\rNewlines\nAdded\n\r\n";
				client.Commit(WcPath, a);

				File.AppendAllText(logFile, Guid.NewGuid().ToString());
				a.LogMessage = "Commit 2";
				client.SetProperty(logFile, "TestProperty", "TestValue");
				client.Commit(WcPath, a);

				string renamedLogFile = Path.Combine(WcPath, "LogTestFileDest");
				client.Move(logFile, renamedLogFile);
				a.LogMessage = "Commit 3" + Environment.NewLine + "With newline";
				client.Commit(WcPath, a);

				int n = 0;
				SvnLogArgs la = new SvnLogArgs();
				la.StrictNodeHistory = false;
				client.Log(new SvnUriTarget(new Uri(WcUri, "LogTestFileDest")),
					delegate(object sender, SvnLogEventArgs e)
					{
						SvnChangeItem ci;
						SvnChangeItem ci2;

						Assert.That(e.Author, Is.EqualTo(Environment.UserName));
						Assert.That(e.Cancel, Is.False);
						Assert.That(e.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
						Assert.That(e.HasChildren, Is.False);
						switch (n)
						{
							case 0:
								Assert.That(e.LogMessage, Is.EqualTo("Commit 3" + Environment.NewLine + "With newline"));
								Assert.That(e.ChangedPaths, Is.Not.Null);
								Assert.That(e.ChangedPaths.Count, Is.EqualTo(2));
								Assert.That(e.ChangedPaths.Contains("/folder/LogTestFileBase"));
								Assert.That(e.ChangedPaths.Contains("/folder/LogTestFileDest"));
								ci = e.ChangedPaths["/folder/LogTestFileBase"];
								ci2 = e.ChangedPaths["/folder/LogTestFileDest"];

								Assert.That(ci, Is.Not.Null);
								Assert.That(ci2, Is.Not.Null);
								Assert.That(ci.Path, Is.EqualTo("/folder/LogTestFileBase"));
								Assert.That(ci.Action, Is.EqualTo(SvnChangeAction.Delete));
								Assert.That(ci.CopyFromPath, Is.Null);
								Assert.That(ci.CopyFromRevision, Is.EqualTo(-1));
								Assert.That(ci2.Path, Is.EqualTo("/folder/LogTestFileDest"));
								Assert.That(ci2.Action, Is.EqualTo(SvnChangeAction.Add));
								Assert.That(ci2.CopyFromPath, Is.EqualTo("/folder/LogTestFileBase"));
								Assert.That(ci2.CopyFromRevision, Is.Not.EqualTo(-1));
								break;
							case 1:
								Assert.That(e.LogMessage, Is.EqualTo("Commit 2"));
								Assert.That(e.ChangedPaths, Is.Not.Null);
								Assert.That(e.ChangedPaths.Count, Is.EqualTo(1));
								ci = e.ChangedPaths[0];
								Assert.That(ci, Is.Not.Null);
								Assert.That(ci.Path, Is.EqualTo("/folder/LogTestFileBase"));
								Assert.That(ci.Action, Is.EqualTo(SvnChangeAction.Modify));
								Assert.That(ci.CopyFromPath, Is.Null);
								Assert.That(ci.CopyFromRevision, Is.EqualTo(-1));
								break;
							case 2:
								Assert.That(e.LogMessage, Is.EqualTo("Commit 1" + Environment.NewLine + "With" +
									Environment.NewLine + "Some" + Environment.NewLine + "Random" + Environment.NewLine +
									"Newlines" + Environment.NewLine + "Added" + Environment.NewLine + Environment.NewLine));
								Assert.That(e.ChangedPaths, Is.Not.Null);
								Assert.That(e.ChangedPaths.Count, Is.EqualTo(2));
								ci = e.ChangedPaths[0];
								Assert.That(ci, Is.Not.Null);
								Assert.That(ci.Path, Is.EqualTo("/folder/LogTestFileBase"));
								Assert.That(ci.Action, Is.EqualTo(SvnChangeAction.Add));
								Assert.That(ci.CopyFromPath, Is.Null);
								Assert.That(ci.CopyFromRevision, Is.EqualTo(-1));
								break;
							default:
								Assert.That(false);
								break;
						}
						n++;
					});

				Assert.That(n, Is.EqualTo(3));
			}
		}

		[Test]
		public void MinimalMergeTest()
		{
			using (SvnClient client = NewSvnClient(true, false))
			{
				string merge1 = Path.Combine(WcPath, "mmerge-1");
				string merge2 = Path.Combine(WcPath, "mmerge-2");
				client.CreateDirectory(merge1);

				string f1 = Path.Combine(merge1, "myfile.txt");

				using (StreamWriter fs = File.CreateText(f1))
				{
					fs.WriteLine("First line");
					fs.WriteLine("Second line");
					fs.WriteLine("Third line");
					fs.WriteLine("Fourth line");
				}

				client.Add(f1);

				SvnCommitInfo ci;
				client.Commit(WcPath, out ci);
				client.Copy(new SvnPathTarget(merge1), merge2);
				client.Commit(WcPath);
				client.Update(WcPath);

				SvnMergeSourcesCollection sources;
				client.GetSuggestedMergeSources(new SvnPathTarget(merge1), out sources);

				Assert.That(sources.Count, Is.EqualTo(0));

				client.GetSuggestedMergeSources(new SvnPathTarget(merge2), out sources);

				Assert.That(sources.Count, Is.EqualTo(1));

				Uri fromUri = new Uri(ReposUri, new Uri("folder/mmerge-1", UriKind.Relative));
				Assert.That(sources[0].Uri, Is.EqualTo(fromUri));

				SvnAppliedMergeInfo applied;
				client.GetAppliedMergeInfo(new SvnPathTarget(merge2), out applied);

				Assert.That(applied, Is.Not.Null);
				Assert.That(applied.AppliedMerges.Count, Is.EqualTo(0));
				Assert.That(applied.Target, Is.Not.Null);

				SvnAvailableMergeInfo available;
				client.GetAvailableMergeInfo(new SvnPathTarget(merge2), fromUri, out available);
				Assert.That(available, Is.Not.Null);
				Assert.That(available.MergeRanges.Count, Is.EqualTo(1));
				Assert.That(available.MergeRanges[0].Start, Is.EqualTo(ci.Revision));
				Assert.That(available.MergeRanges[0].End, Is.EqualTo(ci.Revision + 1));
				Assert.That(available.MergeRanges[0].Inheritable, Is.True);
				Assert.That(available.Target, Is.Not.Null);

				using (StreamWriter fs = File.AppendText(f1))
				{
					fs.WriteLine("Fifth line");
				}
				client.Commit(merge1);

				client.GetAvailableMergeInfo(new SvnPathTarget(merge2), fromUri, out available);
				Assert.That(available, Is.Not.Null);
				Assert.That(available.MergeRanges.Count, Is.EqualTo(1));
				Assert.That(available.MergeRanges[0].Start, Is.EqualTo(ci.Revision));
				Assert.That(available.MergeRanges[0].End, Is.EqualTo(ci.Revision + 2));
				Assert.That(available.MergeRanges[0].Inheritable, Is.True);
				Assert.That(available.Target, Is.Not.Null);

				client.Merge(merge2, fromUri, available.MergeRanges[0]);

				client.GetAvailableMergeInfo(new SvnPathTarget(merge2), fromUri, out available);
				Assert.That(available, Is.Not.Null);
				Assert.That(available.MergeRanges.Count, Is.EqualTo(0));
				Assert.That(available.Target, Is.Not.Null);

				client.Commit(WcPath);

				client.GetAvailableMergeInfo(new SvnPathTarget(merge2), fromUri, out available);
				Assert.That(available, Is.Not.Null);
				Assert.That(available.MergeRanges.Count, Is.EqualTo(1));
				Assert.That(available.Target, Is.Not.Null);


				client.GetAppliedMergeInfo(new SvnPathTarget(merge2), out applied);

				Assert.That(applied, Is.Not.Null);
				Assert.That(applied.AppliedMerges.Count, Is.EqualTo(1));
				Assert.That(applied.AppliedMerges[0].Uri, Is.EqualTo(fromUri));
				Assert.That(applied.AppliedMerges[0].MergeRanges, Is.Not.Null);
				Assert.That(applied.AppliedMerges[0].MergeRanges.Count, Is.EqualTo(1));
				Assert.That(applied.AppliedMerges[0].MergeRanges[0].Start, Is.EqualTo(ci.Revision));
				Assert.That(applied.AppliedMerges[0].MergeRanges[0].End, Is.EqualTo(ci.Revision + 2));
				Assert.That(applied.AppliedMerges[0].MergeRanges[0].Inheritable, Is.True);
				Assert.That(applied.Target, Is.Not.Null);
			}
		}

		[Test]
		public void TestMultiMove()
		{
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
