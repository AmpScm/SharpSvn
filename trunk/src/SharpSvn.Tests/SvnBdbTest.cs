// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.IO;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class SvnBdbTest : SvnTestBase
    {

        [Test]
        public void CreateBdbRepos()
        {
            string path = Path.Combine(TestPath, "BdbRepos");

            if(Directory.Exists(path))
                ForcedDeleteDirectory(path);

            SvnRepositoryClient reposClient = new SvnRepositoryClient();

            SvnCreateRepositoryArgs ra = new SvnCreateRepositoryArgs();
            ra.RepositoryType = SvnRepositoryFilesystem.BerkeleyDB;
            reposClient.CreateRepository(path, ra);

            Assert.That(File.Exists(Path.Combine(path, "db/DB_CONFIG")));
            Assert.That(File.Exists(Path.Combine(path, "db/DB_CONFIG")));
        }

    }
}
