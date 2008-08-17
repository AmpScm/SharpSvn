using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class FileVersions : TestBase
    {
#if NOT
        [Test]
        public void ListIndex()
        {
            bool touched = false;
            Client.FileVersions(new Uri(CollabReposUri, "trunk/index.html"),
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    touched = true;
                });

            Assert.That(touched);
        }

        [Test]
        public void ListIndexDir()
        {
            bool touched = false;
            Client.FileVersions(new Uri(CollabReposUri, "trunk/"),
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    touched = true;
                });

            Assert.That(touched);
        }
#endif
    }
}

