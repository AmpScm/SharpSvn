using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.RemoteTests
{
    [TestFixture]
    public class RemoteLogTests : TestBase
    {
        [Test]
        public void TestOptions()
        {
            Assert.That(SvnRemoteSession.IsConnectionlessRepository(new Uri("http://svn.collab.net/")));
            Assert.That(SvnRemoteSession.RequiresExternalAuthorization(new Uri("http://svn.collab.net/")), Is.False);
        }

        [Test]
        public void OpenRepos()
        {
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
                r.List("", rev,
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

    }
}
