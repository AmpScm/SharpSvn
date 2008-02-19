// $Id$
using System;
using NUnit.Framework;
using SharpSvn;
using System.Text;

/// <summary>
/// Tests the Client::RevPropGet	
/// </summary>

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class GetRevisionPropertyTest : TestBase
	{

		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractWorkingCopy();
			this.ExtractRepos();
		}

		/// <summary>
		///Attempts to Get Properties on a directory in the repository represented by url. 
		/// </summary>
		[Test]
		public void TestRevPropGetDir()
		{

			int headRev = int.Parse(this.RunCommand("svnlook", "youngest " + this.ReposPath));

			this.RunCommand("svn", "ps --revprop -r HEAD cow moo " + this.ReposUrl);

			string value;
			Assert.That(Client.GetRevisionProperty(this.ReposUrl, "cow", out value));

			Assert.AreEqual("moo", value, "Wrong property value");
		}
	}
}