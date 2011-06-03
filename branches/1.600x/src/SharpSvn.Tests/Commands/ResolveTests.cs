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
using SharpSvn.Security;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::Resolve
	/// </summary>
	[TestFixture]
	public class ResolveTests : TestBase
	{
		string _path;
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this._path = this.GetTempFile();
			UnzipToFolder(Path.Combine(ProjectBase, "Zips/conflictwc.zip"), _path);
			this.RenameAdminDirs(this._path);
		}

		[TearDown]
		public override void TearDown()
		{
			base.TearDown();
			RecursiveDelete(_path);
		}

		/// <summary>
		///Attempts to resolve a conflicted file.
		/// </summary>
		[Test]
		public void TestResolveFile()
		{
			string filePath = Path.Combine(this._path, "Form.cs");

			this.Client.Resolved(filePath);

			Assert.That(this.GetSvnStatus(filePath), Is.EqualTo('M'), "Resolve didn't work!");

		}

		/// <summary>
		///Attempts to resolve a conflicted directory recursively.
		/// </summary>
		[Test]
		public void TestResolveDirectory()
		{
			SvnResolveArgs a = new SvnResolveArgs();
			a.Depth = SvnDepth.Infinity;

			this.Client.Resolve(this._path, SvnAccept.Merged, a);

			Assert.That(this.GetSvnStatus(this._path), Is.EqualTo('M'),
				" Resolve didn't work! Directory still conflicted");
			Assert.That(this.GetSvnStatus(Path.Combine(this._path, "Form.cs")), Is.EqualTo('M'),
				"Resolve didn't work! File still conflicted");
		}

		[Test]
		public void RepeatedEventHookUp_SOC_411()
		{
			Uri projectRoot = new Uri(@"http://sharpsvn.open.collab.net/svn/sharpsvn/trunk/imports");

			using (var svnClient = new SvnClient())
			{
				for (int i = 0; i < 3; i++)
				{
					string workingcopy = GetTempDir();
					Directory.CreateDirectory(workingcopy);

					svnClient.Authentication.UserNamePasswordHandlers += DoNowt;
					try
					{
						SvnUpdateResult svnUpdateResult;
						Assert.IsTrue(svnClient.CheckOut(projectRoot, workingcopy, new SvnCheckOutArgs(), out svnUpdateResult));
						Assert.IsNotNull(svnUpdateResult);
						Assert.IsTrue(svnUpdateResult.HasRevision);
					}
					finally
					{
						svnClient.Authentication.UserNamePasswordHandlers -= DoNowt;
					}
				}
			}
		}

		private void DoNowt(object sender, SvnUserNamePasswordEventArgs e)
		{
			Assert.Fail("Didn't expect to be called here, make sure your credentials are in the cache");
		}

	}

}
