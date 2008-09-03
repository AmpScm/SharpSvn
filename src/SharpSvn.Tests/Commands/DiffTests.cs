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
	/// Tests Client::Diff
	/// </summary>
	[TestFixture]
	public class DiffTests : TestBase
	{
		[Test]
		public void TestLocalDiff()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");


			using (StreamWriter w = new StreamWriter(form, false))
				w.Write("Moo moo moo moo moo\r\nmon\r\nmooo moo moo \r\nssdgo");

			//Necessary to fix up headers

			string clientDiff = this.RunCommand("svn", "diff \"" + form + "\"");

			MemoryStream outstream = new MemoryStream();
			MemoryStream errstream = new MemoryStream();

			SvnDiffArgs a = new SvnDiffArgs();
			a.ErrorStream = errstream;
			this.Client.Diff(
				new SvnPathTarget(form, SvnRevision.Base),
				new SvnPathTarget(form, SvnRevision.Working),
				a, outstream);

			string err = Encoding.Default.GetString(errstream.ToArray());
			Assert.That(err, Is.EqualTo(""), "Error in diff: " + err);
			string apiDiff = Encoding.Default.GetString(outstream.ToArray());
			Assert.That(apiDiff, Is.EqualTo(clientDiff), "Client diff differs");
		}

		[Test]
		public void TestReposDiff()
		{
			string clientDiff = this.RunCommand("svn", "diff -r 1:5 " + this.ReposUrl);

			MemoryStream outstream = new MemoryStream();
			MemoryStream errstream = new MemoryStream();

			SvnDiffArgs a = new SvnDiffArgs();
			a.ErrorStream = errstream;
			this.Client.Diff(
				new SvnUriTarget(this.ReposUrl, 1),
				new SvnUriTarget(this.ReposUrl, 5),
				a, outstream);

			string err = Encoding.Default.GetString(errstream.ToArray());
			Assert.That(err, Is.EqualTo(string.Empty), "Error in diff: " + err);

			string apiDiff = Encoding.Default.GetString(outstream.ToArray());
			Assert.That(apiDiff, Is.EqualTo(clientDiff), "Diffs differ");
		}

		[Test]
		public void TestDiffBinary()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "propset svn:mime-type application/octet-stream " +
				path);
			this.RunCommand("svn", "ci -m '' " + path);

			using (StreamWriter w = new StreamWriter(path))
				w.WriteLine("Hi there");


			MemoryStream outstream = new MemoryStream();
			MemoryStream errstream = new MemoryStream();

			SvnDiffArgs a = new SvnDiffArgs();
			a.ErrorStream = errstream;

			// this should not diff a binary file
			Assert.That(Client.Diff(
				path,
				new SvnRevisionRange(SvnRevision.Base, SvnRevision.Working),
				a,
				outstream));

			string diff = Encoding.ASCII.GetString(outstream.ToArray());
			Assert.That(diff.IndexOf("application/octet-stream") >= 0);


			outstream = new MemoryStream();
			errstream = new MemoryStream();
			a = new SvnDiffArgs();
			a.ErrorStream = errstream;
			a.IgnoreContentType = true;

			this.Client.Diff(
				path,
				new SvnRevisionRange(SvnRevision.Base, SvnRevision.Working),
				a,
				outstream);

			Assert.That(outstream.Length > 0);
			diff = Encoding.ASCII.GetString(outstream.ToArray());
			Assert.That(diff.IndexOf("application/octet-stream") < 0);
		}


	}
}
