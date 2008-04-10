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

        [Test]
        public void TestLocalLogVariants()
        {
            SvnLogArgs a = new SvnLogArgs();
            string dir = GetTempDir();
            Client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir);

            bool touched = false;
            Client.Log(dir, delegate(object sender, SvnLogEventArgs e)
            {
                touched = true;
            });
            Assert.That(touched);

            touched = false;

            touched = false;
            Client.Log(dir, a, delegate(object sender, SvnLogEventArgs e)
            {
                touched = true;

            });
            Assert.That(touched);


            touched = false;
            Client.Log(new string[] 
            {
                dir,
                Path.Combine(dir, "index.html")
            }, a, delegate(object sender, SvnLogEventArgs e)
            {
                touched = true;
            });

            Assert.That(touched);
        }


        [Test]
        public void TestMultiLocalLogs()
        {
            bool touched = false;
            SvnLogArgs a = new SvnLogArgs();
            string dir = GetTempDir();
            Client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir);
            touched = false;
            Client.Log(new string[] 
            {
                Path.Combine(dir, "products/big.html"),
                Path.Combine(dir, "news/index.html"),
                Path.Combine(dir, "index.html"),
            }, a, delegate(object sender, SvnLogEventArgs e)
            {
                touched = true;
            });

            Assert.That(touched);
        }

        [Test]
        public void TestMultiLogs()
        {
            bool touched = false;
            Assert.That(Client.Log(new SvnUriTarget(new Uri("http://svn.collab.net/repos/svn/trunk/")),
                delegate(object sender, SvnLogEventArgs e)
                {
                    touched = true;
                    e.Cancel = true;
                    Assert.That(e.MergeLogNestingLevel, Is.EqualTo(0));
                    Assert.That(e.Revision, Is.GreaterThan(20000L));
                    Assert.That(e.LogMessage, Is.Not.Null);
                    Assert.That(e.Time, Is.GreaterThan(new DateTime(2008, 01, 01)));
                    Assert.That(e.Author, Is.Not.Null);
                    Assert.That(e.ChangedPaths, Is.Not.Null);
                    Assert.That(e.LogOrigin, Is.Null);
                }), Is.False);

            Assert.That(touched);

            List<Uri> uris = new List<Uri>();
            long rev = 0;

            Assert.That(Client.Log(new SvnUriTarget(new Uri("http://svn.collab.net/repos/svn/trunk/")),
                delegate(object sender, SvnLogEventArgs e)
                {
                    foreach (SvnChangeItem item in e.ChangedPaths)
                    {
                        Uri uri = new Uri(new Uri("http://svn.collab.net/repos/"), item.Path.TrimStart('/'));

                        if (item.Action == SvnChangeAction.Delete)
                            uris.Remove(uri);
                        else if (item.Action == SvnChangeAction.Add || item.Action == SvnChangeAction.Modify)
                            uris.Add(uri);
                    }

                    if (uris.Count > 10)
                    {
                        e.Cancel = true;
                        rev = e.Revision - 1;
                    }
                }), Is.False);

            Assert.That(uris.Count, Is.GreaterThan(10));

            uris.Clear();
            uris.Add(new Uri("http://svn.collab.net/repos/svn/trunk/README"));
            uris.Add(new Uri("http://svn.collab.net/repos/svn/trunk/INSTALL"));
            SvnLogArgs args = new SvnLogArgs();
            args.Start = 23000;
            args.End = 19000;
            args.Limit = 100;
            args.OriginRevision = 23000;
            touched = false;
            Client.Log(uris, args, delegate(object sender, SvnLogEventArgs e)
            {
                Assert.That(e.LogOrigin, Is.Not.Null);
                touched = true;
            });

            Assert.That(touched);
        }

        [Test]
        public void TestLogCreate()
        {
            SvnClient client = Client;
            string WcPath = GetTempDir();
            Uri WcUri = new Uri(CollabReposUri, "trunk/");

            Client.CheckOut(WcUri, WcPath);


            string logFile = Path.Combine(WcPath, "LogTestFileBase");
            TouchFile(logFile);
            client.Add(logFile);
            SvnCommitArgs a = new SvnCommitArgs();
            a.LogMessage = "Commit 1\rWith\nSome\r\nRandom\n\rNewlines\nAdded\n\r\n";
            client.Commit(WcPath, a);

            File.AppendAllText(logFile, Guid.NewGuid().ToString());
            a.LogMessage = "Commit 2";
            client.SetProperty(logFile, "TestProperty", "TestValue");
            client.Commit(WcPath, a);

            string renamedLogFile = Path.Combine(WcPath, "LogTestFileDest");
            client.Move(logFile, renamedLogFile);
            a.LogMessage = "Commit 3" + Environment.NewLine + "With newline";
            client.Commit(WcPath, a);

            int n = 0;
            SvnLogArgs la = new SvnLogArgs();
            la.StrictNodeHistory = false;


            EventHandler<SvnLogEventArgs> verify = delegate(object sender, SvnLogEventArgs e)
            {
                SvnChangeItem ci;
                SvnChangeItem ci2;

                Assert.That(e.Author, Is.EqualTo(Environment.UserName));
                Assert.That(e.Cancel, Is.False);
                Assert.That(e.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                Assert.That(e.HasChildren, Is.False);
                Assert.That(e.CustomProperties, Is.Not.Null);
                Assert.That(e.CustomProperties.Count, Is.GreaterThanOrEqualTo(3));
                switch (n)
                {
                    case 0:
                        Assert.That(e.LogMessage, Is.EqualTo("Commit 3" + Environment.NewLine + "With newline"));
                        Assert.That(e.ChangedPaths, Is.Not.Null);
                        Assert.That(e.ChangedPaths.Count, Is.EqualTo(2));
                        Assert.That(e.ChangedPaths.Contains("/trunk/LogTestFileBase"));
                        Assert.That(e.ChangedPaths.Contains("/trunk/LogTestFileDest"));
                        ci = e.ChangedPaths["/trunk/LogTestFileBase"];
                        ci2 = e.ChangedPaths["/trunk/LogTestFileDest"];

                        Assert.That(ci, Is.Not.Null);
                        Assert.That(ci2, Is.Not.Null);
                        Assert.That(ci.Path, Is.EqualTo("/trunk/LogTestFileBase"));
                        Assert.That(ci.Action, Is.EqualTo(SvnChangeAction.Delete));
                        Assert.That(ci.CopyFromPath, Is.Null);
                        Assert.That(ci.CopyFromRevision, Is.EqualTo(-1));
                        Assert.That(ci2.Path, Is.EqualTo("/trunk/LogTestFileDest"));
                        Assert.That(ci2.Action, Is.EqualTo(SvnChangeAction.Add));
                        Assert.That(ci2.CopyFromPath, Is.EqualTo("/trunk/LogTestFileBase"));
                        Assert.That(ci2.CopyFromRevision, Is.Not.EqualTo(-1));
                        break;
                    case 1:
                        Assert.That(e.LogMessage, Is.EqualTo("Commit 2"));
                        Assert.That(e.ChangedPaths, Is.Not.Null);
                        Assert.That(e.ChangedPaths.Count, Is.EqualTo(1));
                        ci = e.ChangedPaths[0];
                        Assert.That(ci, Is.Not.Null);
                        Assert.That(ci.Path, Is.EqualTo("/trunk/LogTestFileBase"));
                        Assert.That(ci.Action, Is.EqualTo(SvnChangeAction.Modify));
                        Assert.That(ci.CopyFromPath, Is.Null);
                        Assert.That(ci.CopyFromRevision, Is.EqualTo(-1));
                        break;
                    case 2:
                        Assert.That(e.LogMessage, Is.EqualTo("Commit 1" + Environment.NewLine + "With" +
                            Environment.NewLine + "Some" + Environment.NewLine + "Random" + Environment.NewLine +
                            "Newlines" + Environment.NewLine + "Added" + Environment.NewLine + Environment.NewLine));
                        Assert.That(e.ChangedPaths, Is.Not.Null);
                        Assert.That(e.ChangedPaths.Count, Is.EqualTo(1));
                        ci = e.ChangedPaths[0];
                        Assert.That(ci, Is.Not.Null);
                        Assert.That(ci.Path, Is.EqualTo("/trunk/LogTestFileBase"));
                        Assert.That(ci.Action, Is.EqualTo(SvnChangeAction.Add));
                        Assert.That(ci.CopyFromPath, Is.Null);
                        Assert.That(ci.CopyFromRevision, Is.EqualTo(-1));
                        break;
                    default:
                        Assert.That(false);
                        break;
                }
                n++;
            };

            client.Log(new SvnUriTarget(new Uri(WcUri, "LogTestFileDest")), verify);
            Assert.That(n, Is.EqualTo(3));

            n = 0;

            client.Log(Path.Combine(WcPath, "LogTestFileDest"), verify);
            Assert.That(n, Is.EqualTo(3));
        }



        [Test, ExpectedException(typeof(SvnFileSystemException))]
        public void ExpectLogException()
        {
            using (SvnClient client = new SvnClient())
            {
                // Throws an SvnRepositoryIOException in 1.5.x@29330

                List<Uri> uris = new List<Uri>();
                uris.Add(new Uri("http://svn.collab.net/repos/svn/README"));
                uris.Add(new Uri("http://svn.collab.net/repos/svn/INSTALL"));
                SvnLogArgs args = new SvnLogArgs();
                client.Log(uris, args, delegate(object sender, SvnLogEventArgs e)
                { });
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
