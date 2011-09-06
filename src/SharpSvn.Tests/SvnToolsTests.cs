// $Id$
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
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn.Implementation;
using SharpSvn.Tests.Commands;
using System.IO;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class SvnToolsTests : TestBase
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

        [Test]
        public void IsBelowAdmin()
        {
            string dir = GetTempDir();

            Assert.That(!SvnTools.IsBelowManagedPath(dir), "Temp is not managed");

            string sd = Path.Combine(dir, "w");
            Directory.CreateDirectory(sd);

            Assert.That(!SvnTools.IsBelowManagedPath(sd), "sd not managed");

            string sdsvn = Path.Combine(sd, ".svn");
            Directory.CreateDirectory(sdsvn);

            Assert.That(SvnTools.IsBelowManagedPath(sd), "sd managed");
            Assert.That(!SvnTools.IsBelowManagedPath(sdsvn), "sdsvn not managed");

            string format = Path.Combine(sdsvn, "format");
            File.WriteAllText(format, "-1");
            Assert.That(!SvnTools.IsBelowManagedPath(format), "format not managed");

            string sdsvnsvn = Path.Combine(sd, ".svn");
            Directory.CreateDirectory(sdsvnsvn);
            Assert.That(!SvnTools.IsBelowManagedPath(format), "format not managed");

            string sdsvnd = Path.Combine(sdsvn, "d");
            Directory.CreateDirectory(sdsvnd);
            Assert.That(!SvnTools.IsBelowManagedPath(sdsvnd), "d not managed");
            Directory.CreateDirectory(Path.Combine(sdsvnd, ".svn"));
            Assert.That(SvnTools.IsBelowManagedPath(sdsvnd), "d managed");
        }

        [Test]
        public void SplitPaths()
        {
            string app, args;
            Assert.That(SvnTools.TrySplitCommandLine("wscript script", out app, out args));
            Assert.That(app, Is.EqualTo("wscript"));
            Assert.That(args, Is.EqualTo("script"));
            Assert.That(SvnTools.TryFindApplication("wscript", out app));
            Assert.That(File.Exists(app));
            Assert.That(Path.GetFileName(app).ToLowerInvariant(), Is.EqualTo("wscript.exe"));
        }
    }
}
