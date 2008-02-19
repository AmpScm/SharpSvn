// $Id$
using System;
using System.IO;
using NUnit.Framework;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests for the Client::Checkout method
	/// </summary>
	[TestFixture]
	public class CheckoutTest : TestBase
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

			Assert.IsTrue(File.Exists(Path.Combine(this.newWc, "Form.cs")),
				"Checked out file not there");
			Assert.IsTrue(Directory.Exists(Path.Combine(this.newWc, SvnClient.AdministrativeDirectoryName)),
				"No admin directory found");
			Assert.AreEqual("", this.RunCommand("svn", "st \"" + this.newWc + "\"").Trim(),
				"Wrong status");
		}

		[Test]
		public void TestProgressEvent()
		{
			SvnCheckOutArgs a = new SvnCheckOutArgs();

			this.Client.Progress += new EventHandler<SvnProgressEventArgs>(Client_Progress);
			this.Client.CheckOut(new Uri("http://ankhsvn.com/svn/test"), this.newWc, a);

			Assert.IsTrue(progressCalled, "Progress delegate not called");
		}

		void Client_Progress(object sender, SvnProgressEventArgs args)
		{
			progressCalled = true;
		}

		private bool progressCalled = false;
		private string newWc;

	}
}
