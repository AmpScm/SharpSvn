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
using System.Collections.ObjectModel;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class GetSuggestedMergeSourcesTests : TestBase
    {
        [TestMethod]
        public void MergeSources_VerifyCollabNetRepos()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string dir = GetTempDir();
            SvnMergeSourcesCollection msc;

            SvnTarget me = new SvnUriTarget(new Uri(CollabReposUri, "trunk/"), 16);
            Assert.That(Client.GetSuggestedMergeSources(me, out msc));

            Assert.That(msc, Is.Not.Null);
            Assert.That(msc.Count, Is.EqualTo(3));
            foreach (SvnMergeSource ms in msc)
            {
                Collection<SvnMergesEligibleEventArgs> info;

                Assert.That(Client.GetMergesEligible(me, ms.Uri, out info));
                Assert.That(info, Is.Not.Null);

                if (ms.Uri == new Uri(CollabReposUri, "trunk"))
                {
                    Assert.That(info.Count, Is.EqualTo(1));
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
