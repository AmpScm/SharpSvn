// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
