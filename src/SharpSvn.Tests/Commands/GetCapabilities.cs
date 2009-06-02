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

using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class GetCapabilities : TestBase
    {
        [Test]
        public void LocalCaps()
        {
            using (SvnClient client = new SvnClient())
            {
                Collection<SvnCapability> caps;
                IEnumerable<SvnCapability> rCaps = new SvnCapability[] { SvnCapability.MergeInfo };
                Assert.That(client.GetCapabilities(GetReposUri(TestReposType.Empty), rCaps, out caps));

                Assert.That(caps.Contains(SvnCapability.MergeInfo));

                Assert.That(client.GetCapabilities(GetReposUri(TestReposType.EmptyNoMerge), rCaps, out caps));

                Assert.That(!caps.Contains(SvnCapability.MergeInfo));
            }
        }

        [Test]
        public void SharpCaps()
        {
            using (SvnClient client = new SvnClient())
            {
                Collection<SvnCapability> caps;

                SvnGetCapabilitiesArgs ca = new SvnGetCapabilitiesArgs();
                ca.RetrieveAllCapabilities = true;
                Assert.That(client.GetCapabilities(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/"), ca, out caps));

                Assert.That(caps.Contains(SvnCapability.MergeInfo));
            }
        }
    }
}
