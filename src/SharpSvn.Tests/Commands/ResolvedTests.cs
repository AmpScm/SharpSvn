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
	/// Tests Client::Resolve
	/// </summary>
	[TestFixture]
	public class ResolveTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.path = this.GetTempFile();
			UnzipToFolder(Path.Combine(ProjectBase, "Zips/conflictwc.zip"), path);
			this.RenameAdminDirs(this.path);
		}

		[TearDown]
		public override void TearDown()
		{
			base.TearDown();
			RecursiveDelete(path);
		}

		/// <summary>
		///Attempts to resolve a conflicted file. 
		/// </summary>
		[Test]
		public void TestResolveFile()
		{
			string filePath = Path.Combine(this.path, "Form.cs");

			this.Client.Resolved(filePath);

			Assert.That(this.GetSvnStatus(filePath), Is.EqualTo('M'), "Resolve didn't work!");

		}

		/// <summary>
		///Attempts to resolve a conflicted directory recursively. 
		/// </summary>
		[Test]
		public void TestResolveDirectory()
		{
			SvnResolveArgs a = new SvnResolveArgs();
			a.Depth = SvnDepth.Infinity;

			this.Client.Resolve(this.path, SvnAccept.Merged, a);

			Assert.That(this.GetSvnStatus(this.path), Is.EqualTo('M'),
				" Resolve didn't work! Directory still conflicted");
			Assert.That(this.GetSvnStatus(Path.Combine(this.path, "Form.cs")), Is.EqualTo('M'),
				"Resolve didn't work! File still conflicted");
		}

		private string path;
	}

}
