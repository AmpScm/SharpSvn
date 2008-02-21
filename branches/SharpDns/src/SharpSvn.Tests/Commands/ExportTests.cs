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
	/// Tests for the Client::Export method
	/// </summary>
	[TestFixture]
	public class ExportTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractRepos();
			this.ExtractWorkingCopy();
			this.newWc = this.FindDirName(Path.Combine(Path.GetTempPath(), "moo"));
		}

		[TearDown]
		public override void TearDown()
		{
			base.TearDown();
			Directory.Delete(this.newWc, true);
		}

		/// <summary>
		/// Test export operation from a repository
		/// </summary>
		[Test]
		public void TestExportRepos()
		{
			this.Client.Export(new SvnUriTarget(ReposUrl, SvnRevision.Head), this.newWc);

			Assert.That(File.Exists(Path.Combine(this.newWc, "Form.cs")),
				"Exported file not there");
			Assert.That(!Directory.Exists(Path.Combine(this.newWc, SvnClient.AdministrativeDirectoryName)),
				".svn directory found");
		}
		/// <summary>
		/// Test export operation from a working copy
		/// </summary>
		[Test]
		public void TestExportWc()
		{
			this.Client.Export(new SvnPathTarget(this.WcPath), this.newWc);

			Assert.That(File.Exists(Path.Combine(this.newWc, "Form.cs")),
				"Exported file not there");
			Assert.That(!Directory.Exists(Path.Combine(this.newWc, SvnClient.AdministrativeDirectoryName)),
				".svn directory found");
		}

		[Test]
		public void TestExportNonRecursive()
		{
			SvnExportArgs a = new SvnExportArgs();
			a.Depth = SvnDepth.Empty;
			SvnUpdateResult r;
			this.Client.Export(this.WcPath, this.newWc, a, out r);
			Assert.That(Directory.GetDirectories(this.newWc).Length, Is.EqualTo(0));
		}

		private string newWc;

	}
}
