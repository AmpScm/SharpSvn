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
using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using System.Collections.ObjectModel;


namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class MergeInfoTests : TestBase
    {
        [TestMethod]
        public void Mergeinfo_MergesApplied()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            // Extended version of GetSuggestedMergeSourcesTests:VerifyCollabNetRepos

            SvnAppliedMergeInfo applied;

            SvnTarget me = new SvnUriTarget(new Uri(reposUri, "trunk/"), 16);

            Assert.That(Client.GetAppliedMergeInfo(me, out applied));

            Assert.That(applied, Is.Not.Null);
            Assert.That(applied.AppliedMerges.Count, Is.EqualTo(3));

            foreach (SvnMergeItem mi in applied.AppliedMerges)
            {
                if (mi.Uri == new Uri(reposUri, "trunk"))
                {
                    Assert.That(mi.MergeRanges.Count, Is.EqualTo(1));
                    Assert.That(mi.MergeRanges[0].Start, Is.EqualTo(1));
                    Assert.That(mi.MergeRanges[0].End, Is.EqualTo(2));
                    Assert.That(mi.MergeRanges[0].Inheritable, Is.True);
                }
                else if (mi.Uri == new Uri(reposUri, "branches/a"))
                {
                    Assert.That(mi.MergeRanges.Count, Is.EqualTo(1));
                    Assert.That(mi.MergeRanges[0].Start, Is.EqualTo(2));
                    Assert.That(mi.MergeRanges[0].End, Is.EqualTo(11));
                    Assert.That(mi.MergeRanges[0].Inheritable, Is.True);
                }
                else if (mi.Uri == new Uri(reposUri, "branches/b"))
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

        [TestMethod]
        public void MergeInfo_MergesAvailable()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            SvnMergeSourcesCollection msc;

            SvnTarget me = new SvnUriTarget(new Uri(reposUri, "trunk/"), 16);
            Assert.That(Client.GetSuggestedMergeSources(me, out msc));

            Assert.That(msc, Is.Not.Null);
            Assert.That(msc.Count, Is.EqualTo(3));
            foreach (SvnMergeSource ms in msc)
            {
                Collection<SvnMergesEligibleEventArgs> info;

                Assert.That(ms.Uri, Is.Not.Null);

                Assert.That(Client.GetMergesEligible(me, ms.Uri, out info));
                Assert.That(info, Is.Not.Null);

                if (ms.Uri == new Uri(reposUri, "trunk"))
                {

                    Assert.That(info.Count, Is.EqualTo(1));
                    Assert.That(info[0].Revision, Is.EqualTo(17L));
                }
                else if (ms.Uri == new Uri(reposUri, "branches/a"))
                {
                    Assert.That(info.Count, Is.EqualTo(0));
                }
                else if (ms.Uri == new Uri(reposUri, "branches/b"))
                {
                    Assert.That(info.Count, Is.EqualTo(0));
                }
                else
                    Assert.That(false, "Strange branch found");
            }
        }
    }
}
