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
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests for the NSvn.Core.Client.Blame method.
	/// </summary>
	[TestFixture]
	public class BlameTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.blames = new ArrayList();
		}

		[Test]
		public void TestSimple()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");
			string blame = this.RunCommand("svn", "blame -v " + path);
			Blame[] cmdline = this.ParseCommandLineBlame(blame);

			SvnBlameArgs a = new SvnBlameArgs();
			Assert.That(this.Client.Blame(path, a, this.Receiver));

			Assert.That(this.blames.Count, Is.EqualTo(cmdline.Length));
			for (int i = 0; i < cmdline.Length; i++)
			{
				Blame.CheckEqual(cmdline[i], (Blame)this.blames[i]);
			}
		}

		[Test]
		public void TestWithEmptyEntries()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");

			SvnBlameArgs a = new SvnBlameArgs();
			a.Start = SvnRevision.Head;
			a.End = SvnRevision.Head;
			// this won't give any results - verify that there are no exceptions
			Assert.That(this.Client.Blame(path, a, this.Receiver));

			Blame[] b = (Blame[])this.blames.ToArray(typeof(Blame));

			Assert.That(b[0].Revision, Is.EqualTo(-1));
			Assert.That(b[0].Author, Is.EqualTo(null));
			Assert.That(b[0].Date, Is.EqualTo(DateTime.MinValue));
		}

        [Test]
        public void TestMore()
        {
            Uri uri = new Uri(GetReposUri(TestReposType.CollabRepos), "trunk/index.html");

            int n = 0;
            SvnBlameArgs ba = new SvnBlameArgs();
            ba.Notify += delegate(object sender, SvnNotifyEventArgs e)
                {
                    Assert.That(e.Uri, Is.EqualTo(uri));
                    n++;
                };

            int lines = 0;
            Client.Blame(uri, ba,
                delegate(object sender, SvnBlameEventArgs e)
                {
                    Assert.That(e.Author, Is.Not.Null);
                    Assert.That(e.RevisionProperties, Is.Not.Null);
                    Assert.That(e.MergedAuthor, Is.Null);
                    Assert.That(e.MergedRevisionProperties, Is.Null);
                    lines++;
                });

            Assert.That(n, Is.EqualTo(3));
            Assert.That(lines, Is.EqualTo(32));
        }

		private void Receiver(object sender, SvnBlameEventArgs e)
		{
			this.blames.Add(new Blame(e.LineNumber, e.Revision, e.Author, e.Time, e.Line));
		}

		private Blame[] ParseCommandLineBlame(string blame)
		{
			ArrayList blames = new ArrayList();
			long lineNumber = 0;
			foreach (Match m in BlameRegex.Matches(blame))
			{
				int revision = int.Parse(m.Groups["rev"].Value);
				string author = m.Groups["author"].Value;
				DateTime date = DateTime.ParseExact(m.Groups["date"].Value,
					@"yyyy-MM-dd\ HH:mm:ss\ zzzz",
					System.Globalization.CultureInfo.CurrentCulture).ToUniversalTime();
				string line = m.Groups["line"].Value.TrimEnd('\r');
				blames.Add(new Blame(lineNumber++, revision, author, date, line));
			}

			return (Blame[])blames.ToArray(typeof(Blame));
		}

		private class Blame
		{
			public long LineNumber;
			public long Revision;
			public string Author;
			public DateTime Date;
			public string Line;

			public Blame(long lineNumber, long revision, string author,
				DateTime date, string line)
			{
				this.LineNumber = lineNumber;
				this.Revision = revision;
				this.Author = author;
				this.Date = date;
				this.Line = line;
			}

			static DateTime TruncToSecond(DateTime value)
			{
				return new DateTime(value.Ticks - value.Ticks % 10000000L);
			}

			public static void CheckEqual(Blame a, Blame b)
			{
				Assert.That(a.LineNumber, Is.EqualTo(b.LineNumber));
				Assert.That(a.Revision, Is.EqualTo(b.Revision));
				Assert.That(a.Author, Is.EqualTo(b.Author));
				Assert.That(TruncToSecond(a.Date), Is.EqualTo(TruncToSecond(b.Date)));
				Assert.That(a.Line, Is.EqualTo(b.Line));
			}

			private static readonly TimeSpan Second = new TimeSpan(0, 0, 0, 1);
		}

		private readonly Regex BlameRegex = new Regex(
			@"\s+(?<rev>\d+)\s+(?<author>\w+)\s+(?<date>\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d [-+]\d\d\d\d) (\(\w{1,4}, \d\d \w{1,4} \d{4}\) )?(?<line>.*)");

		private ArrayList blames;
	}
}
