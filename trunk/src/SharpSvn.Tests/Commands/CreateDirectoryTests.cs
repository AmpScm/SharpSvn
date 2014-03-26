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
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests the NSvn.Client.MakeDir method
    /// </summary>
    [TestClass]
    public class CreateDirectoryTests : TestBase
    {
        /// <summary>
        /// Tests creating a directory in the working copy
        /// </summary>
        [TestMethod]
        public void CreateDirectory_MakeLocalDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri emptyUri = sbox.CreateRepository(SandBoxRepository.Empty);

            Client.CheckOut(emptyUri, sbox.Wc);

            string path = Path.Combine(sbox.Wc, "foo");
            Assert.That(Client.CreateDirectory(path));

            Assert.That(this.GetSvnStatus(path), Is.EqualTo(SvnStatus.Added), "Wrong status code");
        }

        /// <summary>
        /// Tests creating a directory in the repository
        /// </summary>
        [TestMethod]
        public void CreateDirectory_MakeRepositoryDir()
        {
            Uri url = new Uri(ReposUrl, "mooNewDirectory/");

            Assert.That(Client.RemoteCreateDirectory(url));
            bool gotOne = false;
            Client.List(url, delegate(object sender, SvnListEventArgs e) { gotOne = true; });

            Assert.That(gotOne);
        }

        [TestMethod]
        public void CreateDirectory_CreateTrunk()
        {
            using (SvnClient client = NewSvnClient(true, false))
            {
                Uri trunkUri = new Uri(ReposUrl, "trunk/");
                client.RemoteCreateDirectory(trunkUri);

                string trunkPath = GetTempDir();

                client.CheckOut(trunkUri, trunkPath);

                TouchFile(Path.Combine(trunkPath, "test.txt"));

                Assert.That(SvnTools.IsManagedPath(trunkPath));
                Assert.That(SvnTools.IsBelowManagedPath(trunkPath));
                Assert.That(SvnTools.IsBelowManagedPath(Path.Combine(trunkPath, "q/r/s/t/u/v/test.txt")));

                client.Add(Path.Combine(trunkPath, "test.txt"));

                Directory.CreateDirectory(Path.Combine(trunkPath, "dir"));
                TouchFile(Path.Combine(trunkPath, "dir/test.txt"));

                SvnAddArgs aa = new SvnAddArgs();
                aa.AddParents = true;
                client.Add(Path.Combine(trunkPath, "dir/test.txt"), aa);

                client.Commit(trunkPath);

                client.RemoteDelete(trunkUri, new SvnDeleteArgs());
            }
        }
    }
}
