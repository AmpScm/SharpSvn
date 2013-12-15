using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class RepositoryOperationTests : TestBase
    {
        [Test]
        public void OpenRepositoryOperation()
        {
            Uri uri = GetReposUri(TestReposType.Empty);

            using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri))
            {
                SvnCommitResult cr;
                Assert.That(mucc.Commit(out cr));

                Assert.That(cr, Is.Null);
            }
        }

        [Test]
        public void OpenRepositoryOperationViaSvnClient()
        {
            Uri uri = GetReposUri(TestReposType.Empty);

            using (SvnClient svn = new SvnClient())
            {
                SvnCommitResult cr;

                Assert.That(svn.RepositoryOperation(uri,
                    delegate(SvnMultiCommandClient mucc)
                    {
                        mucc.
                    }, out cr));

                Assert.That(cr, Is.Null);
            }
        }
    }
}
