using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using SharpSvn.Security;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class SvnAuthTests : TestBase
    {
        [Test]
        public void AuthDeleteTest()
        {
            using (SvnClient client = new SvnClient())
            {
                //client.LoadConfiguration(GetTempDir());
                foreach (SvnAuthenticationCacheItem i in client.Authentication.GetCachedItems(SvnAuthenticationCacheType.UserNamePassword))
                {
                    GC.KeepAlive(i);
                }
            }
        }


        public void ListSsh()
        {
            using (SvnClient client = new SvnClient())
            {
                bool foundOne = false;
                client.List(new Uri("svn+ssh://vip.alh.net.qqn.nl/home/bert/repos/"), delegate(object sender, SvnListEventArgs e)
                {
                    foundOne = true;
                });

                Assert.That(foundOne);
            }
        }

    }
}
