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
    public class CapabilitiesTests : TestBase
    {
        [TestMethod]
        public void Capabilities_Local()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri emptyUri = sbox.CreateRepository(SandBoxRepository.Empty);
            Uri emptyNoMergeUri = sbox.CreateRepository(SandBoxRepository.EmptyNoMerge);


            using (SvnClient client = new SvnClient())
            {
                Collection<SvnCapability> caps;
                SvnGetCapabilitiesArgs aa = new SvnGetCapabilitiesArgs();
                aa.RetrieveAllCapabilities = true;

                IEnumerable<SvnCapability> rCaps = new SvnCapability[] { SvnCapability.MergeInfo };
                Assert.That(client.GetCapabilities(emptyUri, rCaps, out caps));

                Assert.That(caps.Contains(SvnCapability.MergeInfo));

                Assert.That(client.GetCapabilities(emptyNoMergeUri, rCaps, out caps));

                Assert.That(!caps.Contains(SvnCapability.MergeInfo));
                Assert.That(caps.Count, Is.EqualTo(0));
                
                Assert.That(client.GetCapabilities(emptyNoMergeUri, aa, out caps));
                Assert.That(caps.Count, Is.GreaterThanOrEqualTo(5));

                Assert.That(client.GetCapabilities(emptyUri, aa, out caps));
                Assert.That(caps.Count, Is.GreaterThanOrEqualTo(6));
            }
        }

        [TestMethod]
        public void Capabilities_SharpCaps()
        {
            using (SvnClient client = NewSvnClient(false, false))
            {
                Collection<SvnCapability> caps;

                SvnGetCapabilitiesArgs ca = new SvnGetCapabilitiesArgs();
                ca.RetrieveAllCapabilities = true;
                Assert.That(client.GetCapabilities(new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn"), ca, out caps));

                Assert.That(caps.Contains(SvnCapability.MergeInfo));
                Assert.That(caps.Count, Is.GreaterThanOrEqualTo(5));
            }
        }
    }
}
