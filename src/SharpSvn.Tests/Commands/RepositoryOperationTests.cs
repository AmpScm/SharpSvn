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
                        //mucc.
                    }, out cr));

                Assert.That(cr, Is.Null);
            }
        }

        [Test]
        public void SetupRepository()
        {
            Uri uri = GetReposUri(TestReposType.Empty);
            SvnCommitResult cr;
            using (SvnMultiCommandClient mucc = new SvnMultiCommandClient(uri))
            {
                mucc.CreateDirectory("trunk");
                mucc.CreateDirectory("branches");
                mucc.CreateDirectory("tags");
                mucc.CreateDirectory("trunk/src");
                mucc.SetProperty("", "svn:auto-props", "*.cs = svn:eol-style=native");
                mucc.SetProperty("", "svn:global-ignores", "bin obj");

                Assert.That(mucc.Commit(out cr)); // Commit r1
                Assert.That(cr, Is.Not.Null);
            }
        }
    }
}
