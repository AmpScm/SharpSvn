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
		/// <summary>
		/// Test export operation from a repository
		/// </summary>
		[Test]
		public void TestExportRepos()
		{
            string wc = Path.Combine(GetTempDir(), "wc");
			this.Client.Export(new SvnUriTarget(ReposUrl, SvnRevision.Head), wc);

			Assert.That(File.Exists(Path.Combine(wc, "Form.cs")),
				"Exported file not there");
			Assert.That(!Directory.Exists(Path.Combine(wc, SvnClient.AdministrativeDirectoryName)),
				".svn directory found");
		}
		/// <summary>
		/// Test export operation from a working copy
		/// </summary>
		[Test]
		public void TestExportWc()
		{
            string wc = Path.Combine(GetTempDir(), "wc");
			this.Client.Export(WcPath, wc);

			Assert.That(File.Exists(Path.Combine(wc, "Form.cs")),
				"Exported file not there");
			Assert.That(!Directory.Exists(Path.Combine(wc, SvnClient.AdministrativeDirectoryName)),
				".svn directory found");
		}

		[Test]
		public void TestExportNonRecursive()
		{
            string wc = Path.Combine(GetTempDir(), "wc");
			SvnExportArgs a = new SvnExportArgs();
			a.Depth = SvnDepth.Empty;
			SvnUpdateResult r;
			this.Client.Export(WcPath, wc, a, out r);
			Assert.That(Directory.GetDirectories(wc).Length, Is.EqualTo(0));
		}
	}
}
