using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.IO;
using NUnit.Framework.SyntaxHelpers;

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

		static void ForcedDeleteDirectory(string dir)
		{
			foreach (FileInfo fif in new DirectoryInfo(dir).GetFiles("*", SearchOption.AllDirectories))
			{
				if ((int)(fif.Attributes & FileAttributes.ReadOnly) != 0)
					fif.Attributes = FileAttributes.Normal;
			}
			Directory.Delete(dir, true);
		}

		static void TouchFile(string filename)
		{
			TouchFile(filename, false);
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
				cra.RepositoryCompatibility = SvnRepositoryCompatibility.SubversionPre1_5;
				reposClient.CreateRepository(RepositoryPath, cra);
			}

			using (SvnClient client = new SvnClient())
			{
				client.RemoteMkDir(new Uri(ReposUri, "folder"));

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
		public void CreateTrunk()
		{
			using (SvnClient client = new SvnClient())
			{
				Uri trunkUri = new Uri(ReposUri, "trunk/");
				client.RemoteMkDir(trunkUri);

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
		public void RemoteListTest()
		{
			using (SvnClient client = new SvnClient())
			{
				IList<SvnListEventArgs> items;
				client.GetList(ReposUri, out items);

				Assert.That(items, Is.Not.Null, "Items retrieved");
				Assert.That(items.Count, Is.GreaterThan(0), "More than 0 items");
			}
		}

		public void InfoTest()
		{
			using (SvnClient client = new SvnClient())
			{
				IList<SvnInfoEventArgs> items;
				client.GetInfo(ReposUri, new SvnInfoArgs(), out items);

				Assert.That(items, Is.Not.Null, "Items retrieved");
				Assert.That(items.Count, Is.EqualTo(1), "1 info item");

				SvnInfoEventArgs info = items[0];
				Assert.That(info.Uri.ToString() + "/", Is.EqualTo(ReposUri.ToString()), "Repository uri matches");
				Assert.That(info.HasWcInfo, Is.False, "No WC info");
				Assert.That(info.Path, Is.EqualTo(""), "Path is empty");

				client.GetInfo(new Uri(ReposUri, "folder"), out info);

				Assert.That(info.Uri.ToString() + "/", Is.EqualTo(ReposUri.ToString()), "Repository uri matches");
				Assert.That(info.HasWcInfo, Is.False, "No WC info");
				Assert.That(info.Path, Is.EqualTo("folder"), "Path is empty");
			}
		}

		[Test]
		public void WcStatusTest()
		{
			using (SvnClient client = new SvnClient())
			{
				string file = Path.Combine(WcPath, "WcStatusTest");
				TouchFile(file);

				client.Add(file);

				IList<SvnStatusEventArgs> items;
				client.GetStatus(WcPath, out items);

				Assert.That(items, Is.Not.Null, "Status retrieved");
				Assert.That(items.Count, Is.GreaterThan(0), "More than 0 items");
			}
		}

		[Test]
		public void CatTest()
		{
			string data = Guid.NewGuid().ToString();
			using (SvnClient client = new SvnClient())
			{
				string file = Path.Combine(WcPath, "CatTest");
				using (StreamWriter sw = File.CreateText(file))
				{
					sw.WriteLine(data);
				}

				client.Add(file);
				client.AddToChangeList(file, "CatTest-Items");

				SvnCommitArgs ca = new SvnCommitArgs();
				ca.Changelist = "CatTest-Items";
				client.Commit(WcPath);

				using (MemoryStream ms = new MemoryStream())
				{
					client.Cat(new SvnPathTarget(file), ms);

					ms.Position = 0;

					using (StreamReader sr = new StreamReader(ms))
					{
						Assert.That(sr.ReadLine(), Is.EqualTo(data));
						Assert.That(sr.ReadToEnd(), Is.EqualTo(""));
					}
				}

				using (MemoryStream ms = new MemoryStream())
				{
					client.Cat(new Uri(WcUri, "CatTest"), ms);

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
			using (SvnClient client = new SvnClient())
			{
				SvnInfoArgs ia = new SvnInfoArgs();
				ia.ThrowOnError = false;
				SvnInfoEventArgs iea;
				IList<SvnInfoEventArgs> ee;
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

				IList<string> paths;
				client.GetChangeList("ChangeListTest-2", WcPath, out paths);

				Assert.That(paths, Is.Not.Null);
				Assert.That(paths.Count, Is.EqualTo(1));
				Assert.That(paths[0], Is.EqualTo(Path.GetFullPath(file2)));

				client.GetChangeList("ChangeListTest-4", WcPath, out paths);

				Assert.That(paths, Is.Not.Null);
				Assert.That(paths.Count, Is.EqualTo(0));

				SvnCommitArgs ca = new SvnCommitArgs();
				ca.Changelist = "ChangeListTest-2";

				client.Commit(WcPath, ca);

				Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile2"), ia, out ee), Is.True);
				Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile3"), ia, out ee), Is.False);
				Assert.That(client.GetInfo(new Uri(WcUri, "ChangeListFile4"), ia, out ee), Is.False);

				bool visited = false;
				client.ListChangeList("ChangeListTest-3", WcPath, delegate(object sender, SvnListChangeListEventArgs e)
				{
					Assert.That(e.Path, Is.EqualTo(file3));
					visited = true;
				});
				Assert.That(visited, Is.True);

				visited = false;
				client.Status(file4, delegate(object sender, SvnStatusEventArgs e)
				{
					Assert.That(e.FullPath, Is.EqualTo(file4));
					Assert.That(e.WcContentStatus, Is.EqualTo(SvnWcStatus.Added));
					Assert.That(e.IsOutOfDate, Is.False);
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
			using (SvnClient client = new SvnClient())
			{
				client.CleanUp(WcPath);
			}
		}

		[Test]
		public void CopyTest()
		{
			using (SvnClient client = new SvnClient())
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
						foreach (KeyValuePair<string, SvnLogChangeItem> i in e.ChangedPaths)
						{
							Assert.That(i.Key.EndsWith("folder/RemoteCopyBase"), "Path ends with folder/RemoteCopyBase");
							Assert.That(i.Value.Action, Is.EqualTo(SvnChangeAction.Add));
							Assert.That(i.Value.CopyFromPath.EndsWith("folder/CopyBase"), "CopyFromPath ensd with folder/CopyBase");
							Assert.That(i.Value.CopyFromRevision, Is.GreaterThan(0L));
						}
					}
					else
					{
						foreach (KeyValuePair<string, SvnLogChangeItem> i in e.ChangedPaths)
						{
							Assert.That(i.Value.Action, Is.EqualTo(SvnChangeAction.Add));
							Assert.That(i.Key.EndsWith("folder/CopyBase"), "Path ends with folder/CopyBase");
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
						foreach (KeyValuePair<string, SvnLogChangeItem> i in e.ChangedPaths)
						{
							Assert.That(i.Key.EndsWith("folder/LocalCopy"), "Path ends with folder/LocalCopy");
							Assert.That(i.Value.Action, Is.EqualTo(SvnChangeAction.Add));
							Assert.That(i.Value.CopyFromPath.EndsWith("folder/CopyBase"), "CopyFromPath ensd with folder/CopyBase");
							Assert.That(i.Value.CopyFromRevision, Is.GreaterThan(0L));
						}
						first = false;
					}
					else
						foreach (KeyValuePair<string, SvnLogChangeItem> i in e.ChangedPaths)
						{
							Assert.That(i.Value.Action, Is.EqualTo(SvnChangeAction.Add));
							Assert.That(i.Key.EndsWith("folder/CopyBase"), "Path ends with folder/CopyBase");
							visited = true;
						}
						
				});
				Assert.That(visited, "Visited local log item");
			}
		}

		bool ItemExists(Uri target)
		{
			bool found = false;
			using (SvnClient client = new SvnClient())
			{
				SvnInfoArgs args = new SvnInfoArgs();
				args.ThrowOnError = false;
				args.Depth = SvnDepth.Empty;
				return client.Info(target, delegate(object sender, SvnInfoEventArgs e)
				{
					found = true;
				}, args) && found;
			}					
		}

		[Test]
		public void MoveTest()
		{
			using (SvnClient client = new SvnClient())
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
		public void DeleteTest()
		{
			using (SvnClient client = new SvnClient())
			{
				string local = Path.Combine(WcPath, "LocalDeleteBase");
				string remote = Path.Combine(WcPath, "RemoteDeleteBase");

				TouchFile(local);
				client.Add(local);
				TouchFile(remote);
				client.Add(remote);

				client.Commit(WcPath);

				Assert.That(ItemExists(new Uri(WcUri, "LocalDeleteBase")), Is.True, "Remote base does exist");
				Assert.That(ItemExists(new Uri(WcUri, "RemoteDeleteBase")), Is.True, "Local base does exist");

				client.Delete(local);
				client.RemoteDelete(new Uri(WcUri, "RemoteDeleteBase"));

				bool visited = false;
				client.Status(local, delegate(object sender, SvnStatusEventArgs e)
				{
					Assert.That(e.FullPath, Is.EqualTo(local));
					Assert.That(e.WcContentStatus, Is.EqualTo(SvnWcStatus.Deleted));
					Assert.That(e.WcSwitched, Is.False);
					Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "LocalDeleteBase")));
					visited = true;
				});
				Assert.That(visited, "Visited handler");

				client.Commit(WcPath);

				Assert.That(ItemExists(new Uri(WcUri, "LocalDeleteBase")), Is.False, "Remote base does not exist");
				Assert.That(ItemExists(new Uri(WcUri, "RemoteDeleteBase")), Is.False, "Local base does not exist");
			}
		}

		[Test]
		public void DiffTests()
		{
			string start = Guid.NewGuid().ToString()+Environment.NewLine + Guid.NewGuid().ToString();
			string end = Guid.NewGuid().ToString()+Environment.NewLine + Guid.NewGuid().ToString();
			string origLine = Guid.NewGuid().ToString();
			string newLine = Guid.NewGuid().ToString();
			using (SvnClient client = new SvnClient())
			{
				string diffFile = Path.Combine(WcPath, "DiffTest");

				using (StreamWriter sw = File.CreateText(diffFile))
				{
					sw.WriteLine(start);
					sw.WriteLine(origLine);
					sw.WriteLine(end);
				}

				client.Add(diffFile);
				client.Commit(diffFile);

				using (StreamWriter sw = File.CreateText(diffFile))
				{
					sw.WriteLine(start);
					sw.WriteLine(newLine);
					sw.WriteLine(end);
				}

				FileStream diffOutput;

				client.Diff(new SvnPathTarget(diffFile, SvnRevisionType.Working), new SvnPathTarget(diffFile, SvnRevisionType.Head), out diffOutput);
				VerifyDiffOutput(origLine, newLine, diffOutput);

				client.Diff(new SvnPathTarget(diffFile, SvnRevisionType.Working), new SvnPathTarget(diffFile, SvnRevisionType.Committed), out diffOutput);
				VerifyDiffOutput(origLine, newLine, diffOutput);

				client.Diff(new SvnPathTarget(diffFile, SvnRevisionType.Working), new Uri(WcUri, "DiffTest"), out diffOutput);
				VerifyDiffOutput(origLine, newLine, diffOutput);
			}


		}

		private static void VerifyDiffOutput(string origLine, string newLine, FileStream diffOutput)
		{
			string path = diffOutput.Name;
			Assert.That(File.Exists(path));
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
			Assert.That(!File.Exists(path));
		}

		[Test]
		public void TestExport()
		{
			string exportDir = Path.Combine(TestPath, "ExportTests");
			if (Directory.Exists(exportDir))
				ForcedDeleteDirectory(exportDir);

			using (SvnClient client = new SvnClient())
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
			using (SvnClient client = new SvnClient())
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
					Assert.That(e.ContentTime, Is.EqualTo(new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)));
					Assert.That(e.CopyFromRev, Is.EqualTo(-1L));
					Assert.That(e.CopyFromUri, Is.Null);
					Assert.That(e.Depth, Is.EqualTo(SvnDepth.Infinity));
					Assert.That(e.FullPath, Is.EqualTo(file));
					Assert.That(e.HasWcInfo, Is.True);
					Assert.That(e.LastChangeAuthor, Is.Null);
					Assert.That(e.LastChangeDate, Is.EqualTo(DateTime.MinValue));
					Assert.That(e.LastChangeRevision, Is.EqualTo(0)); // Not committed yet
					Assert.That(e.Lock, Is.Null);
					Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
					Assert.That(e.Path, Is.Not.Null);
					Assert.That(e.PropertyEditFile, Is.Null);
					Assert.That(e.PropertyTime, Is.EqualTo(e.ContentTime));
					Assert.That(e.RepositorySize, Is.EqualTo(-1L));
					Assert.That(e.ReposRoot, Is.EqualTo(ReposUri));
					Assert.That(e.Revision, Is.EqualTo(0L)); // Not committed yet
					Assert.That(e.Schedule, Is.EqualTo(SvnWcSchedule.Add));
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
					Assert.That(e.ContentTime, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(1, 0, 0)));
					Assert.That(e.CopyFromRev, Is.EqualTo(-1L));
					Assert.That(e.CopyFromUri, Is.Null);
					Assert.That(e.Depth, Is.EqualTo(SvnDepth.Infinity));
					Assert.That(e.FullPath, Is.EqualTo(file));
					Assert.That(e.HasWcInfo, Is.True);
					Assert.That(e.LastChangeAuthor, Is.EqualTo(Environment.UserName));
					Assert.That(e.LastChangeDate, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(1, 0, 0)));
					Assert.That(e.LastChangeRevision, Is.EqualTo(commitData.Revision));
					Assert.That(e.Lock, Is.Null);
					Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
					Assert.That(e.Path, Is.Not.Null);
					Assert.That(e.PropertyEditFile, Is.Null);
					//Assert.That(e.PropertyTime, Is.EqualTo(e.ContentTime)); // Not static, might change
					Assert.That(e.RepositorySize, Is.EqualTo(-1L));
					Assert.That(e.ReposRoot, Is.EqualTo(ReposUri));
					Assert.That(e.Revision, Is.EqualTo(commitData.Revision));
					Assert.That(e.Schedule, Is.EqualTo(SvnWcSchedule.None));
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
					Assert.That(e.HasWcInfo, Is.False);
					Assert.That(e.LastChangeAuthor, Is.EqualTo(Environment.UserName));
					Assert.That(e.LastChangeDate, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(1, 0, 0)));
					Assert.That(e.LastChangeRevision, Is.EqualTo(commitData.Revision));
					Assert.That(e.Lock, Is.Null);
					Assert.That(e.NodeKind, Is.EqualTo(SvnNodeKind.File));
					Assert.That(e.Path, Is.Not.Null);
					Assert.That(e.PropertyEditFile, Is.Null);
					Assert.That(e.PropertyTime, Is.EqualTo(e.ContentTime));
					Assert.That(e.RepositorySize, Is.EqualTo(0L));
					Assert.That(e.ReposRoot, Is.EqualTo(ReposUri));
					Assert.That(e.Revision, Is.EqualTo(commitData.Revision));
					Assert.That(e.Schedule, Is.EqualTo(SvnWcSchedule.None));
					Assert.That(e.Uri, Is.EqualTo(new Uri(WcUri, "InfoFile")));
					Assert.That(e.WorkingCopySize, Is.EqualTo(-1L));
					visited = true;
				});
				Assert.That(visited);
			}
		}
	}
}
