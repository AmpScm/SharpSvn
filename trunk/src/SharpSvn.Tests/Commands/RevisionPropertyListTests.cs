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
	/// Tests Client::RevPropList
	/// </summary>
	public class RevPropListTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

			this.ExtractRepos();
		}

		/// <summary>
		/// Sets two properties on a repos and tries to retrieve them with Client::RevPropList
		/// </summary>
		[Test]
		public void TestBasic()
		{
			this.RunCommand("svn", "ps --revprop -r HEAD foo bar " + this.ReposUrl);
			this.RunCommand("svn", "ps --revprop -r HEAD kung foo " + this.ReposUrl);

			SvnPropertyCollection spc;
			Assert.That(Client.GetRevisionPropertyList(new SvnUriTarget(ReposUrl, SvnRevision.Head), out spc));

			Assert.That(spc["foo"].ToString(), Is.EqualTo("bar"), "Wrong property value");
			Assert.That(spc["kung"].ToString(), Is.EqualTo("foo"), "Wrong property value");
		}
	}
}
