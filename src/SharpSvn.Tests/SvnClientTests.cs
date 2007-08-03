using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.IO;

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
				if(_reposUri == null)
					_reposUri = new Uri("file:///" + RepositoryPath.Replace(Path.DirectorySeparatorChar, '/')+"/");

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
		
		[TestFixtureSetUp]
		public void SetupRepository()
		{
			if (Directory.Exists(RepositoryPath))
			{
				foreach (FileInfo fif in new DirectoryInfo(RepositoryPath).GetFiles("*", SearchOption.AllDirectories))
				{
					if ((int)(fif.Attributes & FileAttributes.ReadOnly) != 0)
						fif.Attributes = FileAttributes.Normal;
				}
				Directory.Delete(RepositoryPath, true);
			}
			if (Directory.Exists(WcPath))
			{
				foreach (FileInfo fif in new DirectoryInfo(WcPath).GetFiles("*", SearchOption.AllDirectories))
				{
					if ((int)(fif.Attributes & FileAttributes.ReadOnly) != 0)
						fif.Attributes = FileAttributes.Normal;
				}

				Directory.Delete(WcPath, true);
			}
			Directory.CreateDirectory(WcPath);

			using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
			{
				reposClient.CreateRepository(RepositoryPath);
			}
		}

		[TestFixtureTearDown]
		public void DestroyRepository()
		{
			using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
			{
				reposClient.DeleteRepository(RepositoryPath);
			}
		}

		[Test]
		public void CreateTrunk()
		{
			using (SvnClient client = new SvnClient())
			{
				Uri trunkUri = new Uri(ReposUri, "trunk/");
				client.RemoteMkDir(trunkUri, new SvnMkDirArgs());

				client.CheckOut(trunkUri, Path.Combine(WcPath, "trunk"));
			}
		}


    }
}
