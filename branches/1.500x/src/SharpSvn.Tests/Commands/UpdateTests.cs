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
	/// Tests Client::Update
	/// </summary>
	[TestFixture]
	public class UpdateTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

            this.wc2 = GetTempDir();

			UnzipToFolder(Path.Combine(ProjectBase, "Zips/wc.zip"), wc2);
			this.RenameAdminDirs(this.wc2);

			SvnClient cl = new SvnClient(); // Fix working copy to real location
			cl.Relocate(wc2, new Uri("file:///tmp/repos/"), ReposUrl);
		}

		public override void TearDown()
		{
			base.TearDown();
			RecursiveDelete(this.wc2);
		}

		[Test]
		public void RevTests()
		{
			string dir = GetTempDir();

			SvnUpdateResult result;
			Assert.That(Client.CheckOut(new SvnUriTarget(new Uri(CollabReposUri, "trunk")), dir, out result));

			long head = result.Revision;

			Assert.That(Client.Update(dir, out result));
			Assert.That(result.Revision, Is.EqualTo(head));

			SvnUpdateArgs ua = new SvnUpdateArgs();

			ua.Revision = head - 5;

			Assert.That(Client.Update(dir, ua, out result));
			Assert.That(result.Revision, Is.EqualTo(head-5));
		}

		/// <summary>
		/// Deletes a file, then calls update on the working copy to restore it 
		/// from the text-base
		/// </summary>
		[Test]
		public void TestDeletedFile()
		{
			string filePath = Path.Combine(this.WcPath, "Form.cs");
			File.Delete(filePath);
			this.Client.Update(this.WcPath);

			Assert.That(File.Exists(filePath), "File not restored after update");
		}

		/// <summary>
		/// Changes a file in a secondary working copy and commits. Updates the 
		/// primary wc and compares
		/// </summary>
		[Test]
		public void TestChangedFile()
		{
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "Form.cs")))
				w.Write("Moo");
			this.RunCommand("svn", "ci -m \"\" " + this.wc2);

			this.Client.Update(this.WcPath);

			string s;
			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "Form.cs")))
				s = r.ReadToEnd();

			Assert.That(s, Is.EqualTo("Moo"), "File not updated");
		}

        [Test]
        public void TestNotify()
        {
            int n = 0;;
            SvnUpdateArgs ua = new SvnUpdateArgs();
            ua.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                Assert.That(e.FullPath, Is.EqualTo(WcPath));
                switch (n++)
                {
                    case 0:
                        Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.UpdateUpdate));
                        break;
                    case 1:
                        Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.UpdateCompleted));
                        break;
                }
            };

            Client.Update(WcPath, ua);

            n = 0;

            Client.Update(new string[] { WcPath }, ua);
        }

		[Test]
		public void TestUpdateMultipleFiles()
		{
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "Form.cs")))
				w.Write("Moo");
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "AssemblyInfo.cs")))
				w.Write("Moo");
			this.RunCommand("svn", "ci -m \"\" " + this.wc2);

			SvnUpdateArgs a = new SvnUpdateArgs();
			a.Depth = SvnDepth.Empty;

			SvnUpdateResult result;

			Assert.That(Client.Update(new string[]{ 
                                                             Path.Combine( this.WcPath, "Form.cs" ),
                                                             Path.Combine( this.WcPath, "AssemblyInfo.cs" )
                                                         }, a, out result));
			Assert.That(result.ResultMap.Count, Is.EqualTo(2));

			string s;
			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "Form.cs")))
				s = r.ReadToEnd();
			Assert.That(s, Is.EqualTo("Moo"), "File not updated");

			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "AssemblyInfo.cs")))
				s = r.ReadToEnd();
			Assert.That(s, Is.EqualTo("Moo"), "File not updated");
		}

		private string wc2;
	}
}
