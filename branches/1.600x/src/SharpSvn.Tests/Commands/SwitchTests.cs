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
	///<summary>
	///Test Client::Switc
	///</summary>
	[TestFixture]
	public class SwitchTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.path = Path.GetTempPath();
		}

		[Test]
		public void PegTests()
		{
			string dir = GetTempDir();

			SvnUpdateResult result;
			Assert.That(Client.CheckOut(new SvnUriTarget(new Uri(CollabReposUri, "trunk")), dir, out result));

			long head = result.Revision;

			Assert.That(Client.Switch(dir, new SvnUriTarget(new Uri(CollabReposUri, "branches/a")), out result));
			Assert.That(result.Revision, Is.EqualTo(head));

			Assert.That(Client.Switch(dir, new SvnUriTarget(new Uri(CollabReposUri, "branches/c"), head-3), out result));
			Assert.That(result.Revision, Is.EqualTo(head - 3));

			SvnSwitchArgs sa = new SvnSwitchArgs();
			sa.Revision = head - 4;
			Assert.That(Client.Switch(dir, new SvnUriTarget(new Uri(CollabReposUri, "branches/b"), head - 5), sa, out result));
			Assert.That(result.Revision, Is.EqualTo(head - 4));

			sa = new SvnSwitchArgs();
			sa.Revision = head - 7;
			Assert.That(Client.Switch(dir, new SvnUriTarget(new Uri(CollabReposUri, "branches/a")), sa, out result));
			Assert.That(result.Revision, Is.EqualTo(head - 7));
		}

		/// <summary>
		/// Try to switch wc to repos/doc
		/// </summary>
		[Test]
		public void TestSwitchUrl()
		{
			Uri switchUrl = new Uri(this.ReposUrl, "doc");
			string checkFile = Path.Combine(this.WcPath, "text_r5.txt");

			this.Client.Switch(this.WcPath, switchUrl);
			Assert.That(File.Exists(checkFile), "Didn't switch to repos/doc");

		}
		private string path;
	}
}