using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using NUnit.Framework;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;

namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class RepositoryOperationTests : TestBase
    {
        public RepositoryOperationTests()
        { }

        [TestMethod]
        public void RepositoryOperation_OpenDirect()
        {
            Uri uri = GetReposUri(TestReposType.Empty);

            using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri))
            {
                SvnCommitResult cr;
                Assert.That(mucc.Commit(out cr));

                Assert.That(cr, Is.Null);
            }

            using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri))
            {
                SvnCommitResult cr;
                bool tick = false;

                mucc.Committing += delegate(object sender, SvnCommittingEventArgs e)
                {
                    Assert.That(e.Items, Is.Not.Empty);
                    Assert.That(e.Items[0].Uri, Is.EqualTo(new Uri(uri, "A/")));
                    Assert.That(e.Items[0].NodeKind, Is.EqualTo(SvnNodeKind.Directory));
                    Assert.That(e.LogMessage, Is.Null);

                    e.LogMessage = "New message";
                    tick = true;
                };

                mucc.CreateDirectory("A");

                Assert.That(mucc.Commit(out cr));
                Assert.That(tick, "Ticked");

                Assert.That(cr, Is.Not.Null);
                Assert.That(cr.Revision, Is.EqualTo(1));
            }
        }

        [TestMethod]
        public void RepositoryOperation_ViaSvnClient()
        {
            Uri uri = GetReposUri(TestReposType.Empty);

            using (SvnClient svn = new SvnClient())
            {
                SvnCommitResult cr;

                Assert.That(svn.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        //mucc.
                    }, out cr));

                Assert.That(cr, Is.Null);

                svn.Configuration.LogMessageRequired = false;

                Assert.That(svn.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.SetProperty("", "A", "B");

                    }, out cr));

                Assert.That(cr, Is.Not.Null);
                Assert.That(cr.Revision, Is.EqualTo(1));
            }
        }

        [TestMethod]
        public void RepositoryOperation_SetupRepository()
        {
            Uri uri = GetReposUri(TestReposType.Empty);
            SvnCommitResult cr;

            SvnRepositoryOperationArgs oa = new SvnRepositoryOperationArgs();
            oa.LogMessage = "Everything in one revision";

            using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri, oa))
            {
                mucc.CreateDirectory("trunk");
                mucc.CreateDirectory("branches");
                mucc.CreateDirectory("tags");
                mucc.CreateDirectory("trunk/src");
                mucc.SetProperty("", "svn:auto-props", "*.cs = svn:eol-style=native");
                mucc.SetProperty("", "svn:global-ignores", "bin obj");

                mucc.CreateFile("trunk/README", new MemoryStream(Encoding.UTF8.GetBytes("Welcome to this project")));
                mucc.SetProperty("trunk/README", "svn:eol-style", "native");

                Assert.That(mucc.Commit(out cr)); // Commit r1
                Assert.That(cr, Is.Not.Null);
            }

            using (SvnClient svn = new SvnClient())
            {
                Collection<SvnListEventArgs> members;
                svn.GetList(uri, out members);

                Assert.That(members, Is.Not.Empty);

                MemoryStream ms = new MemoryStream();
                SvnPropertyCollection props;
                svn.Write(new Uri(uri, "trunk/README"), ms, out props);

                Assert.That(props, Is.Not.Empty);
                Assert.That(Encoding.UTF8.GetString(ms.ToArray()), Is.EqualTo("Welcome to this project"));
                Assert.That(props.Contains("svn:eol-style"));


                Collection<SvnLogEventArgs> la;
                SvnLogArgs ll = new SvnLogArgs();
                ll.Start = 1;
                svn.GetLog(uri, ll, out la);
                Assert.That(la, Is.Not.Empty);
                Assert.That(la[0].LogMessage, Is.EqualTo("Everything in one revision"));
            }
        }
    }
}
