using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn.Remote;
using SharpSvn.Tests.Commands;

namespace SharpSvn.Tests.RemoteTests
{
    [TestClass]
    public class RemoteTests : TestBase
    {
        [TestMethod]
        public void Remote_TestOptions()
        {
            Assert.That(SvnRemoteSession.IsConnectionlessRepository(new Uri("http://svn.collab.net/")));
            Assert.That(SvnRemoteSession.RequiresExternalAuthorization(new Uri("http://svn.collab.net/")), Is.False);
        }

        [TestMethod]
        public void Remote_OpenRepos()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            using (SvnRemoteSession r = new SvnRemoteSession(CollabReposUri))
            {
                r.Reparent(CollabReposUri);
                long rev;
                Uri uri;
                Guid id;
                SvnRemoteStatEventArgs stat;
                long foundRev;

                r.GetLatestRevision(out rev);
                r.GetRepositoryRoot(out uri);
                r.GetRepositoryId(out id);
                r.GetStat(null, out stat);
                r.ResolveRevision(DateTime.Now, out foundRev);

                Assert.That(uri, Is.EqualTo(CollabReposUri));
                Assert.That(rev, Is.EqualTo(17L));
                Assert.That(id, Is.EqualTo(new Guid("d4e3f7eb-7f47-4c2b-9c6e-ba21fc616240")));
                Assert.That(stat, Is.Not.Null);
                Assert.That(stat.Entry, Is.Not.Null);
                Assert.That(stat.Entry.Author, Is.EqualTo("merger"));
                Assert.That(stat.Entry.NodeKind, Is.EqualTo(SvnNodeKind.Directory));

                int n = 0;
                r.List("",
                    delegate(object sender, SvnRemoteListEventArgs e)
                    {
                        Assert.That(e.Name, Is.Not.Null);
                        n++;
                    });

                Assert.That(n, Is.EqualTo(3));
                Assert.That(r.SessionUri, Is.Not.Null);
                Assert.That(r.IsDisposed, Is.False);
            }
        }

        [TestMethod]
        public void Remote_ListLocks()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri tmpRepos = sbox.CreateRepository(SandBoxRepository.Greek);
            using (SvnRemoteSession rs = new SvnRemoteSession())
            {
                rs.Open(tmpRepos);

                int n = 0;
                rs.ListLocks("",
                    delegate(object sender, SvnRemoteListLockEventArgs e)
                    {
                        n++;
                    });

                Assert.That(n, Is.EqualTo(0));
            }

            using(SvnClient cl = new SvnClient())
            {
                cl.RemoteLock(new Uri(tmpRepos, "trunk/iota"), "");
                cl.RemoteLock(new Uri(tmpRepos, "trunk/A/D/gamma"), "");
            }

            using (SvnRemoteSession rs = new SvnRemoteSession())
            {
                rs.Open(tmpRepos);

                int n = 0;
                rs.ListLocks("trunk/A",
                    delegate(object sender, SvnRemoteListLockEventArgs e)
                    {
                        n++;
                    });

                Assert.That(n, Is.EqualTo(1));

                n = 0;
                rs.ListLocks("trunk",
                    delegate(object sender, SvnRemoteListLockEventArgs e)
                    {
                        n++;
                    });

                Assert.That(n, Is.EqualTo(2));
            }
        }

        [TestMethod]
        public void Remote_RemoteList()
        {
            DateTime start = DateTime.Now;
            using (SvnRemoteSession rc = new SvnRemoteSession())
            {
                rc.Open(new Uri("http://svn.apache.org/repos/asf/subversion/"));

                int n = 0;
                bool foundTrunk = false;
                List<Uri> uris = new List<Uri>();
                SvnNodeKind dir;
                rc.GetNodeKind("", out dir);
                Assert.That(dir, Is.EqualTo(SvnNodeKind.Directory));
                rc.List("",
                    delegate(object sender, SvnRemoteListEventArgs e)
                    {
                        n++;
                        if (e.Name == "trunk")
                        {
                            foundTrunk = true;
                            Assert.That(e.Uri, Is.EqualTo(new Uri("http://svn.apache.org/repos/asf/subversion/trunk/")));
                        }
                        uris.Add(e.Uri);
                    });

                Assert.That(foundTrunk);
                Assert.That(n, Is.GreaterThan(4));

                Uri reposRoot;
                rc.GetRepositoryRoot(out reposRoot);
                rc.Reparent(reposRoot);

                int n2 = 0;
                rc.List("subversion/",
                    delegate(object sender, SvnRemoteListEventArgs e)
                    {
                        n2++;
                        Assert.That(uris.Contains(e.Uri), "Same Uri");
                    });

                Assert.That(n2, Is.EqualTo(n));
            }
            DateTime between = DateTime.Now;
            Collection<SvnListEventArgs> items;
            Client.GetList(new Uri("http://svn.apache.org/repos/asf/subversion/"), out items);
            DateTime after = DateTime.Now;

            Console.WriteLine(between - start);
            Console.WriteLine(after - between);
        }

        [TestMethod]
        public void Remote_Lock()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);
            Uri toLock = new Uri(reposUri, "trunk/index.html");
            Client.RemoteLock(toLock, "");

            using (SvnRemoteSession rc = new SvnRemoteSession())
            {
                bool locked = false;
                rc.Open(reposUri);
                rc.ListLocks("",
                    delegate(object sender, SvnRemoteListLockEventArgs e)
                    {
                        Assert.That(e.Lock, Is.Not.Null);
                        Assert.That(e.Path, Is.EqualTo("trunk/index.html"));
                        Assert.That(e.RepositoryPath, Is.EqualTo("trunk/index.html"));
                        Assert.That(e.Uri, Is.EqualTo(toLock));
                        locked = true;
                    });

                Assert.That(locked, Is.True);
            }

            using (SvnRemoteSession rc = new SvnRemoteSession())
            {
                bool locked = false;
                rc.Open(new Uri(reposUri, "trunk"));
                rc.ListLocks("",
                    delegate(object sender, SvnRemoteListLockEventArgs e)
                    {
                        Assert.That(e.Lock, Is.Not.Null);
                        Assert.That(e.Path, Is.EqualTo("index.html"));
                        Assert.That(e.RepositoryPath, Is.EqualTo("trunk/index.html"));
                        Assert.That(e.Uri, Is.EqualTo(toLock));
                        locked = true;
                    });

                Assert.That(locked, Is.True);
            }
        }


        [TestMethod]
        public void Remote_LocationSegments()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            SvnRemoteSession rs;
            using (SvnRemoteSession rc = new SvnRemoteSession())
            {
                Uri root = sbox.CreateRepository(SandBoxRepository.MergeScenario);
                rc.Open(root);
                int n = 0;
                rc.LocationSegments("branches/b/about/index.html",
                    delegate(object sender, SvnRemoteLocationSegmentEventArgs e)
                    {
                        switch (n++)
                        {
                            case 0:
                                Assert.That(e.RepositoryPath, Is.EqualTo("branches/b/about/index.html"));
                                Assert.That(e.Uri, Is.EqualTo(new Uri(root, e.RepositoryPath)));
                                break;
                            case 1:
                                Assert.That(e.RepositoryPath, Is.EqualTo("trunk/about/index.html"));
                                break;
                            default:
                                Assert.That(false);
                                break;
                        }
                    });

                Assert.That(n, Is.EqualTo(2));
                rs = rc;

                long r;
                Assert.That(rc.GetDeletedRevision("trunk", 2, out r));
                Assert.That(r, Is.EqualTo(-1));
            }

            Assert.That(!rs.IsCommandRunning);
            Assert.That(rs.IsDisposed);
        }

        [TestMethod]
        public void Remote_StatFile()
        {
            using (SvnRemoteSession rc = new SvnRemoteSession())
            {
                rc.Open(new Uri("http://svn.apache.org/repos/asf/subversion/trunk/COMMITTERS"));

                SvnRemoteStatEventArgs st;
                rc.GetStat("", out st);

                Assert.That(st, Is.Not.Null);
                Assert.That(st.Entry.Time, Is.GreaterThan(new DateTime(2006, 1, 1)));

                SvnNodeKind kind;
                rc.GetNodeKind("", out kind);
                Assert.That(kind, Is.EqualTo(SvnNodeKind.File));

                rc.GetNodeKind("QQQ", out kind);

                Assert.That(kind, Is.EqualTo(SvnNodeKind.None));
            }
        }

        [TestMethod]
        public void Remote_LogRanges()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            using (SvnRemoteSession rc = new SvnRemoteSession())
            using (SvnRemoteSession rc2 = new SvnRemoteSession())
            {
                Uri reposRoot;
                rc.Open(CollabReposUri);
                rc.GetRepositoryRoot(out reposRoot);
                rc2.Open(reposRoot);

                rc.LocationSegments("branches/c/products/medium.html",
                    delegate(object sender, SvnRemoteLocationSegmentEventArgs e)
                    {
                        rc2.Log(rc2.MakeRelativePath(e.Uri), e.Range.Reverse(),
                            delegate(object sender2, SvnRemoteLogEventArgs e2)
                            {
                                Debug.WriteLine(string.Format("{0} in {1} as {2} ({3}): {4}", e2.Author, e2.Revision, e.RepositoryPath, e.Range, e2.LogMessage));
                            });
                    });
            }
        }

        [TestMethod]
        public void Remote_CompareRanges()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            using (SvnRemoteSession rc = new SvnRemoteSession())
            {
                rc.Open(CollabReposUri);
                Collection<SvnRemoteLocationSegmentEventArgs> c;
                rc.GetLocationSegments("branches/c/products/medium.html", out c);
                List<long> revs = new List<long>();

                foreach(SvnRemoteLocationSegmentEventArgs e in c)
                {
                    revs.Add(e.StartRevision);
                    revs.Add(e.EndRevision);
                    System.Diagnostics.Debug.WriteLine(string.Format("{0}: {1}", e.Uri, e.Range));
                }

                SvnRevisionLocationMap revMap;
                rc.GetLocations("branches/c/products/medium.html", revs, out revMap);

                foreach (SvnRemoteLocationSegmentEventArgs e in c)
                {
                    Assert.That(revMap.Contains(e.StartRevision));
                    Assert.That(revMap[e.StartRevision].Uri, Is.EqualTo(e.Uri));
                    Assert.That(revMap.Contains(e.EndRevision));
                    Assert.That(revMap[e.EndRevision].Uri, Is.EqualTo(e.Uri));
                }
            }
        }

        [TestMethod]
        public void Remote_GetProps()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            using (SvnRemoteSession rc = new SvnRemoteSession(CollabReposUri))
            {
                SvnPropertyCollection pc;
                rc.GetProperties("trunk", out pc);
                Assert.That(pc, Is.Not.Null);
                Assert.That(pc.Count, Is.EqualTo(1));
                Assert.That(pc.Contains(SvnPropertyNames.SvnMergeInfo));

                rc.GetProperties("trunk/about/index.html", out pc);
                Assert.That(pc, Is.Not.Null);
                Assert.That(pc.Count, Is.EqualTo(1));
                Assert.That(pc.Contains(SvnPropertyNames.SvnEolStyle));
            }
        }
    }
}
