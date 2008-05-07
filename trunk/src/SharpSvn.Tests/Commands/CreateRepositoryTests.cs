using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class CreateRepositoryTests : TestBase
    {
        [Test]
        public void CreateFsFsRepository()
        {
            using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
            {
                SvnCreateRepositoryArgs cra = new SvnCreateRepositoryArgs();
                cra.RepositoryCompatibility = SvnRepositoryCompatibility.Default;
                reposClient.CreateRepository(GetTempDir(), cra);
            }
        }

        [Test]
        public void CreateBdbRepository()
        {
            using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
            {
                SvnCreateRepositoryArgs cra = new SvnCreateRepositoryArgs();
                cra.RepositoryType = SvnRepositoryFileSystem.BerkeleyDB;
                cra.RepositoryCompatibility = SvnRepositoryCompatibility.Default;
                reposClient.CreateRepository(GetTempDir(), cra);
            }
        }
    }
}
