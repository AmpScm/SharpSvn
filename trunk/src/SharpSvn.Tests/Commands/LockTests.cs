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

        [Test]
        public void DualLockTest()
        {
            string wc1 = GetTempDir();
            string wc2 = GetTempDir();
            Uri repos = new Uri(CollabReposUri, "trunk/");

            Client.CheckOut(repos, wc1);
            Client.CheckOut(repos, wc2);

            string index1 = Path.Combine(wc1, "index.html");
            string index2 = Path.Combine(wc2, "index.html");

            SvnInfoEventArgs ii;
            Client.GetInfo(index1, out ii);

            Assert.That(ii.Lock, Is.Null, "Not locked");

            Client.Lock(Path.Combine(wc1, "index.html"), "First");

            Client.GetInfo(index1, out ii);
            Assert.That(ii.Lock, Is.Not.Null, "Locked");

            try
            {
                Client.Lock(index2, "Second");

                Assert.That(false, "Lock just didn't do anything");
            }
            catch (SvnFileSystemLockException ex)
            {
                Assert.That(ex.Message.Contains("index.html"));
            }
        }
	}
}
