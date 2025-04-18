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
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using System.IO;

namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class FileVersionsTests : TestBase
    {
        [TestMethod]
        public void FileVersions_ListIndex()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            Uri reposUri = sbox.CreateRepository(SandBoxRepository.DefaultBranched);

            bool touched = false;
            SvnFileVersionsArgs fa = new SvnFileVersionsArgs();
            fa.RetrieveProperties = true;
            Client.FileVersions(new Uri(reposUri, "trunk/README.txt"),
                fa,
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    touched = true;
                    Assert.That(e.RevisionProperties, Is.Not.Null, "Revision properties available");
                    Assert.That(e.Properties, Is.Not.Null, "Properties available");

                    Assert.That(e.RevisionProperties.Contains(SvnPropertyNames.SvnAuthor));
//                    Assert.That(e.Properties.Contains(SvnPropertyNames.SvnEolStyle));

                    Assert.That(e.Revision, Is.GreaterThan(0));
                    Assert.That(e.Author, Is.Not.Null);
                    Assert.That(e.LogMessage, Is.Not.Null);
                    Assert.That(e.Time, Is.LessThan(DateTime.UtcNow).And.GreaterThan(DateTime.UtcNow - new TimeSpan(0,0,2,0)));

                    using (StreamReader sr = new StreamReader(e.GetContentStream()))
                    {
                        string content = sr.ReadToEnd();

                        Assert.That(content, Is.Not.Null);
                        Assert.That(content.Length, Is.GreaterThan(10));
                    }
                });

            Assert.That(touched);
        }

        [TestMethod, ExpectedException(typeof(SvnFileSystemNodeTypeException))]
        public void FileVersions_ListIndexDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            Uri reposUri = sbox.CreateRepository(SandBoxRepository.Default);

            Client.FileVersions(new Uri(reposUri, "trunk/"),
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                });

            throw new InvalidOperationException(); // Should have failed
        }

        [TestMethod]
        public void FileVersions_WalkKeywords()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);

            string wc = sbox.Wc;
            string file = Path.Combine(wc, "myFile.txt");
            string nl = Environment.NewLine;

            File.WriteAllText(file, "Line1 $Id: FileVersions.cs 2139 2012-05-19 10:21:53Z rhuijben $" + nl + "$HeadURL$" + nl + nl);

            Client.Add(file);
            Client.SetProperty(file, SvnPropertyNames.SvnKeywords, "Id\nHeadURL");
            Client.Commit(file);
            File.AppendAllText(file, "Line" + nl);
            Client.Commit(file);
            Client.SetProperty(file, SvnPropertyNames.SvnEolStyle, "native");
            Client.Commit(file);
            File.AppendAllText(file, "Line" + nl + "Line");
            Client.Commit(file);
            Client.SetProperty(file, SvnPropertyNames.SvnEolStyle, "CR");
            Client.Commit(file);

            string f2 = file + "2";
            Client.Copy(file, f2);
            SvnCommitArgs xa = new SvnCommitArgs();
            xa.LogProperties.Add("extra", "value");
            Client.Commit(wc, xa);
            Client.Update(wc);

            SvnFileVersionsArgs va;

            using (SvnClient c2 = new SvnClient())
            {
                Uri fileUri = c2.GetUriFromWorkingCopy(file);
                Uri f2Uri = c2.GetUriFromWorkingCopy(f2);

                for (int L = 0; L < 2; L++)
                {
                    va = new SvnFileVersionsArgs();
                    va.RetrieveProperties = true;
                    switch (L)
                    {
                        case 0:
                            va.Start = SvnRevision.Zero;
                            va.End = SvnRevision.Head;
                            break;
                        default:
                            break;
                    }

                    int i = 0;
                    Client.FileVersions(f2, va,
                        delegate(object sender, SvnFileVersionEventArgs e)
                        {
                            Assert.That(e.Revision, Is.EqualTo(i + 1L));
                            Assert.That(e.RepositoryRoot, Is.Not.Null);
                            Assert.That(e.Uri, Is.EqualTo(i == 5 ? f2Uri : fileUri));
                            Assert.That(e.Author, Is.EqualTo(Environment.UserName));
                            Assert.That(e.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 10, 0, 0)));
                            Assert.That(e.RevisionProperties.Count, Is.GreaterThanOrEqualTo(3));

                            if (i == 5)
                            {
                                Assert.That(e.RevisionProperties.Contains("extra"), "Contains extra property");
                                //Assert.That(e.Properties.Contains(SvnPropertyNames.SvnMergeInfo), Is.True, "Contains merge info in revision 5");
                            }
                            else
                                Assert.That(e.Properties.Contains(SvnPropertyNames.SvnMergeInfo), Is.False, "No mergeinfo");

                            MemoryStream ms1 = new MemoryStream();
                            MemoryStream ms2 = new MemoryStream();

                            e.WriteTo(ms1);
                            c2.Write(new SvnUriTarget(e.Uri, e.Revision), ms2);

                            string s1 = Encoding.UTF8.GetString(ms1.ToArray());
                            string s2 = Encoding.UTF8.GetString(ms2.ToArray());

                            //Assert.That(ms1.Length, Is.EqualTo(ms2.Length), "Export lengths equal");
                            Assert.That(s1, Is.EqualTo(s2));
                            i++;
                        });

                    Assert.That(i, Is.EqualTo(6), "Found 6 versions");
                }
            }
        }

        [TestMethod]
        public void FileVersions_WalkMe()
        {
            SvnFileVersionsArgs a = new SvnFileVersionsArgs();
            SvnUriTarget me = new SvnUriTarget(new Uri("https://svn.apache.org/repos/asf/subversion/trunk/win-tests.py"), 874950);
            a.End = 874950;

            int n = 0;

            Client.FileVersions(me, a,
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    GC.KeepAlive(e);
                    e.WriteTo(GetTempFile());
                    n++;
                });

            Assert.That(n, Is.EqualTo(93));
        }

        [TestMethod]
        public void FileVersions_WalkChange()
        {
            SvnFileVersionsArgs a = new SvnFileVersionsArgs();
            SvnUriTarget me = new SvnUriTarget(new Uri("https://svn.apache.org/repos/asf/subversion/trunk/win-tests.py"), 874950);
            a.Start =873511;
            a.End = 873512;
            int n = 0;

            Client.FileVersions(me, a,
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    GC.KeepAlive(e);
                    e.WriteTo(GetTempFile());
                    n++;
                });

            Assert.That(n, Is.EqualTo(2));
        }

#if DEBUG
        [TestMethod]
        public void FileVersions_WriteRelated()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            Uri reposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            Dictionary<SvnUriTarget, Stream> targets = new Dictionary<SvnUriTarget,Stream>();
            Uri fileUri = new Uri(reposUri, "trunk/index.html");

            targets.Add(new SvnUriTarget(fileUri, 5), File.Create(GetTempFile()));
            targets.Add(new SvnUriTarget(fileUri, 7), File.Create(GetTempFile()));

            using(Stream file1 = File.Create(GetTempFile()))
            using(Stream file2 = File.Create(GetTempFile()))
            Client.WriteRelated(
                new SvnUriTarget[]
                {
                    new SvnUriTarget(fileUri, 5),
                    new SvnUriTarget(fileUri, 9)
                },
                new Stream[]
                {
                    file1,
                    file2,
                });
        }
#endif
    }
}

