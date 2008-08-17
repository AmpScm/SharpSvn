using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using System.ComponentModel;
using System.Threading;
using System.CodeDom.Compiler;
using System.IO;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.LookCommands
{
    [TestFixture]
    public class VerifyTransactions : HookTestBase
    {
        void OnPreCommit(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();
                ia.Transaction = e.HookArgs.TransactionName;

                SvnChangeInfoEventArgs i;
                Assert.That(cl.GetChangeInfo(e.HookArgs.RepositoryPath, ia, out i));

                Assert.That(i.Revision, Is.LessThan(0L));
                Assert.That(i.Author, Is.EqualTo(Environment.UserName));
            }
        }

        [Test]
        public void TestPreCommit()
        {
            Uri uri = PathToUri(CreateRepos(TestReposType.CollabRepos));

            using (InstallHook(uri, SvnHookType.PreCommit, OnPreCommit))
            {
                string dir = GetTempDir();
                Client.CheckOut(new Uri(uri, "trunk/"), dir);

                TouchFile(Path.Combine(dir, "Pre.txt"));
                Client.Add(Path.Combine(dir, "Pre.txt"));
                SvnCommitArgs ca = new SvnCommitArgs();
                ca.LogMessage = "Blaat!\nQWQQ\n";
                Client.Commit(dir, ca);
            }            
        }
        void OnPostCommit(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();
                ia.Revision = e.HookArgs.Revision;

                SvnChangeInfoEventArgs i;
                Assert.That(cl.GetChangeInfo(e.HookArgs.RepositoryPath, ia, out i));

                GC.KeepAlive(i);
                Assert.That(i.Revision, Is.GreaterThanOrEqualTo(0L));
                Assert.That(i.Author, Is.EqualTo(Environment.UserName));
            }
        }

        [Test]
        public void TestPostCommit()
        {
            Uri uri = PathToUri(CreateRepos(TestReposType.CollabRepos));

            using (InstallHook(uri, SvnHookType.PostCommit, OnPostCommit))
            {
                string dir = GetTempDir();
                Client.CheckOut(new Uri(uri, "trunk/"), dir);

                TouchFile(Path.Combine(dir, "Post.txt"));
                Client.Add(Path.Combine(dir, "Post.txt"));
                SvnCommitArgs ca = new SvnCommitArgs();
                ca.LogMessage = "Blaat!\nQWQQ\n";
                Client.Commit(dir, ca);
            }
        }
    }
}
