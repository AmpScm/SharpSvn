using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class InheritedPropertyList : TestBase
    {

        [Test]
        public void TestIprops()
        {
            Uri uri = GetReposUri(TestReposType.CollabRepos);
            string wc = GetTempDir();

            SvnSetPropertyArgs pa = new SvnSetPropertyArgs();
            pa.BaseRevision = 17;
            Client.RemoteSetProperty(uri, "root", "root", pa);
            pa.BaseRevision++;
            Client.RemoteSetProperty(new Uri(uri, "trunk"), "trunk", "trunk", pa);
            pa.BaseRevision++;
            Client.RemoteSetProperty(new Uri(uri, "trunk/jobs"), "jobs", "jobs", pa);
            pa.BaseRevision++;
            Client.RemoteSetProperty(new Uri(uri, "trunk/jobs/index.html"), "index", "index", pa);

            SvnUpdateResult ur;
            Client.CheckOut(new Uri(uri, "trunk"), wc, out ur);
            Assert.That(ur.Revision, Is.EqualTo(pa.BaseRevision + 1));

            Client.SetProperty(wc, "local-trunk", "local-trunk");
            Client.SetProperty(Path.Combine(wc, "jobs"), "local-jobs", "local-jobs");

            Collection<SvnInheritedPropertyListEventArgs> results;
            string target = Path.Combine(wc, "jobs/index.html");
            Client.GetInheritedPropertyList(Path.Combine(wc, "jobs/index.html"), out results);

            Assert.That(results, Is.Not.Null);
            Assert.That(results.Count, Is.EqualTo(4));
            Assert.That(results[0].Path, Is.EqualTo(SvnTools.GetNormalizedFullPath(target)), "Path 0");
            Assert.That(results[0].Properties, Is.Not.Empty);
            Assert.That(results[1].Path, Is.EqualTo(SvnTools.GetNormalizedFullPath(Path.Combine(target, ".."))), "Path 1");
            Assert.That(results[1].Properties, Is.Not.Empty);
            Assert.That(results[2].Path, Is.EqualTo(SvnTools.GetNormalizedFullPath(Path.Combine(target, "../.."))), "Path 2");
            Assert.That(results[2].Properties, Is.Not.Empty);
            Assert.That(results[3].Uri, Is.EqualTo(uri), "Path 3");
            Assert.That(results[3].Properties, Is.Not.Empty);
        }
    }
}
