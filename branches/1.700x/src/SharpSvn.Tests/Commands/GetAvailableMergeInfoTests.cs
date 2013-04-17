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
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class GetEligableMergeInfoTests : TestBase
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
                Collection<SvnMergesEligibleEventArgs> info;

                Assert.That(ms.Uri, Is.Not.Null);

                Assert.That(Client.GetMergesEligible(me, ms.Uri, out info));
				Assert.That(info, Is.Not.Null);

				if (ms.Uri == new Uri(CollabReposUri, "trunk"))
				{

					Assert.That(info.Count, Is.EqualTo(1));
					Assert.That(info[0].Revision, Is.EqualTo(17L));
				}
				else if (ms.Uri == new Uri(CollabReposUri, "branches/a"))
				{
					Assert.That(info.Count, Is.EqualTo(0));
				}
				else if (ms.Uri == new Uri(CollabReposUri, "branches/b"))
				{
					Assert.That(info.Count, Is.EqualTo(0));
				}
				else
					Assert.That(false, "Strange branch found");
			}
		}
	}
}
