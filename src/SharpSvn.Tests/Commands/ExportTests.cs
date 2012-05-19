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
	/// Tests for the Client::Export method
	/// </summary>
	[TestFixture]
	public class ExportTests : TestBase
	{
		/// <summary>
		/// Test export operation from a repository
		/// </summary>
		[Test]
		public void TestExportRepos()
		{
            string wc = Path.Combine(GetTempDir(), "wc");
			this.Client.Export(new SvnUriTarget(ReposUrl, SvnRevision.Head), wc);

			Assert.That(File.Exists(Path.Combine(wc, "Form.cs")),
				"Exported file not there");
			Assert.That(!Directory.Exists(Path.Combine(wc, SvnClient.AdministrativeDirectoryName)),
				".svn directory found");
		}
		/// <summary>
		/// Test export operation from a working copy
		/// </summary>
		[Test]
		public void TestExportWc()
		{
            string wc = Path.Combine(GetTempDir(), "wc");
			this.Client.Export(WcPath, wc);

			Assert.That(File.Exists(Path.Combine(wc, "Form.cs")),
				"Exported file not there");
			Assert.That(!Directory.Exists(Path.Combine(wc, SvnClient.AdministrativeDirectoryName)),
				".svn directory found");
		}

		[Test]
		public void TestExportNonRecursive()
		{
            string wc = Path.Combine(GetTempDir(), "wc");
			SvnExportArgs a = new SvnExportArgs();
			a.Depth = SvnDepth.Empty;
			SvnUpdateResult r;
			this.Client.Export(WcPath, wc, a, out r);
			Assert.That(Directory.GetDirectories(wc).Length, Is.EqualTo(0));
		}
	}
}
