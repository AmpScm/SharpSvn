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

			SvnLockArgs la = new SvnLockArgs();
			la.Comment = "Moo ";
			la.Notify += new EventHandler<SvnNotifyEventArgs>(OnLockNotify);
			this.Client.Lock(new string[] { filepath }, la);

			char lockStatus = this.RunCommand("svn", "status " + filepath)[5];
			Assert.That(lockStatus, Is.EqualTo('K'), "File not locked");
		}

		void OnLockNotify(object sender, SvnNotifyEventArgs e)
		{
			GC.KeepAlive(e);
			//throw new NotImplementedException();
		}
	}
}
