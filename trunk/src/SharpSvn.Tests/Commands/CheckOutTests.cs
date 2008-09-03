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
	public class CheckOutTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.newWc = this.FindDirName(Path.Combine(Path.GetTempPath(), "moo"));
		}

		[TearDown]
		public override void TearDown()
		{
			base.TearDown();
			RecursiveDelete(this.newWc);
		}

		[Test]
		public void TestPegCheckOuts()
		{
			Uri repos = new Uri(GetReposUri(TestReposType.CollabRepos), "trunk/");

			SvnUpdateResult result;
			Assert.That(Client.CheckOut(repos, GetTempDir(), out result));

			Assert.That(result, Is.Not.Null);
			long head = result.Revision;

			Assert.That(Client.CheckOut(new SvnUriTarget(repos, head - 10), GetTempDir(), out result));
			Assert.That(result.Revision, Is.EqualTo(head - 10));

			SvnCheckOutArgs a = new SvnCheckOutArgs();
			a.Revision = head - 5;

			Assert.That(Client.CheckOut(new SvnUriTarget(repos, head - 10), GetTempDir(), a, out result));
			Assert.That(result.Revision, Is.EqualTo(head - 5));

			Assert.That(Client.CheckOut(repos, GetTempDir(), a, out result));
			Assert.That(result.Revision, Is.EqualTo(head - 5));
		}


		/// <summary>
		/// Test a standard checkout operation
		/// </summary>
		[Test]
		public void TestBasicCheckout()
		{
			SvnCheckOutArgs a = new SvnCheckOutArgs();
			this.Client.CheckOut(this.ReposUrl, this.newWc, a);

			Assert.That(File.Exists(Path.Combine(this.newWc, "Form.cs")),
				"Checked out file not there");
			Assert.That(Directory.Exists(Path.Combine(this.newWc, SvnClient.AdministrativeDirectoryName)),
				"No admin directory found");
			Assert.That(this.RunCommand("svn", "st \"" + this.newWc + "\"").Trim(), Is.EqualTo(""),
				"Wrong status");
		}

		[Test]
		public void TestProgressEvent()
		{
			SvnCheckOutArgs a = new SvnCheckOutArgs();

			this.Client.Progress += new EventHandler<SvnProgressEventArgs>(Client_Progress);
			this.Client.CheckOut(new Uri("http://ankhsvn.com/svn/test"), this.newWc, a);

			Assert.That(progressCalled, "Progress delegate not called");
		}

		void Client_Progress(object sender, SvnProgressEventArgs args)
		{
			progressCalled = true;
		}

		private bool progressCalled = false;
		private string newWc;
	}
}
