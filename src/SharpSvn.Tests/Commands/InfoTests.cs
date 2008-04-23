﻿// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class InfoTests : TestBase
	{
		[Test]
		public void InfoTest()
		{
			string dir = GetTempDir();
			SvnUpdateResult r;

			Client.CheckOut(CollabReposUri, dir, out r);

			int i = 0;
			Assert.That(Client.Info(dir,
				delegate(object sender, SvnInfoEventArgs e)
				{
					i++;
					Assert.That(e.Revision, Is.EqualTo(r.Revision));
					Assert.That(e.LastChangeRevision, Is.EqualTo(r.Revision));
				}), Is.True);


			Assert.That(i, Is.EqualTo(1));

			i = 0;
			SvnInfoArgs aa = new SvnInfoArgs();
			aa.Depth = SvnDepth.Children;

			Assert.That(Client.Info(dir, aa,
				delegate(object sender, SvnInfoEventArgs e)
				{
					i++;
					Assert.That(e.Revision, Is.EqualTo(r.Revision));
					Assert.That(e.LastChangeRevision, Is.LessThanOrEqualTo(r.Revision));
				}), Is.True);

			Assert.That(i, Is.EqualTo(4)); // trunk branches tags
		}

		[Test]
		public void WcCasing()
		{
			string dir = GetTempDir();
			SvnUpdateResult r;

			Assert.That(Client.CheckOut(CollabReposUri, dir, out r));
			Collection<SvnInfoEventArgs> items;
			SvnInfoArgs aa = new SvnInfoArgs();
			aa.Depth = SvnDepth.Children;

			Assert.That(Client.GetInfo(Path.Combine(dir, "trunk"), aa, out items));

			foreach (SvnInfoEventArgs ia in items)
			{
				bool reached = false;
				Assert.That(Client.Info(ia.FullPath,
					delegate(object sender, SvnInfoEventArgs e)
					{
						reached = true;
					}));

				Assert.That(reached);

				reached = false;
				SvnInfoArgs a = new SvnInfoArgs();
				a.ThrowOnError = false;
				Client.Info(ia.FullPath.ToUpperInvariant(), a,
					delegate(object sender, SvnInfoEventArgs e)
					{
						reached = true;
					});

				if (ia.NodeKind != SvnNodeKind.Directory)
					Assert.That(reached, Is.False);
			}
		}

        [Test]
        public void WcDirMissing()
        {
            string dir = GetTempDir();
            SvnUpdateResult r;

            Assert.That(Client.CheckOut(CollabReposUri, dir, out r));

            Directory.Move(Path.Combine(dir, "trunk"), Path.Combine(dir, "banaan"));

            SvnInfoEventArgs iaParent;
            SvnInfoEventArgs iaTrunk;

            Client.GetInfo(dir, out iaParent);
            Client.GetInfo(Path.Combine(dir, "trunk"), out iaTrunk);

            Assert.That(iaParent.FullPath, Is.EqualTo(dir));
            Assert.That(iaTrunk.FullPath, Is.Not.EqualTo(dir));
            Assert.That(iaParent.Uri, Is.EqualTo(iaTrunk.Uri));

            SvnWorkingCopyVersion ver;
            Assert.That(Client.GetWorkingCopyVersion(dir, out ver));
            Assert.That(ver, Is.Not.Null);

            Assert.That(ver.Modified, Is.True);
            Assert.That(ver.Switched, Is.False);
            Assert.That(ver.Start, Is.EqualTo(17));
            Assert.That(ver.End, Is.EqualTo(17));
            Assert.That(ver.IncompleteWorkingCopy, Is.False);
        }
	}
}
