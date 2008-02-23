// $Id$
// Copyright (c) SharpSvn Project 2008
using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class GetAppliedMergeInfoTests : TestBase
	{
		[Test]
		public void VerifyCollabNetRepos()
		{
			// Extended version of GetSuggestedMergeSourcesTests:VerifyCollabNetRepos
			string dir = GetTempDir();
			SvnAppliedMergeInfo applied;

			SvnTarget me = new SvnUriTarget(new Uri(CollabReposUri, "trunk/"), 16);

			Assert.That(Client.GetAppliedMergeInfo(me, out applied));

			Assert.That(applied, Is.Not.Null);
			Assert.That(applied.AppliedMerges.Count, Is.EqualTo(3));

			foreach (SvnMergeItem mi in applied.AppliedMerges)
			{
				if (mi.Uri == new Uri(CollabReposUri, "trunk"))
				{
					Assert.That(mi.MergeRanges.Count, Is.EqualTo(1));
					Assert.That(mi.MergeRanges[0].Start, Is.EqualTo(1));
					Assert.That(mi.MergeRanges[0].End, Is.EqualTo(2));
					Assert.That(mi.MergeRanges[0].Inheritable, Is.True);
				}
				else if (mi.Uri == new Uri(CollabReposUri, "branches/a"))
				{
					Assert.That(mi.MergeRanges.Count, Is.EqualTo(1));
					Assert.That(mi.MergeRanges[0].Start, Is.EqualTo(2));
					Assert.That(mi.MergeRanges[0].End, Is.EqualTo(11));
					Assert.That(mi.MergeRanges[0].Inheritable, Is.True);
				}
				else if (mi.Uri == new Uri(CollabReposUri, "branches/b"))
				{
					Assert.That(mi.MergeRanges.Count, Is.EqualTo(1));
					Assert.That(mi.MergeRanges[0].Start, Is.EqualTo(9));
					Assert.That(mi.MergeRanges[0].End, Is.EqualTo(13));
					Assert.That(mi.MergeRanges[0].Inheritable, Is.True);
				}
				else
					Assert.That(false, "Strange applied merge");
			}
		}
	}
}
