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

// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the NSvn.Client.MakeDir method
	/// </summary>
	[TestFixture]
	public class CreateDirectoryTests : TestBase
	{
		/// <summary>
		/// Tests creating a directory in the working copy
		/// </summary>
		[Test]
		public void TestMakeLocalDir()
		{
			string path = Path.Combine(this.WcPath, "foo");
			Assert.That(Client.CreateDirectory(path));

			Assert.That(this.GetSvnStatus(path), Is.EqualTo('A'), "Wrong status code");
		}

		/// <summary>
		/// Tests creating a directory in the repository
		/// </summary>
		[Test]
		public void TestMakeRepositoryDir()
		{
			Uri url = new Uri(ReposUrl, "mooNewDirectory/");

			Assert.That(Client.RemoteCreateDirectory(url));
            bool gotOne = false;
            Client.List(url, delegate(object sender, SvnListEventArgs e) { gotOne = true; });

            Assert.That(gotOne);
        }

        [Test]
        public void CreateTrunk()
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

                TouchFile(Path.Combine(trunkPath, "dir/test.txt"), true);

                SvnAddArgs aa = new SvnAddArgs();
                aa.AddParents = true;
                client.Add(Path.Combine(trunkPath, "dir/test.txt"), aa);

                client.Commit(trunkPath);

                client.RemoteDelete(trunkUri, new SvnDeleteArgs());
            }
        }
	}
}
