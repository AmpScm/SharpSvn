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
	/// Tests for the Client::Checkout method
	/// </summary>
	[TestFixture]
	public class DeleteTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
		}

		[TearDown]
		public override void TearDown()
		{
			base.TearDown();
		}


		/// <summary>
		/// Tests deleting files in a working copy
		/// </summary>
		[Test]
		public void TestDeleteWCFiles()
		{
			string path1 = Path.Combine(this.WcPath, "Form.cs");
			string path2 = Path.Combine(this.WcPath, "AssemblyInfo.cs");

			SvnDeleteArgs a = new SvnDeleteArgs();

			Assert.That(Client.Delete(new string[] { path1, path2 }, a));

			Assert.That(!File.Exists(path1), "File not deleted");
			Assert.That(!File.Exists(path2), "File not deleted");

			Assert.That(this.GetSvnStatus(path1), Is.EqualTo('D'), "File not deleted");
			Assert.That(this.GetSvnStatus(path2), Is.EqualTo('D'), "File not deleted");

            Assert.That(Client.Commit(WcPath));
		}

        [Test]
        public void TestReplacedStatus()
        {
            string file = CreateTextFile("ToBeReplaced.vb");

            Client.Add(file);
            Assert.That(Client.Commit(WcPath));
            Client.Delete(file);

            file = CreateTextFile("ToBeReplaced.vb");
            Assert.That(Client.Add(file));

            bool reached = false;
            Assert.That(Client.Status(file, delegate(object sender, SvnStatusEventArgs e)
            {
                reached = true;
                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Replaced));
            }));
            Assert.That(reached);

            Assert.That(Client.Commit(WcPath));
        }

		/// <summary>
		/// Tests deleting a directory in the repository
		/// </summary>
		//TODO: Implement the variable admAccessBaton
		[Test]
		public void TestDeleteFromRepos()
		{
			Uri path1 = new Uri(this.ReposUrl, "doc");
			Uri path2 = new Uri(this.ReposUrl, "Form.cs");

			SvnCommitResult ci;
			SvnDeleteArgs a = new SvnDeleteArgs();

			Assert.That(Client.RemoteDelete(new Uri[] { path1, path2 }, a, out ci));

			String cmd = this.RunCommand("svn", "list " + this.ReposUrl);
			Assert.That(cmd.IndexOf("doc") == -1, "Directory wasn't deleted ");
			Assert.That(cmd.IndexOf("Form.cs") == -1, "Directory wasn't deleted");

			Assert.That(ci, Is.Not.Null, "CommitInfo is invalid");
		}

		[Test]
		public void TestForceDelete()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");

			// modify the file
			using (StreamWriter writer = new StreamWriter(path, true))
			{
				writer.WriteLine("Hi ho");
			}

			// this will throw if force doesn't work
			SvnDeleteArgs a = new SvnDeleteArgs();
			a.ThrowOnError = false;

			Assert.That(Client.Delete(path, a), Is.False);

			a.ThrowOnError = true;
			a.Force = true;

			Assert.That(Client.Delete(path, a), Is.True, "Delete failed");
		}

	}
}
