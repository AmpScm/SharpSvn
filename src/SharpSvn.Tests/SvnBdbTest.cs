//
// Copyright 2007-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn.Implementation;
using System.Reflection;

namespace SharpSvn.Tests
{
    [TestClass]
    public class SvnBdbTest : Commands.TestBase
    {

        [TestMethod]
        public void CreateBdbRepos()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            string path = sbox.GetTempDir();

            SvnRepositoryClient reposClient = new SvnRepositoryClient();

            SvnCreateRepositoryArgs ra = new SvnCreateRepositoryArgs();
            ra.RepositoryType = SvnRepositoryFileSystem.BerkeleyDB;
            reposClient.CreateRepository(path, ra);

            Assert.That(File.Exists(Path.Combine(path, "db/DB_CONFIG")));

            reposClient.HotCopy(path, sbox.GetTempDir());

            reposClient.SetRevisionProperty(path, 0, SvnPropertyNames.SvnLog, "Hahaha");
        }

        [TestMethod]
        public void TestAllProps()
        {
            Assert.That(SvnPropertyNames.AllSvnRevisionProperties.Contains(SvnPropertyNames.SvnAuthor));
            Assert.That(SvnPropertyNames.TortoiseSvnDirectoryProperties.Contains(SvnPropertyNames.TortoiseSvnLogMinSize));
        }
    }
}
