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
			this.ExtractRepos();
			this.newWc = this.FindDirName(Path.Combine(Path.GetTempPath(), "moo"));
		}

		[TearDown]
		public override void TearDown()
		{
			base.TearDown();
			RecursiveDelete(this.newWc);
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
