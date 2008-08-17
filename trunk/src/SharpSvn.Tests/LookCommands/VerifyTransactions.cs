using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using System.ComponentModel;
using System.Threading;
using System.CodeDom.Compiler;
using System.IO;

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

                GC.KeepAlive(i);
            }
        }

        [Test]
        public void TestPreCommit()
        {
            Uri uri = GetReposUri(TestReposType.CollabRepos);

            using (InstallHook(uri, SvnHookType.PreCommit, OnPreCommit))
            {
                string dir = GetTempDir();
                Client.CheckOut(new Uri(uri, "trunk/"), dir);

                TouchFile(Path.Combine(dir, "1.txt"));
                Client.Add(Path.Combine(dir, "1.txt"));
                SvnCommitArgs ca = new SvnCommitArgs();
                ca.LogMessage = "Blaat!\nQWQQ\n";
                Client.Commit(dir, ca);
            }            
        }
    }
}
