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

// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Summary description for PropGetTest.
    /// </summary>
    [TestClass]
    public class GetPropertyTests : TestBase
    {
        [TestMethod]
        public void TestPropGetOnFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;

            string path = Path.Combine(WcPath, "Form.cs");
            TouchFile(path);
            Client.Add(path);
            Client.SetProperty(path, "foo", "bar");

            string value;
            Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out value));

            Assert.That(value, Is.EqualTo("bar"));

            SvnPropertyValue pval;

            Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out pval));

            Assert.That(pval.StringValue, Is.EqualTo("bar"));
            Assert.That(pval.Key, Is.EqualTo("foo"));
            Assert.That(pval.Target.TargetName, Is.EqualTo(path));
        }

        [TestMethod]
        public void GetPropertyValues()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            Uri trunk = new Uri(CollabReposUri, "trunk");
            string dir = sbox.GetTempDir();
            Client.CheckOut(trunk, dir);

            SvnGetPropertyArgs pa = new SvnGetPropertyArgs();
            pa.Depth = SvnDepth.Infinity;
            SvnTargetPropertyCollection pc;
            Client.GetProperty(trunk, SvnPropertyNames.SvnEolStyle, pa, out pc);

            foreach (SvnPropertyValue pv in pc)
            {
                SvnUriTarget ut = pv.Target as SvnUriTarget;
                Assert.That(ut, Is.Not.Null);
                Uri relative = trunk.MakeRelativeUri(ut.Uri);
                Assert.That(!relative.ToString().StartsWith("/"));
                Assert.That(!relative.ToString().StartsWith("../"));
            }

            Client.GetProperty(dir, SvnPropertyNames.SvnEolStyle, pa, out pc);

            dir += "\\";
            foreach (SvnPropertyValue pv in pc)
            {
                SvnPathTarget pt = pv.Target as SvnPathTarget;
                Assert.That(pt, Is.Not.Null);
                Assert.That(pt.TargetPath.StartsWith(dir));
            }

            Assert.That(pc[dir + "index.html"], Is.Not.Null, "Can get wcroot\\index.html?");
        }

        [TestMethod]
        public void TestNonExistentPropertyExistingFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            string wc = sbox.Wc;
            Client.CheckOut(new Uri(CollabReposUri, "trunk"), wc);

            SvnGetPropertyArgs pa = new SvnGetPropertyArgs();
            pa.ThrowOnError = false;

            string value;
            Assert.That(Client.GetProperty(Path.Combine(wc, "index.html"),
                "new-prop", out value), Is.True,
                "GetProperty succeeds on non existent property");

            Assert.That(value, Is.Null, "No value available");
        }

        [TestMethod, ExpectedException(typeof(SvnUnversionedNodeException))]
        public void TestNonExistentPropertyNonExistingFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string wc = sbox.Wc;
            Client.CheckOut(new Uri(CollabReposUri, "trunk"), wc);

            string value;
            Client.GetProperty(Path.Combine(wc, "no-index.html"), "new-prop", out value);
        }

        [TestMethod, ExpectedException(typeof(SvnInvalidNodeKindException))]
        public void TestNoWcProperty()
        {
            string value;
            Client.GetProperty("c:/{632382A5-F992-4ab8-8D37-47977B190819}/no-file.txt", "no-prop", out value);
        }

        [TestMethod]
        public void TestGetOnCwd()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string wc = sbox.Wc;
            Client.CheckOut(new Uri(CollabReposUri, "trunk"), wc);

            string dir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory(wc);
            try
            {
                string v;
                Assert.That(Client.TryGetProperty(".", SvnPropertyNames.SvnMergeInfo, out v));
                Assert.That(v, Is.Not.Null);

                Assert.That(Client.TryGetProperty(SvnTarget.FromString("."), SvnPropertyNames.SvnMergeInfo, out v));
                Assert.That(v, Is.Not.Null);
            }
            finally
            {
                Directory.SetCurrentDirectory(wc);
            }
        }
    }
}
