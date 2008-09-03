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
	/// Summary description for PropGetTest.
	/// </summary>
	[TestFixture]
	public class GetPropertyTests : TestBase
	{
		[Test]
		public void TestPropGetOnFile()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "ps foo bar " + path);

			string value;
			Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out value));

			Assert.That(value, Is.EqualTo("bar"));

			SvnPropertyValue pval;

			Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out pval));

			Assert.That(pval.StringValue, Is.EqualTo("bar"));
			Assert.That(pval.Key, Is.EqualTo("foo"));
			Assert.That(pval.Target.TargetName, Is.EqualTo(path));
		}

		[Test]
		public void TestNonExistentPropertyExistingFile()
		{
			string wc = GetTempDir();
			Client.CheckOut(new Uri(CollabReposUri, "trunk"), wc);

			SvnGetPropertyArgs pa = new SvnGetPropertyArgs();
			pa.ThrowOnError = false;

			string value;
			Assert.That(Client.GetProperty(Path.Combine(wc, "index.html"), 
				"new-prop", out value), Is.True, 
				"GetProperty succeeds on non existent property");

			Assert.That(value, Is.Null, "No value available");
		}

		[Test, ExpectedException(typeof(SvnEntryNotFoundException),
			ExpectedMessage="no-index.html", MatchType=MessageMatch.Contains)]
		public void TestNonExistentPropertyNonExistingFile()
		{
			string wc = GetTempDir();
			Client.CheckOut(new Uri(CollabReposUri, "trunk"), wc);

			SvnGetPropertyArgs pa = new SvnGetPropertyArgs();
			pa.ThrowOnError = false;

			string value;
			Client.GetProperty(Path.Combine(wc, "no-index.html"), "new-prop", out value);
		}

		[Test, ExpectedException(typeof(SvnInvalidNodeKindException), 
			ExpectedMessage="{632382A5-F992-4ab8-8D37-47977B190819}", MatchType=MessageMatch.Contains)]
		public void TestNoWcProperty()
		{
			string value;
			Client.GetProperty("c:/{632382A5-F992-4ab8-8D37-47977B190819}/no-file.txt", "no-prop", out value);
		}
	}
}



