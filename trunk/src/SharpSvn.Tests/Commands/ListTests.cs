// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;
using System.Net;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::List
	/// </summary>
	[TestFixture]
	public class ListTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractRepos();
		}

		/// <summary>
		/// Compares the list from the command line client with that obtained
		/// from Client::List
		/// </summary>
		[Test]
		public void TestList()
		{
			string list = this.RunCommand("svn", "list -v " + this.ReposUrl);

			// clean out whitespace
			string[] entries = Regex.Split(list, @"\r\n");
			//string[] entries = list.Trim().Split( '\n' );
			Hashtable ht = new Hashtable();
			foreach (string e in entries)
			{
				if (e != "")
				{
					Entry ent = new Entry(e);
					ht[ent.Path] = ent;
				}
			}

			Collection<SvnListEventArgs> apiList;

			SvnListArgs a = new SvnListArgs();
			a.Depth = SvnDepth.Children;

			Assert.That(Client.GetList(new SvnUriTarget(ReposUrl, SvnRevision.Head), a, out apiList));

			Assert.That(apiList.Count, Is.EqualTo(ht.Count), "Wrong number of entries returned");

			foreach (SvnListEventArgs ent in apiList)
			{
				string path = ent.Path;

				if (path == "")
					break; // New in 1.4+ ; was not available in ankh tests, as svn_client_ls was used instead of svn_client_list

				if (ent.Entry.NodeKind == SvnNodeKind.Directory)
					path += "/";

				Entry entry = (Entry)ht[path];
				Assert.IsNotNull(entry, "No entry found for " + path);

				entry.Match(ent.Entry);
			}
		}

        [Test]
        public void TestRoot()
        {
            using (SvnClient client = new SvnClient())
            {
                client.List(CollabReposUri, delegate(object sender, SvnListEventArgs e)
                {
                    Assert.That(e.RepositoryRoot, Is.EqualTo(CollabReposUri));
                    Assert.That(e.RepositoryRoot.ToString().EndsWith("/"));
                });
            }
        }

        [Test]
        public void ListSharp()
        {
            using (SvnClient client = new SvnClient())
            {
                Client.Authentication.Clear();
                Client.Authentication.DefaultCredentials = new NetworkCredential("guest", "");

                client.List(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk"),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        Assert.That(e.RepositoryRoot, Is.EqualTo(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/")));
                        Assert.That(e.BaseUri, Is.EqualTo(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk/")));
                        Assert.That(e.BasePath, Is.EqualTo("/trunk"));
                    });

            }
        }

        [Test]
        public void WorstLocalDir()
        {
            Uri uri = CollabReposUri;
            string tmp = GetTempDir();

            Client.CheckOut(uri, tmp);

            string s1 = "start #";
            string s2 = "q!@#$%^&()_- +={}[]',.Æeiaæå";
            string s3  = "done!";

            string d = tmp;
            Directory.CreateDirectory(d = Path.Combine(d, s1));
            Directory.CreateDirectory(d = Path.Combine(d, s2));
            Directory.CreateDirectory(d = Path.Combine(d, s3));

            string f1, f2;
            TouchFile(f1 = Path.Combine(d, "file.txt"));
            TouchFile(f2 = Path.Combine(d, s2 + ".txt"));

            SvnAddArgs aa = new SvnAddArgs();
            aa.AddParents = true;
            Client.Add(f1, aa);
            Client.Add(f2, aa);
            Client.Commit(tmp);

            SvnInfoEventArgs ea;
            Client.GetInfo(d, out ea);

            Uri r = new Uri(new Uri(new Uri(uri, SvnTools.PathToRelativeUri(s1 + "/")), SvnTools.PathToRelativeUri(s2 + "/")), SvnTools.PathToRelativeUri(s3+"/"));

            Assert.That(r.ToString(), Is.EqualTo(ea.Uri.ToString()));

            // Run with a .Net normalized Uri
            Client.List(r,
                delegate(object sender, SvnListEventArgs e)
                {
                    Assert.That(e.RepositoryRoot, Is.EqualTo(uri));
                });

            // Run with a SVN normalized Uri
            Client.List(ea.Uri,
                delegate(object sender, SvnListEventArgs e)
                {
                    Assert.That(e.RepositoryRoot, Is.EqualTo(uri));
                });

            r = new Uri(new Uri(new Uri(uri, SvnTools.PathToRelativeUri(s1 + "//")), SvnTools.PathToRelativeUri(s2 + "///")), SvnTools.PathToRelativeUri(s3 + "////"));

            // Run with uncanonical Uri
            Client.List(r,
                delegate(object sender, SvnListEventArgs e)
                {
                    Assert.That(e.RepositoryRoot, Is.EqualTo(uri));
                });
        }

        [Test]
        public void TestHash()
        {
            using (SvnClient client = new SvnClient())
            {
                Uri reposUri = new Uri("https://svn.apache.org/repos/asf/");
                client.List(new SvnUriTarget(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/"), 656380),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        if (string.IsNullOrEmpty(e.Path))
                        {
                            Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                            return;
                        }

                        Assert.That(e.EntryUri, Is.EqualTo(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/")));
                    });

                client.List(new SvnUriTarget(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/"), 656380),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        if (string.IsNullOrEmpty(e.Path))
                        {
                            Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                            Assert.That(e.BaseUri, Is.EqualTo(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/")));
                            return;
                        }
                    });

                client.List(new SvnUriTarget(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/src/"), 656380),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        if (string.IsNullOrEmpty(e.Path))
                        {
                            Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                            Assert.That(e.BaseUri, Is.EqualTo(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/src/")));
                            return;
                        }
                    });
            }
        }

        [Test]
        public void TestSpace()
        {
            using(SvnClient client = new SvnClient())
            {
                int n = 0;
                client.List(new Uri("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with spaces"),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        if (string.IsNullOrEmpty(e.Path))
                        {
                            Assert.That(e.RepositoryRoot, Is.EqualTo(new Uri("http://sharpsvn.googlecode.com/svn/")));
                            Assert.That(e.BaseUri, Is.EqualTo(new Uri("http://sharpsvn.googlecode.com/svn/trunk/tests/folder%20with spaces/")));
                            return;
                        }

                        n++;
                    }
                );

                Assert.That(n, Is.EqualTo(2));
            }
        }

		private class Entry
		{
			public Entry(string line)
			{
				if (!Reg.IsMatch(line))
					throw new Exception("Commandline client bad output");

				Match match = Reg.Match(line);

				this.createdRevision = int.Parse(match.Groups[1].ToString());
				this.author = match.Groups[2].ToString();

				if (match.Groups[3].Success)
					this.size = long.Parse(match.Groups[3].ToString());
				else
					this.size = 0;

				System.IFormatProvider format =
					System.Globalization.CultureInfo.CurrentCulture;

				// get the month and day
				string date = match.Groups[4].ToString();
				this.time = DateTime.ParseExact(date, "MMM' 'dd",
					format);

				// the year
				if (match.Groups[5].Success)
				{
					this.time = this.time.AddYears(-this.time.Year +
						int.Parse(match.Groups[5].ToString()));
				}

				// or the time of day?
				DateTime timeOfDay = DateTime.Today;
				if (match.Groups[6].Success)
				{
					timeOfDay = DateTime.ParseExact(match.Groups[6].ToString(),
						"HH':'mm", format);
				}
				this.time = this.time.AddHours(timeOfDay.Hour);
				this.time = this.time.AddMinutes(timeOfDay.Minute);

				this.path = match.Groups[7].ToString();
			}

			public void Match(SvnDirEntry ent)
			{
				Assert.That(ent.Revision, Is.EqualTo(this.createdRevision),
					"CreatedRevision differs");
				Assert.That(ent.FileSize, Is.EqualTo(this.size),
					"Size differs");

				// strip off time portion
				DateTime entryTime = ent.Time.ToLocalTime();
				entryTime = entryTime - entryTime.TimeOfDay;

				long delta = Math.Abs(this.time.Ticks - entryTime.Ticks);
				Assert.That(delta < TICKS_PER_MINUTE,
					"Time differs: " + this.time + " vs " +
					entryTime + " Delta is " + delta);
				Assert.That(ent.Author, Is.EqualTo(this.author), "Last author differs");
			}

			public string Path
			{
				get { return this.path; }
			}

			private const long TICKS_PER_MINUTE = 600000000;

			private long createdRevision;
			private string author;
			private long size;
			private DateTime time;
			private string path;
			private static readonly Regex Reg = new Regex(
				@"\s+(\d+)\s+(\w+)\s+(\d+)?\s+(\w+\s\d+)\s+(?:(\d{4})|(\d\d:\d\d))\s+(\S+)");
		}
	}
}
