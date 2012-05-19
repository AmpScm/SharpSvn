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

// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using NUnit.Framework;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::Diff
	/// </summary>
	[TestFixture]
	public class DiffTests : TestBase
	{
		[Test]
		public void TestLocalDiff()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");


			using (StreamWriter w = new StreamWriter(form, false))
				w.Write("Moo moo moo moo moo\r\nmon\r\nmooo moo moo \r\nssdgo");

			//Necessary to fix up headers

			string clientDiff = this.RunCommand("svn", "diff \"" + form + "\"");

			MemoryStream outstream = new MemoryStream();
			MemoryStream errstream = new MemoryStream();

			SvnDiffArgs a = new SvnDiffArgs();
			a.ErrorStream = errstream;
			this.Client.Diff(
				new SvnPathTarget(form, SvnRevision.Base),
				new SvnPathTarget(form, SvnRevision.Working),
				a, outstream);

			string err = Encoding.Default.GetString(errstream.ToArray());
			Assert.That(err, Is.EqualTo(""), "Error in diff: " + err);
			string apiDiff = Encoding.Default.GetString(outstream.ToArray());
			Assert.That(apiDiff, Is.EqualTo(clientDiff), "Client diff differs");
		}

		[Test]
		public void TestReposDiff()
		{
			string clientDiff = this.RunCommand("svn", "diff -r 1:5 " + this.ReposUrl);

			MemoryStream outstream = new MemoryStream();
			MemoryStream errstream = new MemoryStream();

			SvnDiffArgs a = new SvnDiffArgs();
			a.ErrorStream = errstream;
			Client.Diff(ReposUrl, new SvnRevisionRange(1, 5), a, outstream);

			string err = Encoding.Default.GetString(errstream.ToArray());
			Assert.That(err, Is.EqualTo(string.Empty), "Error in diff: " + err);

			string apiDiff = Encoding.Default.GetString(outstream.ToArray());
            string[] clientLines = clientDiff.Split('\n');
            string[] apiLines = apiDiff.Split('\n');
            Array.Sort<string>(clientLines);
            Array.Sort<string>(apiLines);

			Assert.That(apiLines, Is.EqualTo(clientLines), "Diffs differ");
		}

		[Test]
		public void TestDiffBinary()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "propset svn:mime-type application/octet-stream " +
				path);
			this.RunCommand("svn", "ci -m '' " + path);

			using (StreamWriter w = new StreamWriter(path))
				w.WriteLine("Hi there");


			MemoryStream outstream = new MemoryStream();
			MemoryStream errstream = new MemoryStream();

			SvnDiffArgs a = new SvnDiffArgs();
			a.ErrorStream = errstream;

			// this should not diff a binary file
			Assert.That(Client.Diff(
				path,
				new SvnRevisionRange(SvnRevision.Base, SvnRevision.Working),
				a,
				outstream));

			string diff = Encoding.ASCII.GetString(outstream.ToArray());
			Assert.That(diff.IndexOf("application/octet-stream") >= 0);


			outstream = new MemoryStream();
			errstream = new MemoryStream();
			a = new SvnDiffArgs();
			a.ErrorStream = errstream;
			a.IgnoreContentType = true;

			this.Client.Diff(
				path,
				new SvnRevisionRange(SvnRevision.Base, SvnRevision.Working),
				a,
				outstream);

			Assert.That(outstream.Length > 0);
			diff = Encoding.ASCII.GetString(outstream.ToArray());
			Assert.That(diff.IndexOf("application/octet-stream") < 0);
		}

        [Test]
        public void RunDiffTests()
        {
            string start = Guid.NewGuid().ToString() + Environment.NewLine + Guid.NewGuid().ToString();
            string end = Guid.NewGuid().ToString() + Environment.NewLine + Guid.NewGuid().ToString();
            string origLine = Guid.NewGuid().ToString();
            string newLine = Guid.NewGuid().ToString();
            using (SvnClient client = NewSvnClient(true, false))
            {
                string diffFile = Path.Combine(WcPath, "DiffTest");

                using (StreamWriter sw = File.CreateText(diffFile))
                {
                    sw.WriteLine(start);
                    sw.WriteLine(origLine);
                    sw.WriteLine(end);
                }

                client.Add(diffFile);
                SvnCommitResult ci;
                client.Commit(diffFile, out ci);

                using (StreamWriter sw = File.CreateText(diffFile))
                {
                    sw.WriteLine(start);
                    sw.WriteLine(newLine);
                    sw.WriteLine(end);
                }

                MemoryStream diffOutput = new MemoryStream();

                client.Diff(diffFile, new SvnPathTarget(diffFile, SvnRevisionType.Head), diffOutput);
                VerifyDiffOutput(origLine, newLine, diffOutput);

                diffOutput = new MemoryStream();

                client.Diff(diffFile, new SvnPathTarget(diffFile, SvnRevisionType.Committed), diffOutput);
                VerifyDiffOutput(origLine, newLine, diffOutput);

                diffOutput = new MemoryStream();

                client.Diff(diffFile, new Uri(ReposUrl, "DiffTest"), diffOutput);
                VerifyDiffOutput(origLine, newLine, diffOutput);

                SvnCommitResult info;
                client.Commit(diffFile, out info);

                bool visited = false;
                client.DiffSummary(new SvnUriTarget(ReposUrl, info.Revision - 1), ReposUrl,
                    delegate(object sender, SvnDiffSummaryEventArgs e)
                    {
                        if (e.Path == "DiffTest")
                        {
                            Assert.That(e.DiffKind, Is.EqualTo(SvnDiffKind.Modified));
                            Assert.That(e.PropertiesChanged, Is.False);
                            visited = true;
                        }
                    });

                Assert.That(visited);

                int n = 0;

                client.Blame(new SvnPathTarget(diffFile), delegate(object sender, SvnBlameEventArgs e)
                {
                    Assert.That(e.Author, Is.EqualTo(Environment.UserName));
                    Assert.That(e.LineNumber, Is.EqualTo((long)n));
                    switch (n)
                    {
                        case 0:
                        case 1:
                        case 3:
                        case 4:
                            Assert.That(e.Revision, Is.EqualTo(ci.Revision));
                            break;
                        case 2:
                            Assert.That(e.Revision, Is.EqualTo(info.Revision));
                            break;
                        default:
                            Assert.That(false, "EOF");
                            break;
                    }
                    Assert.That(e.Line, Is.Not.Null);
                    n++;
                });

                Assert.That(n, Is.EqualTo(5), "Blame receiver received 5 lines");

                string tempFile = GetTempFile();
                using(FileStream fs = File.Create(tempFile))
                {
                    SvnDiffArgs da = new SvnDiffArgs();
                    da.RelativeToPath = Path.GetDirectoryName(diffFile);
                    client.Diff(diffFile, new SvnRevisionRange(ci.Revision, SvnRevisionType.Head), da, fs);
                }

                client.Update(diffFile, new SvnUpdateArgs { Revision = ci.Revision });
                SvnPatchArgs pa = new SvnPatchArgs();
                n = 0;
                pa.Filter += delegate(object sender, SvnPatchFilterEventArgs e)
                {
                    Assert.That(File.Exists(e.RejectPath));
                    Assert.That(File.Exists(e.ResultPath));
                    Assert.That(new FileInfo(e.RejectPath).Length, Is.EqualTo(0));
                    n++;
                };
                client.Patch(tempFile, Path.GetDirectoryName(diffFile), pa);
                Assert.That(n, Is.EqualTo(1));
            }
        }

        private static void VerifyDiffOutput(string origLine, string newLine, Stream diffOutput)
        {
            diffOutput.Position = 0;
            using (StreamReader sr = new StreamReader(diffOutput))
            {
                bool foundMin = false;
                bool foundPlus = false;

                string line;

                while (null != (line = sr.ReadLine()))
                {
                    if (line.Contains(newLine))
                    {
                        Assert.That(line.StartsWith("+"));
                        foundPlus = true;
                    }
                    else if (line.Contains(origLine))
                    {
                        Assert.That(line.StartsWith("-"));
                        foundMin = true;
                    }
                }
                Assert.That(foundMin, "Found -");
                Assert.That(foundPlus, "Found +");
            }
        }


	}
}
