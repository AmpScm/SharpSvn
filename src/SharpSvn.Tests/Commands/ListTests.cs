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
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Reflection;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn.Security;
using System.Threading.Tasks;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::List
    /// </summary>
    [TestClass]
    public class ListTests : TestBase
    {
        /// <summary>
        /// Compares the list from the command line client with that obtained
        /// from Client::List
        /// </summary>
        [TestMethod]
        public void List_TestList()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.AnkhSvnCases);
            string list = this.RunCommand("svn", "list -v " + ReposUrl);

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

        [TestMethod]
        public void List_TestRoot()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            using (SvnClient client = new SvnClient())
            {
                client.List(CollabReposUri, delegate(object sender, SvnListEventArgs e)
                {
                    Assert.That(e.RepositoryRoot, Is.EqualTo(CollabReposUri));
                    Assert.That(e.RepositoryRoot.ToString().EndsWith("/"));
                });
            }
        }

        [TestMethod]
        public void List_TestListReflection()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Type svnClientType = Type.GetType("SharpSvn.SvnClient, SharpSvn");
            Type svnUriTarget = Type.GetType("SharpSvn.SvnUriTarget, SharpSvn");

            object client = Activator.CreateInstance(svnClientType);
            object target = Activator.CreateInstance(svnUriTarget, sbox.CreateRepository(SandBoxRepository.MergeScenario));

            object[] args = new object[] { target, null };

            svnClientType.InvokeMember("GetList", BindingFlags.Instance | BindingFlags.InvokeMethod | BindingFlags.Public, null, client, args);

            IList list = args[1] as IList;

            Assert.That(list, Is.Not.Null);
            Assert.That(list.Count, Is.GreaterThan(0));
        }

        [TestMethod]
        public void List_ReadDash()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string dir = sbox.Wc;
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

        [TestMethod]
        public void List_ListSharp()
        {
            using (SvnClient client = NewSvnClient(false, false))
            {
                Client.Authentication.Clear();
                Client.Authentication.DefaultCredentials = new NetworkCredential("guest", "");

                client.List(new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn/trunk"),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        Assert.That(e.RepositoryRoot, Is.EqualTo(new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn/")));
                        Assert.That(e.BaseUri, Is.EqualTo(new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn/trunk/")));
                        Assert.That(e.BasePath, Is.EqualTo("/trunk"));
                    });

            }
        }

        [TestMethod, Ignore]
        public void List_ParallelList()
        {
            List<Task> handlers = new List<Task>();
            for (int i = 0; i < 32; i++)
            {
                int n = i;

                handlers.Add(Task.Run(() =>
                {
                    Trace.WriteLine("Starting job" + n.ToString());
                    using (var cl = new SvnClient())
                    {
                        SetupAuth(cl);
                        cl.List(new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn/trunk/src/SharpSvn"),
                        delegate { });
                    }
                }));
            }

            Task.WaitAll(handlers.ToArray());
        }

        [TestMethod]
        public void List_ParallelLocalList()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            Uri reposUri = new Uri(CollabReposUri, "trunk/");
            List<Task> handlers = new List<Task>();
            for (int i = 0; i < 128; i++)
            {
                int n = i;

                handlers.Add(Task.Run(() =>
                {
                    Trace.WriteLine("Starting job" + n.ToString());
                    new SvnClient().List(reposUri,
                        delegate { });
                }));
            }

            Task.WaitAll(handlers.ToArray());
        }

        [TestMethod]
        public void List_WorstLocalDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            Uri uri = CollabReposUri;
            string tmp = sbox.GetTempDir();

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

            if (Environment.Version.Major < 4 && !IsCore())
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

        [TestMethod]
        public void List_ListDepth()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            Uri url = new Uri(CollabReposUri, "trunk/");
            string dir = sbox.Wc;
            Client.CheckOut(url, dir);

            foreach (SvnDepth d in new SvnDepth[] { SvnDepth.Infinity, SvnDepth.Children, SvnDepth.Files, SvnDepth.Empty })
            {
                Collection<SvnListEventArgs> remoteList, localList;
                SvnListArgs la = new SvnListArgs();
                la.Depth = d;

                Client.GetList(url, la, out remoteList);
                Client.GetList(dir, la, out localList);

                Assert.That(remoteList.Count, Is.EqualTo(localList.Count));

                switch (d)
                {
                    case SvnDepth.Infinity:
                        Assert.That(localList.Count, Is.EqualTo(16));
                        break;
                    case SvnDepth.Children:
                        Assert.That(localList.Count, Is.EqualTo(7));
                        break;
                    case SvnDepth.Files:
                        Assert.That(localList.Count, Is.EqualTo(2));
                        break;
                    case SvnDepth.Empty:
                        Assert.That(localList.Count, Is.EqualTo(1));
                        break;
                }
            }
        }

        [TestMethod]
        public void List_TestHash()
        {
            using (SvnClient client = NewSvnClient(false, false))
            {
                Uri reposUri = new Uri("https://ctf.open.collab.net/svn/repos/ankhsvn/");
                string baseUri = "https://ctf.open.collab.net/svn/repos/ankhsvn/testcases/trunk/WorstCase/AllTypesSolution/";
                string exUri = baseUri + "%e3%83%97%e3%83%ad%e3%82%b0%e3%83%a9%e3%83%9f%e3%83%b3%e3%82%b0%23Silverlight/";

                bool found = false;
                client.List(new SvnUriTarget(new Uri(baseUri), 11888),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        if (string.IsNullOrEmpty(e.Path))
                        {
                            Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                            return;
                        }

                        if (e.Uri == new Uri(exUri))
                            found = true;
                    });

                Assert.That(found, "Found subdir");
                found = false;

                client.List(new SvnUriTarget(new Uri(baseUri), 11888),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        if (string.IsNullOrEmpty(e.Path))
                        {
                            Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                            Assert.That(e.BaseUri, Is.EqualTo(new Uri(baseUri)));
                            return;
                        }
                    });

                client.List(new SvnUriTarget(new Uri(exUri + "Properties/"), 11888),
                    delegate(object sender, SvnListEventArgs e)
                    {
                        if (string.IsNullOrEmpty(e.Path))
                        {
                            Assert.That(e.RepositoryRoot, Is.EqualTo(reposUri));
                            Assert.That(e.BaseUri, Is.EqualTo(new Uri(exUri + "Properties/")));
                            return;
                        }
                    });
            }
        }

        [TestMethod]
        public void List_RemoteListTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Greek);

            using (SvnClient client = NewSvnClient(false, false))
            {
                Collection<SvnListEventArgs> items;
                client.GetList(ReposUrl, out items);

                Assert.That(items, Is.Not.Null, "Items retrieved");
                Assert.That(items.Count, Is.GreaterThan(0), "More than 0 items");
            }
        }

        [TestMethod]
        public void List_TestLowerDrive()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            string tmpDir = sbox.GetTempDir();
            if (tmpDir.Contains(":"))
                return; // Testing on UNC share

            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Greek);

            using (SvnClient client = NewSvnClient(false, false))
            {
                Uri origUri = ReposUrl;
                string uriString = origUri.AbsoluteUri;
                SvnUriTarget target = new SvnUriTarget("file:///" + char.ToLower(uriString[8]) + uriString.Substring(9));

                client.List(target, delegate(object sender, SvnListEventArgs e)
                {
                });

                target = new SvnUriTarget("file://localhost/" + char.ToLower(uriString[8]) + uriString.Substring(9));

                client.List(target, delegate(object sender, SvnListEventArgs e)
                {
                });
            }
        }

        //[TestMethod]
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

        [TestMethod]
        public void List_ListDetails()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            string WcPath = sbox.Wc;
            Uri WcUri = sbox.Uri;

            using (SvnClient client = NewSvnClient(true, false))
            {

                string oneFile = Path.Combine(WcPath, "LocalFileForTestList");
                TouchFile(oneFile);
                client.Add(oneFile);

                SvnCommitResult ci;
                client.Commit(WcPath, out ci);
                SvnUpdateResult r;
                client.Update(WcPath, out r);

                Assert.That(r, Is.Not.Null);
                Assert.That(r.HasRevision);
                Assert.That(r.HasResultMap);
                Assert.That(r.Revision, Is.EqualTo(ci.Revision));

                bool visited = false;
                SvnListArgs a = new SvnListArgs();
                a.RetrieveEntries = SvnDirEntryItems.AllFieldsV15;

                client.List(new SvnPathTarget(WcPath), a, delegate(object sender, SvnListEventArgs e)
                {
                    Assert.That(e.Entry, Is.Not.Null, "Entry set");
                    Assert.That(e.RepositoryRoot, Is.Null, "Only valid when listing a Uri");

                    if (e.Path == "LocalFileForTestList")
                    {
                        Assert.That(e.BasePath, Is.EqualTo("/trunk"), "Basepath");
                        Assert.That(e.Lock, Is.Null);
                        Assert.That(e.Entry.Author, Is.EqualTo(Environment.UserName));
                        Assert.That(e.Entry.FileSize, Is.EqualTo(0));
                        Assert.That(e.Entry.NodeKind, Is.EqualTo(SvnNodeKind.File));
                        Assert.That(e.Entry.Revision, Is.EqualTo(ci.Revision));
                        Assert.That(e.Entry.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                        visited = true;
                    }
                });
                Assert.That(visited, Is.True, "Visited is true");


                visited = false;
                client.List(WcUri, a, delegate(object sender, SvnListEventArgs e)
                {
                    Assert.That(e.Entry, Is.Not.Null, "Entry set");

                    if (e.Path == "LocalFileForTestList")
                    {
                        Assert.That(e.BasePath, Is.EqualTo("/trunk"), "Basepath");
                        Assert.That(e.Lock, Is.Null);
                        Assert.That(e.Entry.Author, Is.EqualTo(Environment.UserName));
                        Assert.That(e.Entry.FileSize, Is.EqualTo(0));
                        Assert.That(e.Entry.NodeKind, Is.EqualTo(SvnNodeKind.File));
                        Assert.That(e.Entry.Revision, Is.EqualTo(ci.Revision));
                        Assert.That(e.Entry.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                        visited = true;
                    }
                });
                Assert.That(visited, Is.True, "Visited is true");

                SvnWorkingCopyClient wcC = new SvnWorkingCopyClient();
                SvnWorkingCopyState state;
                Assert.That(wcC.GetState(oneFile, out state));

                Assert.That(state, Is.Not.Null);
                Assert.That(state.IsTextFile, Is.True);

                client.SetProperty(oneFile, "svn:mime-type", "application/binary");

                Assert.That(wcC.GetState(oneFile, out state));

                Assert.That(state, Is.Not.Null);
                Assert.That(state.IsTextFile, Is.False);
            }
        }
    }
}
