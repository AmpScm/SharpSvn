// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the NSvn.Client.MoveFile method
	/// </summary>
	[TestFixture]
	public class MergeTests : TestBase
	{
		[Test]
		public void RevertToBase()
		{
			string dir = GetTempDir();
			Assert.That(Client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir));
			
			int nChanges = 0;
			Client.Notify += delegate(object sender, SvnNotifyEventArgs e)
			{
				nChanges++;
			};

			Assert.That(Client.Merge(dir, new Uri(CollabReposUri, "trunk/"), new SvnRevisionRange(SvnRevision.Head, 2)));

			Assert.That(nChanges, Is.GreaterThan(5));
		}

        [Test]
        public void MinimalMergeTest()
        {
            using (SvnRepositoryClient rc = new SvnRepositoryClient())
            {
                rc.UpgradeRepository(ReposPath);
            }
            using (SvnClient client = NewSvnClient(true, false))
            {
                string merge1 = Path.Combine(WcPath, "mmerge-1");
                string merge2 = Path.Combine(WcPath, "mmerge-2");
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
                client.Commit(WcPath, out ci);
                client.Copy(new SvnPathTarget(merge1), merge2);
                client.Commit(WcPath);
                client.Update(WcPath);

                SvnMergeSourcesCollection sources;
                client.GetSuggestedMergeSources(new SvnPathTarget(merge1), out sources);

                Assert.That(sources.Count, Is.EqualTo(0));

                client.GetSuggestedMergeSources(new SvnPathTarget(merge2), out sources);

                Assert.That(sources.Count, Is.EqualTo(1));

                Uri fromUri = new Uri(ReposUrl, new Uri("mmerge-1", UriKind.Relative));
                Assert.That(sources[0].Uri, Is.EqualTo(fromUri));

                SvnAppliedMergeInfo applied;
                client.GetAppliedMergeInfo(new SvnPathTarget(merge2), out applied);

                Assert.That(applied, Is.Not.Null);
                Assert.That(applied.AppliedMerges.Count, Is.EqualTo(0));
                Assert.That(applied.Target, Is.Not.Null);

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
                Assert.That(available[0].Revision, Is.EqualTo(9L));
                Assert.That(available[0].SourceUri, Is.Not.Null);

                client.Merge(merge2, fromUri, available[0].AsRange());

                client.GetMergesEligible(new SvnPathTarget(merge2), fromUri, out available);
                Assert.That(available, Is.Not.Null);
                Assert.That(available.Count, Is.EqualTo(0));

                client.Commit(WcPath);

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
	}
}
