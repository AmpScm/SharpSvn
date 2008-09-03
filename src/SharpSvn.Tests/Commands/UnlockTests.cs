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
	/// Summary description for UnlockTest.
	/// </summary>
	[TestFixture]
	public class UnlockTests : TestBase
	{
		[Test]
		public void BasicUnlockTest()
		{
			string filepath = Path.Combine(this.WcPath, "Form.cs");

			this.RunCommand("svn", "lock " + filepath);

			char lockStatus;
			lockStatus = this.RunCommand("svn", "status " + filepath)[5];
			Assert.That(lockStatus, Is.EqualTo('K'), "file not locked");

			this.Client.Unlock(filepath);

			Assert.That(this.RunCommand("svn", "status " + filepath).Length == 0, "file not unlocked");
		}
	}
}
