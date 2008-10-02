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

            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:///e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file://e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:////e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("E:\\")).AbsoluteUri, Is.EqualTo("file:///E:/"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share")).AbsoluteUri, Is.EqualTo("file://server/share"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share\")).AbsoluteUri, Is.EqualTo("file://server/share"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share\a")).AbsoluteUri, Is.EqualTo("file://server/share/a"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share\a\")).AbsoluteUri, Is.EqualTo("file://server/share/a"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("HTTP://localhost/test")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("hTTp://uSeR@localhost/test/")).AbsoluteUri, Is.EqualTo("http://uSeR@localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("httP://localhost/test//")).AbsoluteUri, Is.EqualTo("http://localhost/test"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\SERVER\share\a\")).AbsoluteUri, Is.EqualTo("file://server/share/a"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\SERVER\Share\a\")).AbsoluteUri, Is.EqualTo("file://server/Share/a"));

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

        [Test]
        public void TestUriCanonicalization()
        {
            Assert.That(new Uri("svn://127.0.0.1:1234").ToString(), Is.EqualTo("svn://127.0.0.1:1234/"));
            Assert.That(new SvnUriTarget(new Uri("svn://127.0.0.1:1234")).TargetName, Is.EqualTo("svn://127.0.0.1:1234/"));

            Assert.That(SvnTools.GetFileName(new Uri("http://svn/dir/File%23%25q/")), Is.EqualTo("File#%q"));
            Assert.That(SvnTools.GetFileName(new Uri("http://svn/dir/File%23%25q?abc")), Is.EqualTo("File#%q"));

            Assert.That(SvnTools.GetFileName(new Uri("http://svn/File%23%25q/")), Is.EqualTo("File#%q"));
            Assert.That(SvnTools.GetFileName(new Uri("http://user@svn/File%23%25q?abc")), Is.EqualTo("File#%q"));

            Assert.That(SvnTools.GetFileName(new Uri("http://svn/")), Is.EqualTo(""));
            Assert.That(SvnTools.GetFileName(new Uri("http://user@svn")), Is.EqualTo(""));

            Assert.That(SvnTools.GetFileName(new Uri("http://svn/q/")), Is.EqualTo("q"));
            Assert.That(SvnTools.GetFileName(new Uri("http://user@svn/r#")), Is.EqualTo("r"), "# is not part of the repository url");
            Assert.That(SvnTools.GetFileName(new Uri("http://svn/s")), Is.EqualTo("s"));
        }

        [Test]
        public void TestLibs()
        {
            foreach (SvnLibrary lib in SvnClient.SvnLibraries)
            {
                Assert.That(lib.Name, Is.Not.Null);

                Assert.That(lib.VersionString, Is.Not.Null,"VersionString is null for {0}", lib.Name);
                Assert.That(lib.Version, Is.Not.Null, "Version is null for {0} ({1})", lib.Name, lib.VersionString);
                Assert.That(lib.Version, Is.GreaterThan(new Version(0, 1)));
            }
        }
    }
}
