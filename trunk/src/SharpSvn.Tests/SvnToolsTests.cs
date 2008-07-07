using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn.Implementation;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class SvnToolsTests
    {
        [Test]
        public void TestNormalizeUri()
        {
            Assert.That(SvnTools.GetNormalizedUri(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/")).AbsoluteUri,
                Is.EqualTo("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23"));


            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test/")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test//")).AbsoluteUri, Is.EqualTo("http://localhost/test"));


            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b/")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));
            
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a%20b")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a%20b/")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));

        }

        [Test]
        public void TestPathToUri()
        {
            Uri root = new Uri("http://server/q/");
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("\\a b\\test")).AbsoluteUri, Is.EqualTo("http://server/a%20b/test"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("\\a b\\test\\")).AbsoluteUri, Is.EqualTo("http://server/a%20b/test/"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("\\c#\\test\\")).AbsoluteUri, Is.EqualTo("http://server/c%23/test/"));

            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("a b\\test")).AbsoluteUri, Is.EqualTo("http://server/q/a%20b/test"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("a b\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/a%20b/test/"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("c#\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/c%23/test/"));

            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("r\\a b\\test")).AbsoluteUri, Is.EqualTo("http://server/q/r/a%20b/test"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("r\\a b\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/r/a%20b/test/"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("r\\c#\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/r/c%23/test/"));
        }
    }
}
