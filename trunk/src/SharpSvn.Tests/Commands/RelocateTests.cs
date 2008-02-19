// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using NUnit.Framework;
using System.Diagnostics;
using System.IO;

using SharpSvn;
using System.Collections.ObjectModel;
using NUnit.Framework.SyntaxHelpers;
using System.Threading;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// A test for Client.Relocate
	/// </summary>
	[TestFixture]
	public class RelocateTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractWorkingCopy();
			this.ExtractRepos();
		}

		[Test]
		[Ignore("Fails on svnserve")]
		public void Test()
		{
			// start a svnserve process on this repos
			Process svnserve = this.StartSvnServe(this.ReposPath.Replace('\\', '/'));
			Thread.Sleep(3000);
			try
			{
				Uri localUri = new Uri(String.Format("svn://localhost:{0}/", PortNumber));
				Assert.That(Client.Relocate(this.WcPath, ReposUrl, localUri));

				Collection<SvnInfoEventArgs> list;
				SvnInfoArgs a = new SvnInfoArgs();

				Assert.That(Client.GetInfo(WcPath, a, out list));

				Assert.That(list.Count, Is.GreaterThan(0));
				Assert.That(list[0].RepositoryRoot.ToString().StartsWith(localUri.ToString()));
			}
			finally
			{
				svnserve.CloseMainWindow();
				System.Threading.Thread.Sleep(500);
				if (!svnserve.HasExited)
				{
					svnserve.Kill();
					svnserve.WaitForExit();
				}
			}
		}


	}
}
