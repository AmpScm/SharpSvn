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
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the Client.Commit method.
	/// </summary>
	[TestFixture]
	public class CommitTests : TestBase
	{
		/// <summary>
		/// Modifies a file in the working copy and commits the containing directory.
		/// </summary>
		[Test]
		public void TestBasicCommit()
		{
			string filepath = Path.Combine(this.WcPath, "Form.cs");
			using (StreamWriter w = new StreamWriter(filepath))
				w.Write("Moo");

			SvnCommitResult info;

            

			Assert.That(this.Client.Commit(this.WcPath, out info));

			Assert.That(info, Is.Not.Null);

			char status = this.GetSvnStatus(filepath);
			Assert.That(status, Is.EqualTo((char)0), "File not committed");
		}

		/// <summary>
		/// Locks and modifies a file in the working copy and commits the containing directory keeping, and not keeping locks
		/// </summary>
		[Test]
		public void TestCommitWithLocks()
		{
			string filepath = Path.Combine(this.WcPath, "Form.cs");
			using (StreamWriter w = new StreamWriter(filepath))
				w.Write("Moo");

			this.RunCommand("svn", "lock " + filepath);

			SvnCommitResult info;

			SvnCommitArgs a = new SvnCommitArgs();
			a.KeepLocks = true;

			Assert.That(this.Client.Commit(this.WcPath, a, out info));
			Assert.IsNotNull(info);

			char locked = this.RunCommand("svn", "status " + filepath)[5];
			Assert.That(locked == 'K', "File was unlocked while lock should be kept");

			using (StreamWriter w = new StreamWriter(filepath))
				w.Write("Bah");

			a.KeepLocks = false;
			Assert.That(Client.Commit(WcPath, a, out info));
			Assert.IsNotNull(info);

			string output = this.RunCommand("svn", "status " + filepath);
			Assert.That(output, Is.EqualTo(String.Empty));
		}

        [Test]
        public void TestSetCustomProps()
        {
            string fp = Path.Combine(WcPath, "NewFile.cs");
            Touch2(fp);
            Client.Add(fp);

            SvnCommitArgs ca = new SvnCommitArgs();
            ca.LogMessage = "Committed extra";
            ca.LogProperties.Add("my:prop", "PropValue");

            SvnCommitResult cr;
            Client.Commit(WcPath, ca, out cr);

            string value;
            Client.GetRevisionProperty(ReposUrl, cr.Revision, "my:prop", out value);

            Assert.That(value, Is.EqualTo("PropValue"));


            SvnLogArgs la = new SvnLogArgs();
            la.RetrieveProperties.Add("my:prop");
            la.Start = la.End = cr.Revision;

            Collection<SvnLogEventArgs> lc;
            Client.GetLog(WcPath, la, out lc);

            Assert.That(lc.Count, Is.EqualTo(1));
            Assert.That(lc[0].RevisionProperties.Contains("my:prop"));
            SvnLogEventArgs l = lc[0];
            Assert.That(l.CustomProperties["my:prop"].StringValue, Is.EqualTo("PropValue"));
            Assert.That(l.Author, Is.EqualTo(Environment.UserName));
        }

		/// <summary>
		/// Commits a single file
		/// </summary>
		[Test]
		public void TestCommitFile()
		{
			string filepath = Path.Combine(this.WcPath, "Form.cs");
			using (StreamWriter w = new StreamWriter(filepath))
				w.Write("Moo");

			SvnCommitArgs a = new SvnCommitArgs();
			a.Depth = SvnDepth.Empty;

			Assert.That(this.Client.Commit(filepath, a));

			char status = this.GetSvnStatus(filepath);
			Assert.That(status, Is.EqualTo((char)0), "File not committed");

		}

		/*[Test]
		public void TestCommitWithLogMessage()
		{
			this.filepath = Path.Combine(this.WcPath, "Form.cs");
			using (StreamWriter w = new StreamWriter(filepath))
				w.Write("Moo");

			AuthenticationBaton baton = new AuthenticationBaton();
			this.Client.AuthBaton.Add(AuthenticationProvider.GetUsernameProvider());
			this.Client.AuthBaton.SetParameter(
				AuthenticationBaton.ParamDefaultUsername, Environment.UserName);


			this.Client.LogMessage += new LogMessageDelegate(this.LogMessageCallback);

			this.logMessage = "Moo ";
			CommitInfo info = this.Client.Commit(new string[] { this.WcPath }, Recurse.Full);

			Assert.That( info.Author, Is.EqualTo(Environment.UserName), "Wrong username");
			string output = this.RunCommand("svn", "log " + this.filepath + " -r HEAD");

			Assert.That(output.IndexOf(this.logMessage) >= 0,
				"Log message not set");

		}*/

		[Test]
		public void TestCommitWithNonAnsiCharsInLogMessage()
		{
			this.filepath = Path.Combine(this.WcPath, "Form.cs");
			using (StreamWriter w = new StreamWriter(filepath))
				w.Write("Moo");

			SvnCommitArgs a = new SvnCommitArgs();
			a.LogMessage = " ¥ · £ · € · $ · ¢ · ₡ · ₢ · ₣ · ₤ · ₥ · ₦ · ₧ · ₨ · ₩ · ₪ · ₫ · ₭ · ₮ · ₯";

			Assert.That(Client.Commit(this.WcPath, a));

			SvnLogArgs la = new SvnLogArgs();
			la.Start = SvnRevision.Head;
			la.End = SvnRevision.Head;

			Collection<SvnLogEventArgs> logList;

			Client.GetLog(WcPath, la, out logList);

			Assert.That(logList, Is.Not.Null);
			Assert.That(logList.Count, Is.EqualTo(1));
			Assert.That(logList[0].LogMessage, Is.EqualTo(a.LogMessage));
		}

		private ArrayList logMessages = new ArrayList();
		private void GetLogMessage(object sender, SvnCommittingEventArgs e)
		{
			logMessages.Add(e.LogMessage);
		}

		/// <summary>
		/// Tests that a commit on an unmodified repos returns CommitInfo.Invalid.
		/// </summary>
		[Test]
		public void TestCommitWithNoModifications()
		{
			this.Client.Committing += new EventHandler<SvnCommittingEventArgs>(this.LogMessageCallback);
			SvnCommitResult ci;

			Assert.That(this.Client.Commit(WcPath, out ci), Is.True);
			Assert.That(ci, Is.Null);
		}

		/// <summary>
		/// Tests that you can cancel a commit.
		/// </summary>
		[Test]
		public void TestCancelledCommit()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");
			using (StreamWriter w = new StreamWriter(path))
				w.Write("MOO");
			this.Client.Committing += new EventHandler<SvnCommittingEventArgs>(this.CancelLogMessage);

			SvnCommitResult info;
			SvnCommitArgs a = new SvnCommitArgs();
			a.ThrowOnCancel = false;

			Assert.That(this.Client.Commit(path, a, out info), Is.False);

			Assert.That(info, Is.Null, "info should be Invalid for a cancelled commit");

			string output = this.RunCommand("svn", "st " + this.WcPath).Trim();
			Assert.That(output[0], Is.EqualTo('M'), "File committed even for a cancelled log message");
		}

        void VerifyNotify(object sender, SvnNotifyEventArgs e)
        {
            Assert.That(File.Exists(e.FullPath), "{0} does exist; path was defined as {1}", e.FullPath, e.Path);
        }

		[Test]
		[ExpectedException(typeof(SvnWorkingCopyLockException))]
		public void TestLockedWc()
		{
			string lockPath = Path.Combine(
				Path.Combine(this.WcPath, SvnClient.AdministrativeDirectoryName), "lock");
			using (File.CreateText(lockPath))
			{
				SvnCommitArgs a = new SvnCommitArgs();
				a.Depth = SvnDepth.Empty;
				this.Client.Commit(this.WcPath, a);
			}
		}

        [Test]
        public void NonRecursiveDirDelete()
        {
            string dir = GetTempDir();
            Client.CheckOut(GetReposUri(TestReposType.Empty), dir);

            string name = Path.Combine(dir, "sd");

            Client.CreateDirectory(name);
            Client.Commit(name);

            Client.Delete(name);
            SvnCommitArgs ca = new SvnCommitArgs();
            ca.Depth = SvnDepth.Empty;
            Client.Commit(name, ca);
        }

		private void LogMessageCallback(object sender, SvnCommittingEventArgs e)
		{
			Assert.That(e.Items.Count, Is.EqualTo(1), "Wrong number of commit items");
			Assert.That(e.Items[0].Path.IndexOf(this.filepath) >= 0,
				"Wrong path");
			Assert.That(e.Items[0].NodeKind, Is.EqualTo(SvnNodeKind.File), "Wrong kind");
			Assert.That(e.Items[0].Revision, Is.EqualTo(6), "Wrong revision");
		}

		private void CancelLogMessage(object sender, SvnCommittingEventArgs e)
		{
			e.LogMessage = null;
			e.Cancel = true;
		}


		private string filepath;
	}
}
