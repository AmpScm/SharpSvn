using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests.AdminCommands
{
    [TestClass]
    public class DumpTest : TestBase
    {

        [TestMethod]
        public void Dump_DumpDb()
        {
            string repos = GetTempDir();

            using (SvnRepositoryClient cl = new SvnRepositoryClient())
            {
                SvnCreateRepositoryArgs ra = new SvnCreateRepositoryArgs();
                ra.RepositoryType = SvnRepositoryFileSystem.BerkeleyDB;
                ra.RepositoryCompatibility = SvnRepositoryCompatibility.Subversion10;

                cl.CreateRepository(repos, ra);

                string file = GetTempFile();
                using(FileStream s = File.Create(file))
                {
                    SvnDumpRepositoryArgs da = new SvnDumpRepositoryArgs();
                    da.Start = new SvnRevision(0);
                    da.End = new SvnRevision(SvnRevisionType.Head);
                    cl.DumpRepository(repos, s, da);
                }

                Assert.That(new FileInfo(file).Length, Is.GreaterThan(12));
            }
        }
    }
}
