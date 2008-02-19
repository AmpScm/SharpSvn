// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using NUnit.Framework;
using System;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the NSvn.Client.MoveFile method
	/// </summary>
	[TestFixture]
	public class MoveTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

			this.ExtractRepos();
			this.ExtractWorkingCopy();
		}

		/// <summary>
		/// Tests moving a file in WC
		/// </summary>
		[Test]
		public void TestMoveWCFile()
		{
			string srcPath = Path.Combine(this.WcPath, "Form.cs");
			string dstPath = Path.Combine(this.WcPath, "renamedForm.cs");

			Assert.That(Client.Move(srcPath, dstPath));

			Assert.IsTrue(File.Exists(dstPath), "File wasn't moved");
			Assert.IsTrue(!File.Exists(srcPath), "Source File still exists");

		}

		/// <summary>
		/// Tests moving a directory in a WC
		/// </summary>
		[Test]
		public void TestMoveWCDir()
		{
			string srcPath = Path.Combine(this.WcPath, @"bin\Debug");
			string dstPath = Path.Combine(this.WcPath, @"renamedDebug");

			Assert.That(Client.Move(srcPath, dstPath));

			Assert.IsTrue(Directory.Exists(dstPath), "Directory wasn't moved");
			Assert.AreEqual('D', this.GetSvnStatus(srcPath), "Status is not 'D'");
		}

		/// <summary>
		/// Tests moving a file in a Repos
		/// </summary>
		[Test]
		public void TestMoveReposFile()
		{
			Uri srcPath = new Uri(this.ReposUrl, "Form.cs");
			Uri dstPath = new Uri(this.ReposUrl, "renamedForm");

			SvnCommitResult ci;

			Assert.That(Client.RemoteMove(srcPath, dstPath, out ci));

			Assert.That(ci, Is.Not.Null);

			String cmd = this.RunCommand("svn", "list " + this.ReposUrl);
			Assert.IsTrue(cmd.IndexOf("Form.cs") == -1, "File wasn't moved");
			Assert.IsTrue(cmd.IndexOf("renamedForm") >= 0, "Moved file doens't exist");
		}
	}
}
