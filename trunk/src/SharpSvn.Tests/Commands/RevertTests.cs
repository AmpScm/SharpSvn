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
	/// Tests Client::Revert 
	/// </summary>

	[TestFixture]
	public class RevertTests : TestBase
	{
		/// <summary>
		///Attempts to revert single file. 
		/// </summary>
		[Test]
		public void TestRevertFile()
		{
			string filePath = Path.Combine(this.WcPath, "Form.cs");

			string oldContents;
			string newContents;
			this.ModifyFile(out oldContents, out newContents, filePath, filePath, SvnDepth.Empty);


			Assert.That(newContents, Is.EqualTo(oldContents), "File not reverted");

		}

		/// <summary>
		///Attempts to revert the whole working copy 
		/// </summary>
		[Test]
		public void TestRevertDirectory()
		{
			string oldContents;
			string newContents;
			this.ModifyFile(out oldContents, out newContents, Path.Combine(this.WcPath, "Form.cs"),
				this.WcPath, SvnDepth.Infinity);

			Assert.That(newContents, Is.EqualTo(oldContents), "File not reverted");

		}

		private void ModifyFile(out string oldContents, out string newContents, string filePath,
			string revertPath, SvnDepth depth)
		{

			using (StreamReader reader = new StreamReader(filePath))
				oldContents = reader.ReadToEnd();
			using (StreamWriter writer = new StreamWriter(filePath))
				writer.WriteLine("mooooooo");

			SvnRevertArgs a = new SvnRevertArgs();
			a.Depth = depth;
			this.Client.Revert(revertPath, a);

			using (StreamReader reader = new StreamReader(filePath))
				newContents = reader.ReadToEnd();
		}

	}
}
