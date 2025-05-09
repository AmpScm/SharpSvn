﻿//
// Copyright 2008-2025 The SharpSvn Project
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
using System.IO;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn.Implementation;
using SharpSvn.Tests.Commands;

namespace SharpSvn.Tests
{
    [TestClass]
    public class SvnToolsTests : TestBase
    {
        [TestMethod]
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

        [TestMethod]
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

        [TestMethod]
        public void IsBelowAdmin()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            string dir = sbox.GetTempDir();

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

        [TestMethod]
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

        [TestMethod]
        public void Normalize800000()
        {
            for (int i = 0; i < 800000; i++)
            {
                GC.KeepAlive(SvnTools.GetNormalizedFullPath("C:\\Users\\Bert\\AppData\\Local\\Microsoft\\VisualStudio\\10.0\\Extensions\\Microsoft\\Productivity Power Tools\\10.0.20626.18\\package\\services\\digital-signature\\xml-signature\\yp5utn4wx4uezottryfbg9mac.psdsxs"));
            }
        }

        [TestMethod]
        public void TestGetUriFromWc()
        {
            Assert.That(SvnTools.GetUriFromWorkingCopy("C:\\"), Is.Null);
            Assert.That(SvnTools.GetUriFromWorkingCopy("B:\\"), Is.Null);
        }
    }
}
