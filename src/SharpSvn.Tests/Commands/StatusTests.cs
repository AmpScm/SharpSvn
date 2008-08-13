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
	/// Tests Client::Status
	/// </summary>
	[TestFixture]
	public class StatusTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

			this.ExtractWorkingCopy();
			this.ExtractRepos();
		}

		/// <summary>Compares the status from Client::Status with the output from 
		/// commandline client</summary>
		[Test]
		public void TestLocalStatus()
		{
			string unversioned = this.CreateTextFile("unversioned.cs");
			string added = this.CreateTextFile("added.cs");
			this.RunCommand("svn", "add " + added);
			string changed = this.CreateTextFile("Form.cs");
			string ignored = this.CreateTextFile("foo.ignored");
			string propChange = Path.Combine(this.WcPath, "App.ico");
			this.RunCommand("svn", "ps foo bar " + propChange);
			this.RunCommand("svn", "ps svn:ignore *.ignored " + this.WcPath);

			SvnStatusArgs a = new SvnStatusArgs();

			Client.Status(unversioned, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.NotVersioned, Is.EqualTo(this.currentStatus.LocalContentStatus),
				"Wrong text status on " + unversioned);
			Assert.That(string.Compare(unversioned, currentStatus.Path, true) == 0, "Unversioned filenames don't match");


			Client.Status(added, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.Added, Is.EqualTo(this.currentStatus.LocalContentStatus),
				"Wrong text status on " + added);
			Assert.That(string.Compare(added, currentStatus.Path, true) == 0, "Added filenames don't match");


			Client.Status(changed, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.Modified, Is.EqualTo(this.currentStatus.LocalContentStatus),
				"Wrong text status " + changed);
			Assert.That(string.Compare(changed, this.currentStatus.Path, true) == 0, "Changed filenames don't match");


			Client.Status(propChange, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(SvnStatus.Modified, Is.EqualTo(this.currentStatus.LocalPropertyStatus),
				"Wrong property status " + propChange);
			Assert.That(string.Compare(propChange, currentStatus.Path, true) == 0, "Propchanged filenames don't match");

			a.RetrieveAllEntries = true;
			currentStatus = null;

			Client.Status(ignored, a, new EventHandler<SvnStatusEventArgs>(StatusFunc));
			Assert.That(this.currentStatus.LocalContentStatus, Is.EqualTo(SvnStatus.Ignored),
				"Wrong content status " + ignored);
		}


		[Test]
		public void TestEntry()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock " + form);

			string output = this.RunCommand("svn", "info " + form);
			Info info = new Info(output);

			Collection<SvnStatusEventArgs> statuses;

			Client.GetStatus(form, out statuses);

			info.CheckEquals(statuses[0].WorkingCopyInfo);

			//Status s = SingleStatus(client, form);

			SvnStatusArgs a = new SvnStatusArgs();
			this.Client.Status(form, a, new EventHandler<SvnStatusEventArgs>(this.StatusFunc));

			info.CheckEquals(this.currentStatus.WorkingCopyInfo);


		}

        [Test]
        public void CheckRemoteStatus()
        {
            string dir = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, "..\\.."));

            SvnStatusArgs sa = new SvnStatusArgs();
            sa.RetrieveRemoteStatus = true;

            Collection<SvnStatusEventArgs> r;
            Client.GetStatus(dir, sa, out r);
        }

        [Test]
        public void CheckLocalRemoteStatus()
        {
            string dir = WcPath;

            SvnStatusArgs sa = new SvnStatusArgs();
            sa.RetrieveRemoteStatus = true;

            Collection<SvnStatusEventArgs> r;
            using (SvnClient c = new SvnClient())
            {
                c.GetStatus(dir, sa, out r);
            }
        }

		/// <summary>
		/// Tests an update where we contact the repository
		/// </summary>
		/*[Test]
		public void TestRepositoryStatus()
		{
			// modify the file in another working copy and commit
			string wc2 = this.FindDirName(Path.Combine(this.WcPath, "wc2"));
			try
			{
				Zip.ExtractZipResource(wc2, this.GetType(), WC_FILE);
				this.RenameAdminDirs(wc2);
				using (StreamWriter w = new StreamWriter(
						   Path.Combine(wc2, "Form.cs"), true))
					w.Write("Hell worl");
				this.RunCommand("svn", "ci -m \"\" " + wc2);

				// find the status in our own wc
				int youngest;
				string form = Path.Combine(this.WcPath, "Form.cs");
				this.Client.Status(out youngest,
					form, Revision.Head, new StatusCallback(this.StatusFunc),
					false, false, true, true);

				Assert.AreEqual(this.currentStatus.RepositoryTextStatus,
					SvnStatus.Modified, "Wrong status");
			}
			finally
			{
				PathUtils.RecursiveDelete(wc2);
			}
		}*/

        SvnStatusEventArgs SingleStatus(SvnClient client, string path)
        {
            SvnStatusArgs sa = new SvnStatusArgs();
            sa.Depth = SvnDepth.Empty;
            sa.RetrieveAllEntries = true;

            Collection<SvnStatusEventArgs> rslt;
            client.GetStatus(path, sa, out rslt);

            if(rslt.Count != 1)
                return null;
            return rslt[0];
        }

		[Test]
		public void TestSingleStatus()
		{
			string unversioned = this.CreateTextFile("unversioned.cs");
			string added = this.CreateTextFile("added.cs");
			this.RunCommand("svn", "add " + added);

			string changed = this.CreateTextFile("Form.cs");

			string propChange = Path.Combine(this.WcPath, "App.ico");

			SvnStatusEventArgs status = SingleStatus(Client,unversioned);
			Assert.That( status.LocalContentStatus, Is.EqualTo(SvnStatus.NotVersioned),
				"Wrong text status on " + unversioned);

			status = SingleStatus(Client, added);
			Assert.That( status.LocalContentStatus, Is.EqualTo(SvnStatus.Added),
				"Wrong text status on " + added);

			status = SingleStatus(Client, changed);
			Assert.That( status.LocalContentStatus, Is.EqualTo(SvnStatus.Modified),
				"Wrong text status " + changed);

			this.RunCommand("svn", "ps foo bar " + propChange);
			status = SingleStatus(Client, propChange);
			Assert.AreEqual(
				SvnStatus.Modified, status.LocalPropertyStatus,
				"Wrong property status " + propChange);
		}

		[Test]
		public void TestSingleStatusNonExistentPath()
		{
			string doesntExist = Path.Combine(this.WcPath, "doesnt.exist");
			SvnStatusEventArgs status = SingleStatus(Client, doesntExist);
			Assert.That( status, Is.Null);
		}

		/*[Test]
		public void TestSingleStatusUnversionedPath()
		{
			string dir = Path.Combine(this.WcPath, "Unversioned");
			string file = Path.Combine(dir, "file.txt");
			SvnStatusEventArgs status = SingleStatus(Client, file);
			Assert.That( status, Is.EqualTo(SvnStatusEventArgs.None));

		}*/

		[Test]
		public void TestSingleStatusNodeKind()
		{
			string file = Path.Combine(this.WcPath, "Form.cs");
			Assert.That( SingleStatus(Client, file).NodeKind, Is.EqualTo(SvnNodeKind.File));
			Assert.That( SingleStatus(Client, file).WorkingCopyInfo.Name, Is.EqualTo("Form.cs"));

			SvnStatusEventArgs dir = SingleStatus(Client, this.WcPath);
			Assert.That( SingleStatus(Client, this.WcPath).WorkingCopyInfo.NodeKind, Is.EqualTo(SvnNodeKind.Directory));
		}



		[Test]
		public void LockSingleStatusIsNullForUnlocked()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			SvnStatusEventArgs status1 = SingleStatus(Client, form);
			Assert.IsNull(status1.WorkingCopyInfo.LockToken);
		}

		[Test]
		public void LocalLockSingleStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock -m test " + form);

			SvnStatusEventArgs s = SingleStatus(Client, form);
			Assert.IsNotNull(s.WorkingCopyInfo.LockToken);
			Assert.That( s.WorkingCopyInfo.LockOwner, Is.EqualTo(Environment.UserName));
			Assert.That( s.WorkingCopyInfo.LockTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
			Assert.That( s.WorkingCopyInfo.LockComment, Is.EqualTo("test"));
		}

		[Test]
		public void LocalLockStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock -m test " + form);

            SvnStatusArgs sa = new SvnStatusArgs();

            this.Client.Status(form, StatusFunc);
			SvnStatusEventArgs s = this.currentStatus;

			Assert.IsNotNull(s.WorkingCopyInfo.LockToken);
			Assert.That( s.WorkingCopyInfo.LockOwner, Is.EqualTo(Environment.UserName));
			Assert.That( s.WorkingCopyInfo.LockTime.ToLocalTime().Date, Is.EqualTo(DateTime.Now.Date));
			Assert.That( s.WorkingCopyInfo.LockComment, Is.EqualTo("test"));

		}

		/*[Test]
		public void RepositoryLockStatus()
		{
			string form = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "lock -m test " + form);

			int youngest;
			this.Client.Status(out youngest, form, Revision.Unspecified, new StatusCallback(this.StatusFunc),
				false, true, true, false);
			Status status = this.currentStatus;
			Assert.IsNotNull(status.ReposLock);
			Assert.That( status.ReposLock.Owner, Is.EqualTo(Environment.UserName));
			Assert.That( DateTime.Now.Date, Is.EqualTo(status.ReposLock.CreationDate.ToLocalTime().Date));
			Assert.That( status.ReposLock.Comment, Is.EqualTo("test"));
			Assert.IsFalse(status.ReposLock.IsDavComment);
		}*/

		private void StatusFunc(object sender, SvnStatusEventArgs e)
		{
			e.Detach();
			this.currentStatus = e;
		}

		private class Info
		{
			private static readonly Regex INFO = new Regex(@"Path:\s(?'path'.+?)\s+Name:\s(?'name'\S+)\s+" +
				@"Url:\s(?'url'\S+)\s+Repository UUID:\s(?'reposuuid'\S+)\s+Revision:\s(?'revision'\S+)\s+" +
				@"Node Kind:\s(?'nodekind'\S+)\s+Schedule:\s(?'schedule'\S+)\s+Last Changed Author:\s+" +
				@"(?'lastchangedauthor'\S+).*Lock Token:\s+(?'locktoken'\S+)\s+" +
				@"Lock Owner:\s+(?'lockowner'\S+)\s+Lock Created:\s+(?'lockcreated'\S+)",
				RegexOptions.IgnoreCase | RegexOptions.Singleline);

			public Info(string output)
			{
				this.output = output;
			}

			public void CheckEquals(SvnWorkingCopyInfo entry)
			{
				if (!INFO.IsMatch(this.output))
					throw new Exception("No match");

				Match match = INFO.Match(this.output);

				Assert.That(entry.Uri.ToString(), Is.EqualTo(match.Groups["url"].Value), "Url differs");
				Assert.That(entry.Name, Is.EqualTo(match.Groups["name"].Value), "Name differs");
				Assert.That(entry.Revision, Is.EqualTo(int.Parse(match.Groups["revision"].Value)),
					"Revision differs");
				Assert.That(entry.NodeKind.ToString().ToLower(), Is.EqualTo(match.Groups["nodekind"].Value.ToLower()),
					"Node kind differs");
				Assert.That(entry.RepositoryId.ToString(), Is.EqualTo(match.Groups["reposuuid"].Value),
					"Repository UUID differs");
				Assert.That(entry.Schedule.ToString().ToLower(), Is.EqualTo(match.Groups["schedule"].Value.ToLower()),
					"Schedule differs");
				Assert.That(entry.LastChangeAuthor, Is.EqualTo(match.Groups["lastchangedauthor"].Value),
					"Last changed author differs");
				Assert.That(entry.LockToken, Is.EqualTo(match.Groups["locktoken"].Value), "Lock token differs");
				Assert.That(entry.LockOwner, Is.EqualTo(match.Groups["lockowner"].Value), "Lock owner differs");
			}

			private string output;
		}

		private SvnStatusEventArgs currentStatus;
	}
}
