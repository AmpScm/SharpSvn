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
    /// Tests for the Client::Export method
    /// </summary>
    [TestClass]
    public class ExportTests : TestBase
    {
        /// <summary>
        /// Test export operation from a repository
        /// </summary>
        [TestMethod]
        public void Export_Repos()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.Default);

            string wc = sbox.GetTempDir();
            Directory.Delete(wc);
            this.Client.Export(reposUri, wc);

            Assert.That(File.Exists(Path.Combine(wc, "trunk/README.txt")),
                "Exported file not there");
            Assert.That(!Directory.Exists(Path.Combine(wc, SvnClient.AdministrativeDirectoryName)),
                ".svn directory found");
        }
        /// <summary>
        /// Test export operation from a working copy
        /// </summary>
        [TestMethod]
        public void Export_ExportWc()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;
            string wc = Path.Combine(sbox.GetTempDir(), "wc");

            string formresx = Path.Combine(WcPath, "Form.resx");
            File.WriteAllText(formresx, "Replaced Data!");

            this.Client.Export(WcPath, wc);

            Assert.That(File.Exists(Path.Combine(wc, "Form.cs")),
                "Exported file not there");
            Assert.That(!Directory.Exists(Path.Combine(wc, SvnClient.AdministrativeDirectoryName)),
                ".svn directory found");

            Assert.That(File.ReadAllText(Path.Combine(wc, "Form.resx")), Is.EqualTo("Replaced Data!"));
        }

        [TestMethod]
        public void Export_ExportNonRecursive()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            string WcPath = sbox.Wc;

            string wc = Path.Combine(sbox.GetTempDir(), "wc");
            SvnExportArgs a = new SvnExportArgs();
            a.Depth = SvnDepth.Empty;
            SvnUpdateResult r;
            this.Client.Export(WcPath, wc, a, out r);
            Assert.That(Directory.GetDirectories(wc).Length, Is.EqualTo(0));
        }

        [TestMethod]
        public void Export_Forced()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            string WcPath = sbox.Wc;
            Uri WcUri = sbox.Uri;

            string exportDir = Path.Combine(sbox.GetTempDir("ExportTests"), "exportTo");

            using (SvnClient client = NewSvnClient(true, false))
            {
                string file = Path.Combine(WcPath, "ExportFile");
                TouchFile(file);
                client.Add(file);

                client.Commit(WcPath);

                Assert.That(Directory.Exists(exportDir), Is.False);

                client.Export(WcUri, exportDir);
                Assert.That(Directory.Exists(exportDir), Is.True);
                Assert.That(File.Exists(Path.Combine(exportDir, "ExportFile")));
                Assert.That(!Directory.Exists(Path.Combine(exportDir, ".svn")));

                ForcedDeleteDirectory(exportDir);

                Assert.That(Directory.Exists(exportDir), Is.False);

                client.Export(new SvnPathTarget(WcPath), exportDir);
                Assert.That(Directory.Exists(exportDir), Is.True);
                Assert.That(File.Exists(Path.Combine(exportDir, "ExportFile")));

                ForcedDeleteDirectory(exportDir);
            }
        }
    }
}
