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
using System.Collections.ObjectModel;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SharpSvn.TestBuilder;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests the NSvn.Client.MoveFile method
    /// </summary>
    [TestClass]
    public class MergeTests : TestBase
    {
        [TestMethod]
        public void Merge_RevertToBase()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.MergeScenario);

            int nChanges = 0;
            Client.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                nChanges++;
            };

            Assert.That(Client.Merge(sbox.Wc, new Uri(sbox.RepositoryUri, "trunk/"), new SvnRevisionRange(SvnRevision.Head, 2)));

            Assert.That(nChanges, Is.GreaterThan(5));
        }

        [TestMethod]
        public void Merge_MinimalMergeTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Empty);

            using (SvnClient client = NewSvnClient(true, false))
            {
                string merge1 = Path.Combine(sbox.Wc, "mmerge-1");
                string merge2 = Path.Combine(sbox.Wc, "mmerge-2");
                client.CreateDirectory(merge1);

                string f1 = Path.Combine(merge1, "myfile.txt");

                using (StreamWriter fs = File.CreateText(f1))
                {
                    fs.WriteLine("First line");
                    fs.WriteLine("Second line");
                    fs.WriteLine("Third line");
                    fs.WriteLine("Fourth line");
                }

                client.Add(f1);

                SvnCommitResult ci;
                client.Commit(sbox.Wc, out ci);
                client.Copy(new SvnPathTarget(merge1), merge2);
                client.Commit(sbox.Wc);
                client.Update(sbox.Wc);

                SvnMergeSourcesCollection sources;
                client.GetSuggestedMergeSources(new SvnPathTarget(merge1), out sources);

                Assert.That(sources.Count, Is.EqualTo(0));

                client.GetSuggestedMergeSources(new SvnPathTarget(merge2), out sources);

                Assert.That(sources.Count, Is.EqualTo(1));

                Uri fromUri = new Uri(sbox.RepositoryUri, new Uri("mmerge-1", UriKind.Relative));
                Assert.That(sources[0].Uri, Is.EqualTo(fromUri));

                SvnAppliedMergeInfo applied;
                client.GetAppliedMergeInfo(new SvnPathTarget(merge2), out applied);

                Assert.That(applied, Is.Null);
                //Assert.That(applied.AppliedMerges.Count, Is.EqualTo(0));
                //Assert.That(applied.Target, Is.Not.Null);

                Collection<SvnMergesEligibleEventArgs> available;
                client.GetMergesEligible(new SvnPathTarget(merge2), fromUri, out available);
                Assert.That(available, Is.Not.Null);
                Assert.That(available.Count, Is.EqualTo(0));
                /*Assert.That(available[0].Revision, Is.EqualTo(ci.Revision));
                //Assert.That(available.MergeRanges[0].End, Is.EqualTo(ci.Revision + 1));
                //Assert.That(available.MergeRanges[0].Inheritable, Is.True);
                Assert.That(available[0].SourceUri, Is.Not.Null);*/

                using (StreamWriter fs = File.AppendText(f1))
                {
                    fs.WriteLine("Fifth line");
                }
                client.Commit(merge1);

                client.GetMergesEligible(new SvnPathTarget(merge2), fromUri, out available);
                Assert.That(available, Is.Not.Null);
                Assert.That(available.Count, Is.EqualTo(1));
                Assert.That(available[0].Revision, Is.EqualTo(3));
                Assert.That(available[0].SourceUri, Is.Not.Null);

                client.Merge(merge2, fromUri, available[0].AsRange());

                client.GetMergesEligible(new SvnPathTarget(merge2), fromUri, out available);
                Assert.That(available, Is.Not.Null);
                Assert.That(available.Count, Is.EqualTo(0));

                client.Commit(sbox.Wc);

                client.GetMergesEligible(new SvnPathTarget(merge2), fromUri, out available);
                Assert.That(available, Is.Not.Null);
                Assert.That(available.Count, Is.EqualTo(0));

                client.GetAppliedMergeInfo(new SvnPathTarget(merge2), out applied);

                Assert.That(applied, Is.Not.Null);
                Assert.That(applied.AppliedMerges.Count, Is.EqualTo(1));
                Assert.That(applied.AppliedMerges[0].Uri, Is.EqualTo(fromUri));
                Assert.That(applied.AppliedMerges[0].MergeRanges, Is.Not.Null);
                Assert.That(applied.AppliedMerges[0].MergeRanges.Count, Is.EqualTo(1));
                Assert.That(applied.AppliedMerges[0].MergeRanges[0].Start, Is.EqualTo(ci.Revision + 1));
                Assert.That(applied.AppliedMerges[0].MergeRanges[0].End, Is.EqualTo(ci.Revision + 2));
                Assert.That(applied.AppliedMerges[0].MergeRanges[0].Inheritable, Is.True);
                Assert.That(applied.Target, Is.Not.Null);
            }
        }

        [TestMethod]
        public void Merge_DiffMerge()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            sbox.Create(SandBoxRepository.MergeScenario);
            string wc = sbox.Wc;

            Client.DiffMerge(Path.Combine(wc, "index.html"), new Uri(sbox.RepositoryUri, "trunk/index.html"), new Uri(sbox.RepositoryUri, "trunk/index.html"));
        }

        [TestMethod, ExpectedException(typeof(ArgumentException))]
        public void Merge_DiffMergeLocal()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            sbox.Create(SandBoxRepository.MergeScenario);
            string wc = sbox.Wc;

            string path = Path.Combine(wc, "index.html");

            Client.DiffMerge(path, path, path);
        }
    }
}
