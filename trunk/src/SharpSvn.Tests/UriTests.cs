using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn.Implementation;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class UriTests
    {
        [Test]
        public void TestNormalizeUri()
        {
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test/")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test//")).AbsoluteUri, Is.EqualTo("http://localhost/test"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///localhost/"), "a%20b")).AbsoluteUri, Is.EqualTo("file:///localhost/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///localhost/"), "a%20b")).AbsoluteUri, Is.EqualTo("file:///localhost/a%20b"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b/")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));
        }
    }
}
