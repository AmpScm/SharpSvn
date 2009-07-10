using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;

namespace SharpSvn.Tests.RemoteTests
{
    [TestFixture]
    public class RemoteLogTests : TestBase
    {
        [Test]
        public void OpenRepos()
        {
            using (SvnRemoteSession r = new SvnRemoteSession(CollabReposUri))
            {
                r.Reparent(CollabReposUri);
                long rev;
                r.GetLatestRevision(out rev);
            }
        }

    }
}
