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
using SharpSvn.Tests.Commands;
using SharpSvn.Diff;

namespace SharpSvn.Tests
{
    [TestClass]
    public class FileDiffTests : TestBase
    {
        [TestMethod]
        public void FileDiff_UniDiff()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.MergeScenario);

            string indexTheir = Path.Combine(sbox.Wc, "about\\index.html");
            string indexMine = Path.Combine(sbox.Wc, "jobs\\index.html");
            string indexLatest = Path.Combine(sbox.Wc, "products\\index.html");
            string indexAncestor = Path.Combine(sbox.Wc, "support\\index.html");

            SvnFileDiff diff;

            Assert.That(SvnFileDiff.TryCreate(indexTheir, indexMine, new SvnFileDiffArgs(), out diff));
            Assert.That(diff, Is.Not.Null);
            Assert.That(diff.HasDifferences, Is.True);
            Assert.That(diff.HasConflicts, Is.False);

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteDifferences(ms, new SvnDiffWriteDifferencesArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine().Contains(indexTheir.Replace('\\', '/')), "The first line contains the path to indexTheir");
                    Assert.That(tr.ReadLine().Contains(indexMine.Replace('\\', '/')), "The second line contains the path to indexMine");
                    Assert.That(tr.ReadLine(), Is.EqualTo("@@ -9,11 +9,12 @@"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(" <body>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(" \t<p>Content for merge tracking early adopter program</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(" \t"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\t<p>This is the index page in the about folder</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\t<p>Here is some content about our company."));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\tWe are really great."));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\tWe have great products."));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\tWe have a great roadmap.</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("+\t<p>This is the index page in the jobs folder</p>"));

                    // Ok, the rest will be ok
                }
            }

            Assert.That(SvnFileDiff.TryCreate(indexTheir, indexMine, indexLatest, new SvnFileDiffArgs(), out diff));
            Assert.That(diff, Is.Not.Null);
            Assert.That(diff.HasDifferences, Is.True);
            Assert.That(diff.HasConflicts, Is.True);

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteDifferences(ms, new SvnDiffWriteDifferencesArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine().Contains(indexTheir.Replace('\\', '/')), "The first line contains the path to indexTheir");
                    Assert.That(tr.ReadLine().Contains(indexMine.Replace('\\', '/')), "The second line contains the path to indexMine");
                    Assert.That(tr.ReadLine(), Is.Null);

                    // EOF
                }
            }

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteMerged(ms, new SvnDiffWriteMergedArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine(), Is.EqualTo("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<html>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"/>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<title></title>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("</head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<body>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>Content for merge tracking early adopter program</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t"));
                    string line = tr.ReadLine();
                    Assert.That(line.StartsWith("<<<<<<< "), "Starts with <<<<<<<");
                    Assert.That(line.Substring(8), Is.EqualTo(indexMine.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the jobs folder</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>List of jobs we need:</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>Obviously we need a web designer.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>We need a product manager.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t</ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("======="));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the products folder.</p>"));
                    line = tr.ReadLine();
                    Assert.That(line.StartsWith(">>>>>>> "), "Starts with >>>>>>>");
                    Assert.That(line.Substring(8), Is.EqualTo(indexLatest.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t"));

                    // Ok, the rest will be ok
                }
            }

            Assert.That(SvnFileDiff.TryCreate(indexTheir, indexMine, indexLatest, indexAncestor, new SvnFileDiffArgs(), out diff));
            Assert.That(diff, Is.Not.Null);
            Assert.That(diff.HasDifferences, Is.True);
            Assert.That(diff.HasConflicts, Is.True);

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteDifferences(ms, new SvnDiffWriteDifferencesArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine().Contains(indexTheir.Replace('\\', '/')), "The first line contains the path to indexTheir");
                    Assert.That(tr.ReadLine().Contains(indexMine.Replace('\\', '/')), "The second line contains the path to indexMine");
                    Assert.That(tr.ReadLine(), Is.EqualTo("@@ -1,25 +1,19 @@"));

                    // The rest of this output does not make any sense.. just ignore it
                }
            }

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteMerged(ms, new SvnDiffWriteMergedArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine(), Is.EqualTo("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<html>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"/>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<title></title>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("</head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<body>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>Content for merge tracking early adopter program</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t"));
                    string line = tr.ReadLine();
                    Assert.That(line.StartsWith("<<<<<<< "), "Starts with <<<<<<<");
                    Assert.That(line.Substring(8), Is.EqualTo(indexMine.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the jobs folder</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>List of jobs we need:</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>Obviously we need a web designer.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>We need a product manager.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t</ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("======="));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the products folder.</p>"));
                    line = tr.ReadLine();
                    Assert.That(line.StartsWith(">>>>>>> "), "Starts with >>>>>>>");
                    Assert.That(line.Substring(8), Is.EqualTo(indexLatest.Replace('\\', '/')));
                    line = tr.ReadLine();
                    Assert.That(line.StartsWith("<<<<<<< "), "Starts with <<<<<<<");
                    Assert.That(line.Substring(8), Is.EqualTo(indexMine.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<ul>"));

                    // Ok, the rest will be ok
                }
            }
        }
    }
}
