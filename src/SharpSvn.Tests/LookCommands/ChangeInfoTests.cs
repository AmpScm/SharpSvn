using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.LookCommands
{
    [TestFixture]
    public class ChangeInfoTests : TestBase
    {
        [Test]
        public void GetInfoHead()
        {
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoEventArgs r;
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();

                ia.RetrieveChangedPaths = false; // Will fail if true

                Assert.That(cl.GetChangeInfo(GetRepos(TestReposType.CollabRepos), ia, out r));

                Assert.That(r, Is.Not.Null);
                Assert.That(r.Author, Is.EqualTo("merger"));
                Assert.That(r.Revision, Is.EqualTo(17L));
                Assert.That(r.LogMessage, Is.EqualTo("Merged branch c back to trunk"));

                // This is the exact time of the commit in microseconds
                DateTime shouldBe = new DateTime(2008, 1, 27, 15, 7, 57, 9, DateTimeKind.Utc).AddTicks(4750);

                Assert.That(r.Time, Is.EqualTo(shouldBe));
            }
        }

        [Test]
        public void GetInfo12()
        {
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoEventArgs r;
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();
                ia.Revision = 12;

                ia.RetrieveChangedPaths = false; // Will fail if true

                Assert.That(cl.GetChangeInfo(GetRepos(TestReposType.CollabRepos), ia, out r));

                Assert.That(r, Is.Not.Null);
                Assert.That(r.Author, Is.EqualTo("merger"));
                Assert.That(r.Revision, Is.EqualTo(12L));
                Assert.That(r.LogMessage, Is.EqualTo("Merge branch a - product roadmap"));

                // This is the exact time of the commit in microseconds
                DateTime shouldBe = new DateTime(2008, 1, 27, 15, 2, 26, 567, DateTimeKind.Utc).AddTicks(6830);

                Assert.That(r.Time, Is.EqualTo(shouldBe));
            }
        }
    }
}
