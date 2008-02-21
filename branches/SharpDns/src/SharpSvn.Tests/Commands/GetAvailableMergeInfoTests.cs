// $Id: ExportTests.cs 303 2008-02-20 13:19:48Z bhuijben $
// Copyright (c) SharpSvn Project 2008
using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class GetAvailableMergeInfoTests : TestBase
	{
		[Test]
		public void VerifyCollabNetRepos()
		{
			// Extended version of GetSuggestedMergeSourcesTests:VerifyCollabNetRepos
			string dir = GetTempDir();
			SvnMergeSourcesCollection msc;

			SvnTarget me = new SvnUriTarget(new Uri(CollabReposUri, "trunk/"), 16);
			Assert.That(Client.GetSuggestedMergeSources(me, out msc));

			Assert.That(msc, Is.Not.Null);
			Assert.That(msc.Count, Is.EqualTo(3));
			foreach (SvnMergeSource ms in msc)
			{
				SvnAvailableMergeInfo info;

				Assert.That(Client.GetAvailableMergeInfo(me, ms.Uri, out info));
				Assert.That(info, Is.Not.Null);

				if (ms.Uri == new Uri(CollabReposUri, "trunk"))
				{
					Assert.That(info.MergeRanges.Count, Is.EqualTo(1));
					Assert.That(info.MergeRanges[0].Start, Is.EqualTo(16L));
					Assert.That(info.MergeRanges[0].End, Is.EqualTo(17L));
					Assert.That(info.MergeRanges[0].Inheritable, Is.True);
				}
				else if (ms.Uri == new Uri(CollabReposUri, "branches/a"))
				{
					Assert.That(info.MergeRanges.Count, Is.EqualTo(1));
					Assert.That(info.MergeRanges[0].Start, Is.EqualTo(11L));
					Assert.That(info.MergeRanges[0].End, Is.EqualTo(17L));
					Assert.That(info.MergeRanges[0].Inheritable, Is.True);
				}
				else if (ms.Uri == new Uri(CollabReposUri, "branches/b"))
				{
					Assert.That(info.MergeRanges.Count, Is.EqualTo(1));
					Assert.That(info.MergeRanges[0].Start, Is.EqualTo(13L));
					Assert.That(info.MergeRanges[0].End, Is.EqualTo(17L));
					Assert.That(info.MergeRanges[0].Inheritable, Is.True);
				}
				else
					Assert.That(false, "Strange branch found");
			}
		}
	}
}
