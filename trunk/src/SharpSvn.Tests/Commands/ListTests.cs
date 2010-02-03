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
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;
using System.Net;
using System.Collections.Generic;
using System.Threading;
using System.Diagnostics;
using System.Reflection;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::List
    /// </summary>
    [TestFixture]
    public class ListTests : TestBase
    {
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
        public void TestListReflection()
        {
            Type svnClientType = Type.GetType("SharpSvn.SvnClient, SharpSvn");
            Type svnUriTarget = Type.GetType("SharpSvn.SvnUriTarget, SharpSvn");

            object client = Activator.CreateInstance(svnClientType);
            object target = Activator.CreateInstance(svnUriTarget, GetReposUri(TestReposType.CollabRepos));

            object[] args = new object[] { target, null };

            svnClientType.InvokeMember("GetList", BindingFlags.Instance | BindingFlags.InvokeMethod | BindingFlags.Public, null, client, args);

            IList list = args[1] as IList;

            Assert.That(list, Is.Not.Null);
            Assert.That(list.Count, Is.GreaterThan(0));
        }

        [Test]
        public void ReadDash()
        {
            string dir = GetTempDir();
            Client.CheckOut(new Uri(CollabReposUri, "trunk"), dir);

            string file = Path.Combine(dir, "File#Dash");
            File.WriteAllText(file, "######");
            Client.Add(file);
            Client.Commit(dir);

            bool touched = false;
            Client.List(file,
                delegate(object sender, SvnListEventArgs e)
                {
                    touched = true;
                    Assert.That(e.RepositoryRoot, Is.Null);
                    Assert.That(e.BasePath.EndsWith("/File#Dash"));
                    Assert.That(e.Name, Is.EqualTo("File#Dash"));
                });

            Assert.That(touched);
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

        [Test, Ignore("Don't run this as normal test")]
        public void ParallelList()
        {
            List<IAsyncResult> handlers = new List<IAsyncResult>();
            for (int i = 0; i < 32; i++)
            {
                int n = i;
                EventHandler eh = delegate
                {
                    Trace.WriteLine("Starting job" + n.ToString());
                    new SvnClient().List(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk/src/SharpSvn"),
                        delegate { });
                };

                handlers.Add(eh.BeginInvoke(null, EventArgs.Empty, null, eh));
            }

            foreach (IAsyncResult ar in handlers)
            {
                ((EventHandler)ar.AsyncState).EndInvoke(ar);
            }
        }

        [Test]
        public void ParallelLocalList()
        {
            Uri reposUri = new Uri(CollabReposUri, "trunk/");
            List<IAsyncResult> handlers = new List<IAsyncResult>();
            for (int i = 0; i < 128; i++)
            {
                int n = i;
                EventHandler eh = delegate
                {
                    Trace.WriteLine("Starting job" + n.ToString());
                    new SvnClient().List(reposUri,
                        delegate { });
                };

                handlers.Add(eh.BeginInvoke(null, EventArgs.Empty, null, eh));
            }

            foreach (IAsyncResult ar in handlers)
            {
                ((EventHandler)ar.AsyncState).EndInvoke(ar);
            }
        }

        [Test]
        public void WorstLocalDir()
        {
            Uri uri = CollabReposUri;
            string tmp = GetTempDir();

            Client.CheckOut(uri, tmp);

            string s1 = "start #";
            string s2 = "q!@#$%^&()_- +={}[]',.eia";
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

                        Assert.That(e.Uri, Is.EqualTo(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/")));
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
        public void RemoteListTest()
        {
            using (SvnClient client = NewSvnClient(false, false))
            {
                Collection<SvnListEventArgs> items;
                client.GetList(ReposUrl, out items);

                Assert.That(items, Is.Not.Null, "Items retrieved");
                Assert.That(items.Count, Is.GreaterThan(0), "More than 0 items");
            }
        }

        //[Test]
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
