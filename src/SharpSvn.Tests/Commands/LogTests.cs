// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::Log
	/// </summary>
	[TestFixture]
	public class LogTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

			this.ExtractRepos();
			this.logMessages.Clear();
		}

		[Test]
		public void TestLog()
		{
			ClientLogMessage[] clientLogs = this.ClientLog(this.ReposUrl);

			//the client prints them in a reverse order by default
			Array.Reverse(clientLogs);
			SvnLogArgs a = new SvnLogArgs();
			a.Range = new SvnRevisionRange(1, SvnRevision.Head);
			a.RetrieveChangedPaths = false;

			this.Client.Log(this.ReposUrl, a, new EventHandler<SvnLogEventArgs>(LogCallback));

			Assert.That(this.logMessages.Count, Is.EqualTo(clientLogs.Length),
				"Number of log entries differs");
			for (int i = 0; i < this.logMessages.Count; i++)
				clientLogs[i].CheckMatch(this.logMessages[i]);
		}

		[Test]
		public void TestLogNonAsciiChars()
		{
			this.RunCommand("svn", "propset svn:log --revprop -r 1  \"Æ e i a æ å, sjø\" " +
				this.ReposUrl);

			SvnLogArgs a = new SvnLogArgs();
			a.Range = new SvnRevisionRange(1, 1);

			this.Client.Log(this.ReposUrl, a, new EventHandler<SvnLogEventArgs>(LogCallback));
			Assert.That(this.logMessages.Count, Is.EqualTo(1));
			Assert.That(this.logMessages[0].LogMessage, Is.EqualTo("Æ e i a æ å, sjø"));
		}

        [Test]
        public void LogFromFile()
        {
            Uri repos = this.GetReposUri(TestReposType.CollabRepos);

            string dir = GetTempDir();

            using (SvnClient client = new SvnClient())
            {
                client.CheckOut(new Uri(repos, "trunk/"), dir);

                int n = 0;
                client.Log(Path.Combine(dir, "index.html"),
                    delegate(object sender, SvnLogEventArgs e)
                    {
                        switch (n++)
                        {
                            case 0:
                                Assert.That(e.LogMessage, Is.EqualTo("Merge branch b - product roadmap and update about page"));
                                break;
                            case 1:
                                Assert.That(e.LogMessage, Is.EqualTo("Merge branch a.  Added medium product"));
                                break;
                            case 2:
                                Assert.That(e.LogMessage, Is.EqualTo("Create initial product structure"));
                                break;
                        }
                    });
                Assert.That(n, Is.EqualTo(3));

            }
        }

		private void LogCallback(object sender, SvnLogEventArgs e)
		{
			e.Detach();
			this.logMessages.Add(e);
		}

		private ClientLogMessage[] ClientLog(string path)
		{
			string output = this.RunCommand("svn", "log --xml " + path);
			XmlDocument doc = new XmlDocument();
			doc.PreserveWhitespace = true;
			doc.LoadXml(output);

			ArrayList list = new ArrayList();

			foreach (XmlNode node in doc.SelectNodes("/log/logentry"))
				list.Add(new ClientLogMessage(node));

			return (ClientLogMessage[])list.ToArray(typeof(ClientLogMessage));
		}

		private ClientLogMessage[] ClientLog(Uri uri)
		{
			string output = this.RunCommand("svn", "log --xml " + uri.ToString());
			XmlDocument doc = new XmlDocument();
			doc.PreserveWhitespace = true;
			doc.LoadXml(output);

			ArrayList list = new ArrayList();

			foreach (XmlNode node in doc.SelectNodes("/log/logentry"))
				list.Add(new ClientLogMessage(node));

			return (ClientLogMessage[])list.ToArray(typeof(ClientLogMessage));
		}

		/// <summary>
		/// Represents a log message gotten from the command line client
		/// </summary>
		private class ClientLogMessage
		{
			public ClientLogMessage(XmlNode node)
			{
				this.date = this.ParseTime(node["date"].InnerText);
				this.revision = int.Parse(node.Attributes["revision"].InnerText);

				// need to get rid of some redundant whitespace
				// (probably introduced somewhere in the XML process)
				string msg = node["msg"].InnerText;
				string[] lines = Regex.Split(msg, Environment.NewLine);
				for (int i = 0; i < lines.Length; i++)
					lines[i] = lines[i].Trim();

				this.message = string.Join(Environment.NewLine, lines);

				this.author = node["author"].InnerText;
			}

			/// <summary>
			/// Does it match the LogMessage object
			/// </summary>
			/// <param name="msg"></param>
			public void CheckMatch(SvnLogEventArgs e)
			{
				Assert.That(e.Author, Is.EqualTo(this.author), "Author differs");
				Assert.That(e.LogMessage, Is.EqualTo(this.message), "Message differs");
				Assert.That(e.Revision, Is.EqualTo(this.revision), "Revision differs");
				Assert.That(e.Time, Is.EqualTo(this.date), "Date differs");
			}

			private DateTime ParseTime(string dateString)
			{
				return DateTime.ParseExact(dateString,
					@"yyyy-MM-dd\THH:mm:ss.ffffff\Z",
					CultureInfo.CurrentCulture);
			}

			private DateTime date;
			private string author;
			private string message;
			private int revision;


		}

		private List<SvnLogEventArgs> logMessages = new List<SvnLogEventArgs>();
	}
}
