// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
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
