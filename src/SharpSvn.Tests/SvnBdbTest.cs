// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using NUnit.Framework;
using SharpSvn.Implementation;
using NUnit.Framework.SyntaxHelpers;

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
            ra.RepositoryType = SvnRepositoryFileSystem.BerkeleyDB;
            reposClient.CreateRepository(path, ra);

            Assert.That(File.Exists(Path.Combine(path, "db/DB_CONFIG")));
            Assert.That(File.Exists(Path.Combine(path, "db/DB_CONFIG")));
        }

		[Test]
		public void TestAllProps()
		{
			Assert.That(SvnPropertyNames.AllSvnRevisionProperties.Contains(SvnPropertyNames.SvnAuthor));
			Assert.That(SvnPropertyNames.TortoiseSvnDirectoryProperties.Contains(SvnPropertyNames.TortoiseSvnLogMinSize));
		}

		[Test]
		public void TestUriCanonicalization()
		{
			Assert.That(new Uri("svn://127.0.0.1:1234").ToString(), Is.EqualTo("svn://127.0.0.1:1234/"));

			Assert.That(new SvnUriTarget(new Uri("svn://127.0.0.1:1234/")).TargetName, Is.EqualTo("svn://127.0.0.1:1234"));
		}

    }
}
