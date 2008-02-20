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
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractWorkingCopy();
		}

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
			Assert.That(pval.Target.TargetName, Is.EqualTo(path.Replace(Path.DirectorySeparatorChar, '/')));
		}
	}
}



