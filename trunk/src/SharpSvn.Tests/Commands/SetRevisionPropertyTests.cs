// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Text;
using NUnit.Framework;

/// <summary>
/// Tests the Client::RevPropSet	
/// </summary>

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class SetRevisionPropertyTests : TestBase
	{

		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractWorkingCopy();
			this.ExtractRepos();
		}

		/// <summary>
		///Attempts to Set Properties on a file in the repository represented by url. 
		/// </summary>
		[Test]
		public void TestRevSetPropDir()
		{
			byte[] propval = Encoding.UTF8.GetBytes("moo");

			this.Client.SetRevisionProperty(new SvnUriTarget(ReposUrl, SvnRevision.Head), "cow", propval);

			Assert.AreEqual("moo", this.RunCommand("svn", "propget cow --revprop -r head " + this.ReposUrl).Trim(),
				"Couldn't set prop on selected Repos!");
		}
	}
}
