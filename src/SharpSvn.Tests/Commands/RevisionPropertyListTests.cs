// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using NUnit.Framework;
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

			int headRev = int.Parse(this.RunCommand("svnlook", "youngest " + this.ReposPath));

			SvnPropertyCollection spc;
			Assert.That(Client.GetRevisionPropertyList(new SvnUriTarget(ReposUrl, SvnRevision.Head), out spc));


			//Assert.AreEqual( headRev, rev, "Revision wrong" );
			Assert.AreEqual("bar", spc["foo"].ToString(), "Wrong property value");
			Assert.AreEqual("foo", spc["kung"].ToString(), "Wrong property value");
		}
	}
}
