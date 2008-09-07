// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the NSvn.Client.MakeDir method
	/// </summary>
	[TestFixture]
	public class CreateDirectoryTests : TestBase
	{
		/// <summary>
		/// Tests creating a directory in the working copy
		/// </summary>
		[Test]
		public void TestMakeLocalDir()
		{
			string path = Path.Combine(this.WcPath, "foo");
			Assert.That(Client.CreateDirectory(path));

			Assert.That(this.GetSvnStatus(path), Is.EqualTo('A'), "Wrong status code");
		}

		/// <summary>
		/// Tests creating a directory in the repository
		/// </summary>
		[Test]
		public void TestMakeRepositoryDir()
		{
			Uri url = new Uri(ReposUrl, "mooNewDirectory/");

			Assert.That(Client.RemoteCreateDirectory(url));

			string output = this.RunCommand("svn", "ls " + this.ReposUrl);
			Assert.That(Regex.IsMatch(output, @"mooNewDirectory/"), "No new dir found: " + output);

		}

        [Test]
        public void CreateTrunk()
        {
            using (SvnClient client = NewSvnClient(true, false))
            {
                Uri trunkUri = new Uri(ReposUrl, "trunk/");
                client.RemoteCreateDirectory(trunkUri);

                string trunkPath = GetTempDir();

                client.CheckOut(trunkUri, trunkPath);

                TouchFile(Path.Combine(trunkPath, "test.txt"));

                Assert.That(SvnTools.IsManagedPath(trunkPath));
                Assert.That(SvnTools.IsBelowManagedPath(trunkPath));
                Assert.That(SvnTools.IsBelowManagedPath(Path.Combine(trunkPath, "q/r/s/t/u/v/test.txt")));

                client.Add(Path.Combine(trunkPath, "test.txt"));

                TouchFile(Path.Combine(trunkPath, "dir/test.txt"), true);

                SvnAddArgs aa = new SvnAddArgs();
                aa.AddParents = true;
                client.Add(Path.Combine(trunkPath, "dir/test.txt"), aa);

                client.Commit(trunkPath);

                client.RemoteDelete(trunkUri, new SvnDeleteArgs());
            }
        }
	}
}
