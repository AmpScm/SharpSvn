﻿//
// Copyright 2008-2025 The SharpSvn Project
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
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class CreateRepositoryTests : TestBase
    {
        [TestMethod]
        public void CreateRepository_FsFs()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
            {
                SvnCreateRepositoryArgs cra = new SvnCreateRepositoryArgs();
                cra.RepositoryCompatibility = SvnRepositoryCompatibility.Default;
                reposClient.CreateRepository(sbox.GetTempDir(), cra);
            }
        }

        [TestMethod]
        public void CreateRepository_Bdb()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            using (SvnRepositoryClient reposClient = new SvnRepositoryClient())
            {
                SvnCreateRepositoryArgs cra = new SvnCreateRepositoryArgs();
                cra.RepositoryType = SvnRepositoryFileSystem.BerkeleyDB;
                cra.RepositoryCompatibility = SvnRepositoryCompatibility.Default;
                reposClient.CreateRepository(sbox.GetTempDir(), cra);
            }
        }
    }
}
