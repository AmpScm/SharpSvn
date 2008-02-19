// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System.IO;
using System.Text;
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::ClientPropSet
	/// </summary>
	[TestFixture]
	public class SetPropertyTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractWorkingCopy();
		}

		/// <summary>
		///Attempts to Set Properties on a file
		/// </summary>
		[Test]
		public void TestSetProp()
		{
			string filePath = Path.Combine(this.WcPath, "Form.cs");

			byte[] propval = Encoding.UTF8.GetBytes("baa");
			this.Client.SetProperty(filePath, "moo", propval);
			Assert.AreEqual("baa", this.RunCommand("svn", "propget moo " + filePath).Trim(),
				"PropSet didn't work!");
		}

		/// <summary>
		///Attempts to set Properties on a directory recursively. 
		/// </summary>
		[Test]
		public void TestSetPropRecursivly()
		{
			string filePath = Path.Combine(this.WcPath, "Form.cs");

			byte[] propval = Encoding.UTF8.GetBytes("baa");
			SvnSetPropertyArgs a = new SvnSetPropertyArgs();
			a.Depth = SvnDepth.Infinity;

			this.Client.SetProperty(WcPath, "moo", propval, a);

			Assert.AreEqual("baa", this.RunCommand("svn", "propget moo " + this.WcPath).Trim(),
				"PropSet didn't work on directory!");

			Assert.AreEqual("baa", this.RunCommand("svn", "propget moo " + filePath).Trim(),
				"PropSet didn't work on file!");
		}

	}

}
