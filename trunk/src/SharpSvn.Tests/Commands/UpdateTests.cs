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
	/// Tests Client::Update
	/// </summary>
	[TestFixture]
	public class UpdateTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractRepos();
			this.ExtractWorkingCopy();
			this.wc2 = this.FindDirName(Path.Combine(TestBase.BASEPATH, TestBase.WC_NAME));

			UnzipToFolder(Path.Combine(ProjectBase, "Zips/wc.zip"), wc2);
			this.RenameAdminDirs(this.wc2);

			SvnClient cl = new SvnClient(); // Fix working copy to real location
			cl.Relocate(wc2, new Uri("file:///tmp/repos/"), ReposUrl);
		}

		public override void TearDown()
		{
			base.TearDown();
			RecursiveDelete(this.wc2);
		}

		/// <summary>
		/// Deletes a file, then calls update on the working copy to restore it 
		/// from the text-base
		/// </summary>
		[Test]
		public void TestDeletedFile()
		{
			string filePath = Path.Combine(this.WcPath, "Form.cs");
			File.Delete(filePath);
			this.Client.Update(this.WcPath);

			Assert.That(File.Exists(filePath), "File not restored after update");
		}

		/// <summary>
		/// Changes a file in a secondary working copy and commits. Updates the 
		/// primary wc and compares
		/// </summary>
		[Test]
		public void TestChangedFile()
		{
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "Form.cs")))
				w.Write("Moo");
			this.RunCommand("svn", "ci -m \"\" " + this.wc2);

			this.Client.Update(this.WcPath);

			string s;
			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "Form.cs")))
				s = r.ReadToEnd();

			Assert.That(s, Is.EqualTo("Moo"), "File not updated");
		}

		[Test]
		public void TestUpdateMultipleFiles()
		{
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "Form.cs")))
				w.Write("Moo");
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "AssemblyInfo.cs")))
				w.Write("Moo");
			this.RunCommand("svn", "ci -m \"\" " + this.wc2);

			SvnUpdateArgs a = new SvnUpdateArgs();
			a.Depth = SvnDepth.Empty;

			SvnUpdateResult result;

			Assert.That(Client.Update(new string[]{ 
                                                             Path.Combine( this.WcPath, "Form.cs" ),
                                                             Path.Combine( this.WcPath, "AssemblyInfo.cs" )
                                                         }, a, out result));
			Assert.That(result.ResultMap.Count, Is.EqualTo(2));

			string s;
			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "Form.cs")))
				s = r.ReadToEnd();
			Assert.That(s, Is.EqualTo("Moo"), "File not updated");

			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "AssemblyInfo.cs")))
				s = r.ReadToEnd();
			Assert.That(s, Is.EqualTo("Moo"), "File not updated");
		}

		private string wc2;
	}
}
