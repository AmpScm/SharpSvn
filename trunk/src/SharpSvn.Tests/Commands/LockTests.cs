// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.IO;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Summary description for LockTest.
	/// </summary>
	[TestFixture]
	public class LockTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractRepos();
			this.ExtractWorkingCopy();
		}

		[Test]
		public void TestBasicLock()
		{
			string filepath = Path.Combine(this.WcPath, "Form.cs");
			this.Client.Lock(new string[] { filepath }, "Moo ");

			char lockStatus = this.RunCommand("svn", "status " + filepath)[5];
			Assert.That(lockStatus, Is.EqualTo('K'), "File not locked");
		}
	}
}
