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
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the NSvn.Client.MoveFile method
	/// </summary>
	[TestFixture]
	public class CopyTests : TestBase
	{
		/// <summary>
		/// Tests copying a file in WC -> WC
		/// </summary>
		[Test]
		public void TestCopyWCWCFile()
		{
			string srcPath = Path.Combine(this.WcPath, "Form.cs");
			string dstPath = Path.Combine(this.WcPath, "renamedForm.cs");

			Assert.That(Client.Copy(new SvnPathTarget(srcPath, SvnRevision.Head), dstPath));

			Assert.That(File.Exists(dstPath), "File wasn't copied");
			Assert.That(File.Exists(srcPath), "Source File don't exists");

            Assert.That(Client.Commit(WcPath));
		}

		/// <summary>
		/// Tests copying a directory in a WC -> WC
		/// </summary>
		[Test]
		public void TestCopyWCWCDir()
		{
			string srcPath = Path.Combine(this.WcPath, @"bin\Debug");
			string dstPath = Path.Combine(this.WcPath, @"copyDebug");

			Assert.That(this.Client.Copy(new SvnPathTarget(srcPath), dstPath));

			Assert.That(Directory.Exists(dstPath), "Directory don't exist ");
			Assert.That(this.GetSvnStatus(dstPath), Is.EqualTo('A'), " Status is not 'A'  ");

            Assert.That(Client.Commit(WcPath));
		}

		/// <summary>
		/// Tests copying a directory in a WC -> URL (repository)
		/// </summary>
		[Test]
		public void TestCopyWCReposDir()
		{
			string srcPath = Path.Combine(this.WcPath, @"bin\Debug");
			Uri dstPath = new Uri(this.ReposUrl, "copyDebug/");

			SvnCommitResult ci;

			Assert.That(Client.RemoteCopy(new SvnPathTarget(srcPath, SvnRevision.Head), dstPath, out ci));

			Assert.That(ci, Is.Not.Null);

			String cmd = this.RunCommand("svn", "list " + this.ReposUrl.ToString());
			Assert.That(cmd.IndexOf("copyDebug") >= 0, "File wasn't copied");
		}

		/// <summary>
		/// Tests copying a from a Repository to WC: URL -> WC
		/// </summary>
		[Test]
		public void TestCopyReposWCFile()
		{
			Uri srcUri = new Uri(this.ReposUrl, "Form.cs");
			string dstPath = Path.Combine(this.WcPath, "copyForm");

			this.Client.Copy(new SvnUriTarget(srcUri), dstPath);

			Assert.That(this.GetSvnStatus(dstPath), Is.EqualTo('A'), " File is not copied  ");

            Assert.That(Client.Commit(WcPath));
		}

        [Test]
        public void CopyReposToWcWithParents()
        {
            Uri srcUri = new Uri(this.ReposUrl, "Form.cs");
            SvnCopyArgs ca = new SvnCopyArgs();
            ca.CreateParents = true;
            Client.Copy(srcUri, Path.Combine(WcPath, "dir/sub/with/more/levels"), ca);
        }

		/// <summary>
		/// Tests copying a file within a Repos: URL -> URL
		/// </summary>
		[Test]
		public void TestCopyReposReposFile()
		{
			Uri srcUri = new Uri(this.ReposUrl, "Form.cs");
			Uri dstUri = new Uri(this.ReposUrl, "copyForm");

			SvnCommitResult ci;

			Assert.That(Client.RemoteCopy(srcUri, dstUri, out ci));
			Assert.That(ci, Is.Not.Null);

			String cmd = this.RunCommand("svn", "list " + this.ReposUrl);
			Assert.That(cmd.IndexOf("Form.cs") >= 0, "File wasn't copied");
			Assert.That(cmd.IndexOf("copyForm") >= 0, "Copied file doesn't exist");
		}

        [Test]
        public void TestSimpleBranch0()
        {
            Uri repos = GetReposUri(TestReposType.CollabRepos);
            Uri trunk = new Uri(repos, "trunk/");

            SvnCopyArgs ca = new SvnCopyArgs();
            ca.CreateParents = true;

            // Subversion 1.5.4 throws an AccessViolationException here
            Client.RemoteCopy(trunk, new Uri(repos, "branch/"), ca);
        }

        [Test, ExpectedException(typeof(SvnFileSystemException), ExpectedMessage="already exists", MatchType=MessageMatch.Contains)]
        public void TestSimpleBranch1()
        {
            Uri repos = GetReposUri(TestReposType.CollabRepos);
            Uri trunk = new Uri(repos, "trunk/");

            Client.RemoteCopy(trunk, new Uri(repos, "branch/"));

            SvnCopyArgs ca = new SvnCopyArgs();
            ca.CreateParents = true;

            // Subversion 1.5.4 throws an AccessViolationException here
            Client.RemoteCopy(trunk, new Uri(repos, "branch/"), ca);
        }

        [Test]
        public void ReposReposCopy()
        {
            Uri trunk = new Uri(CollabReposUri, "trunk/");
            Uri branch = new Uri(CollabReposUri, "branches/new-branch");

            SvnCopyArgs ca = new SvnCopyArgs();
            ca.LogMessage = "Message";
            ca.CreateParents = true;
            Client.RemoteCopy(trunk, branch, ca);

            int n = 0;
            Client.List(branch, delegate(object sender, SvnListEventArgs e)
            {
                if(e.Entry.NodeKind == SvnNodeKind.File)
                    n++;
            });

            Assert.That(n, Is.GreaterThan(0), "Copied files");
        }
	}
}
