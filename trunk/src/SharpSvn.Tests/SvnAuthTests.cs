using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn.Security;

namespace SharpSvn.Tests
{
    [TestClass]
    public class SvnAuthTests : TestBase
    {
        [TestMethod]
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

        [TestMethod]
        public void ListSshCredentials()
        {
            using (SvnClient client = new SvnClient())
            {
                foreach (SvnAuthenticationCacheItem i in client.Authentication.GetCachedItems(SvnAuthenticationCacheType.WindowsSshCredentials))
                {
                    GC.KeepAlive(i);
                }
            }
        }

        [TestMethod, Ignore]
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
