using System;
using System.Collections.ObjectModel;
using System.IO;
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

                mucc.CreateFile("trunk/README", new MemoryStream(Encoding.UTF8.GetBytes("Welcome to this project")));
                mucc.SetProperty("trunk/README", "svn:eol-style", "native");

                Assert.That(mucc.Commit(out cr)); // Commit r1
                Assert.That(cr, Is.Not.Null);
            }

            using (SvnClient svn = new SvnClient())
            {
                Collection<SvnListEventArgs> members;
                svn.GetList(uri, out members);

                MemoryStream ms = new MemoryStream();
                SvnPropertyCollection props;
                svn.Write(new Uri(uri, "trunk/README"), ms, out props);

                Assert.That(props, Is.Not.Empty);
                Assert.That(Encoding.UTF8.GetString(ms.ToArray()), Is.EqualTo("Welcome to this project"));
                Assert.That(props.Contains("svn:eol-style"));
            }
        }
    }
}
