using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpSvn.Tests.Commands;
using System.Collections.ObjectModel;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.LookCommands
{
    [TestFixture]
    public class GetChangesTests : HookTestBase
    {

        [Test]
        public void ChangedDirs()
        {
            string dir = CreateRepos(TestReposType.Empty);
            Uri uri = PathToUri(dir);
            using (InstallHook(uri, SvnHookType.PreCommit, OnChangedDirs))
            {
                using (SvnClient cl = new SvnClient())
                {
                    SvnCreateDirectoryArgs da = new SvnCreateDirectoryArgs();
                    da.CreateParents = true;
                    da.LogMessage = "Created!";
                    cl.RemoteCreateDirectories(
                        new Uri[]
                        {
                        new Uri(uri, "a/b/c/d/e/f"),
                        new Uri(uri, "a/b/c/d/g/h"),
                        new Uri(uri, "i/j/k"),
                        new Uri(uri, "l/m/n"),
                        new Uri(uri, "l/m/n/o/p")
                        }, da);
                }
            }
        }

        private void OnChangedDirs(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient lc = new SvnLookClient())
            {
                SvnChangedArgs ca = new SvnChangedArgs();
                ca.Transaction = e.HookArgs.TransactionName;

                Collection<SvnChangedEventArgs> list;
                Assert.That(lc.GetChanged(e.HookArgs.RepositoryPath, ca, out list));

                Assert.That(list.Count, Is.EqualTo(17)); // 16 + root directory
                Assert.That(list[0].Name, Is.EqualTo(""));
                Assert.That(list[0].Path, Is.EqualTo("/"));
                Assert.That(list[1].Name, Is.EqualTo("a"));
                Assert.That(list[1].Path, Is.EqualTo("/a/"));
                Assert.That(list[2].Name, Is.EqualTo("b"));
                Assert.That(list[2].Path, Is.EqualTo("/a/b/"));
                Assert.That(list[3].Name, Is.EqualTo("c"));
                Assert.That(list[3].Path, Is.EqualTo("/a/b/c/"));
            }
        }
    }
}
