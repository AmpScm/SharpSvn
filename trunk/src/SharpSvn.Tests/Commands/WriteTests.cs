// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::Cat
	/// </summary>
	[TestFixture]
	public class WriteTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractRepos();
			this.ExtractWorkingCopy();
		}

		/// <summary>
		/// Attemts to do a cat on a local working copy item
		/// </summary>
		[Test]
		public void TestCatFromWorkingCopy()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");

			string clientOutput = this.RunCommand("svn", "cat " + path);

			MemoryStream stream = new MemoryStream();
			this.Client.Write(path, stream);

			string wrapperOutput = Encoding.ASCII.GetString(stream.ToArray());
			Assert.That(wrapperOutput, Is.EqualTo(clientOutput),
				"String from wrapper not the same as string from client");

		}

		/// <summary>
		/// Calls cat on a repository item
		/// </summary>
		[Test]
		public void TestCatFromRepository()
		{
			Uri path = new Uri(this.ReposUrl, "Form.cs");

			string clientOutput = this.RunCommand("svn", "cat " + path);

			MemoryStream stream = new MemoryStream();
			this.Client.Write(new SvnUriTarget(path, SvnRevision.Head), stream);

			string wrapperOutput = Encoding.ASCII.GetString(stream.ToArray());
			Assert.That(wrapperOutput, Is.EqualTo(clientOutput),
				"String from wrapper not the same as string from client");
		}

		[Test]
		public void TestCatPeg()
		{
			Uri path = new Uri(this.ReposUrl, "Form.cs");
			Uri toPath = new Uri(this.ReposUrl, "Moo.cs");

			SvnCommitResult ci;
			this.Client.RemoteMove(path, toPath, out ci);

			string clientOutput = this.RunCommand("svn",
				string.Format("cat {0}@{1} -r {2}", toPath, ci.Revision, ci.Revision - 1));

			MemoryStream stream = new MemoryStream();
			SvnWriteArgs a = new SvnWriteArgs();
			a.Revision = ci.Revision - 1;
			this.Client.Write(new SvnUriTarget(toPath, ci.Revision), stream);

			string wrapperOutput = Encoding.ASCII.GetString(stream.ToArray());
			Assert.That(wrapperOutput, Is.EqualTo(clientOutput),
				"String from wrapper not the same as string from client");

		}
	}
}
