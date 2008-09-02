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

                Assert.That(!caps.Contains(SvnCapability.MergeInfo));
            }
        }
    }
}
