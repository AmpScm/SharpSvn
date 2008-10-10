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
