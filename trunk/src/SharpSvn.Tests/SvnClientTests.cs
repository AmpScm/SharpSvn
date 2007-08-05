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
					client.Cat(new Uri(ReposUri, "folder/CatTest"), ms);

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
				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile1"), ia, out ee), Is.False);
				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile2"), ia, out ee), Is.False);
				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile3"), ia, out ee), Is.False);
				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile4"), ia, out ee), Is.False);

				TouchFile(file1);
				TouchFile(file2);
				TouchFile(file3);
				TouchFile(file4);

				client.Add(file1);
				client.Commit(WcPath);

				client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile1"), out iea);
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

				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile2"), ia, out ee), Is.True);
				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile3"), ia, out ee), Is.False);
				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile4"), ia, out ee), Is.False);

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

				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile3"), ia, out ee), Is.True);
				Assert.That(client.GetInfo(new Uri(ReposUri, "folder/ChangeListFile4"), ia, out ee), Is.True);
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

				client.RemoteCopy(new Uri(ReposUri, "folder/CopyBase"), new Uri(ReposUri, "folder/RemoteCopyBase"));
				bool visited = false;
				bool first = true;
				client.Log(new Uri(ReposUri, "folder/RemoteCopyBase"), delegate(object sender, SvnLogEventArgs e)
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
				client.Log(new Uri(ReposUri, "folder/LocalCopy"), delegate(object sender, SvnLogEventArgs e)
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

				client.RemoteImport(Path.Combine(TestPath, "MoveTestImport"), new Uri(ReposUri, "folder/RemoteMoveBase"));

				client.Commit(WcPath);

				Assert.That(ItemExists(new Uri(ReposUri, "folder/RemoteMoveBase")), Is.True, "Remote base does exist");
				Assert.That(ItemExists(new Uri(ReposUri, "folder/LocalMoveBase")), Is.True, "Local base does exist");

				client.RemoteMove(new Uri(ReposUri, "folder/RemoteMoveBase"), new Uri(ReposUri, "folder/RemoteMoveTarget"));
				client.Move(file, Path.Combine(WcPath, "LocalMoveTarget"));

				client.Commit(WcPath);

				Assert.That(ItemExists(new Uri(ReposUri, "folder/RemoteMoveTarget")), Is.True, "Remote target exists");
				Assert.That(ItemExists(new Uri(ReposUri, "folder/LocalMoveTarget")), Is.True, "Local target exists");
				Assert.That(ItemExists(new Uri(ReposUri, "folder/RemoteMoveBase")), Is.False, "Remote base does not exist");
				Assert.That(ItemExists(new Uri(ReposUri, "folder/LocalMoveBase")), Is.False, "Local base does not exist");
			}
		}

	}
}
