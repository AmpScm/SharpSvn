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

// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;


namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::Log
    /// </summary>
    [TestClass]
    public class LogTests : TestBase
    {
        [TestInitialize]
        public void LogSetUp()
        {
            logMessages.Clear();
        }

        [TestMethod]
        public void Log_TestLog()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.AnkhSvnCases);
            ClientLogMessage[] clientLogs = this.ClientLog(ReposUrl);

            //the client prints them in a reverse order by default
            Array.Reverse(clientLogs);
            SvnLogArgs a = new SvnLogArgs();
            a.Range = new SvnRevisionRange(1, SvnRevision.Head);
            a.RetrieveChangedPaths = false;

            this.Client.Log(ReposUrl, a, LogCallback);

            Assert.That(this.logMessages.Count, Is.EqualTo(clientLogs.Length),
                "Number of log entries differs");
            for (int i = 0; i < this.logMessages.Count; i++)
                clientLogs[i].CheckMatch(this.logMessages[i]);
        }

        [TestMethod]
        public void Log_StrangeLog()
        {
            // With Subversion 1.5.4 this gives thousands of result values
            SvnLogArgs la = new SvnLogArgs();
            la.Start = 874743;
            la.End = 0;
            la.Limit = 10;
            la.OperationalRevision = 874743;
            la.RetrieveMergedRevisions = true;
            la.RetrieveChangedPaths = false;
            la.ThrowOnCancel = false;
            int n = 0;
            Client.Log(new Uri("http://svn.apache.org/repos/asf/subversion/branches/1.5.x-reintegrate-improvements/"), la,
                delegate (object sender, SvnLogEventArgs le)
                {
                    n++;

                    if (n > 100)
                        le.Cancel = true;
                });

            Assert.That(la.IsLastInvocationCanceled, "More than 100 items returned");
        }

        [TestMethod]
        public void Log_TestLogNonAsciiChars()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Default);
            sbox.InstallRevpropHook(ReposUrl);
            this.RunCommand("svn", "propset svn:log --revprop -r 1  \" e i a  , sj\" " +
                ReposUrl);

            SvnLogArgs a = new SvnLogArgs();
            a.Range = new SvnRevisionRange(1, 1);

            this.Client.Log(ReposUrl, a, LogCallback);
            Assert.That(this.logMessages.Count, Is.EqualTo(1));
            Assert.That(this.logMessages[0].LogMessage, Is.EqualTo(" e i a  , sj"));
        }

        [TestMethod]
        public void Log_LogFromFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri repos = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string dir = sbox.Wc;

            using (SvnClient client = new SvnClient())
            {
                client.CheckOut(new Uri(repos, "trunk/"), dir);

                int n = 0;
                client.Log(Path.Combine(dir, "index.html"),
                    delegate (object sender, SvnLogEventArgs e)
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

        [TestMethod]
        public void Log_TestLocalLogVariants()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            SvnLogArgs a = new SvnLogArgs();
            string dir = sbox.Wc;
            Client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir);

            bool touched = false;
            Client.Log(dir, delegate (object sender, SvnLogEventArgs e)
            {
                touched = true;
            });
            Assert.That(touched);

            touched = false;

            touched = false;
            Client.Log(dir, a, delegate (object sender, SvnLogEventArgs e)
            {
                touched = true;

            });
            Assert.That(touched);

            touched = false;
            Client.Log(Path.Combine(dir, "index.html"), a, delegate (object sender, SvnLogEventArgs e)
            {
                touched = true;

            });
            Assert.That(touched);


            touched = false;
            Client.Log(new string[]
            {
                dir,
                Path.Combine(dir, "index.html")
            }, a, delegate (object sender, SvnLogEventArgs e)
            {
                touched = true;
            });

            Assert.That(touched);
        }

        [TestMethod]
        public void Log_TestLogSingleFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            bool touched = false;
            SvnLogArgs a = new SvnLogArgs();
            string dir = sbox.Wc;
            Client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir);
            touched = false;

            Client.Log(Path.Combine(dir, "index.html"), a, delegate (object sender, SvnLogEventArgs e)
                {
                    touched = true;

                });
            Assert.That(touched);
        }


        [TestMethod]
        public void Log_TestMultiLocalLogs()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            bool touched = false;
            SvnLogArgs a = new SvnLogArgs();
            string dir = sbox.Wc;
            Client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir);
            touched = false;
            Client.Log(new string[]
            {
                Path.Combine(dir, "products/big.html"),
                Path.Combine(dir, "news/index.html"),
                Path.Combine(dir, "index.html"),
            }, a, delegate (object sender, SvnLogEventArgs e)
            {
                touched = true;
            });

            Assert.That(touched);
        }

        [TestMethod]
        public void Log_MultiLogs()
        {
            bool touched = false;
            Assert.That(Client.Log(new Uri("http://svn.apache.org/repos/asf/subversion/trunk/"),
                delegate (object sender, SvnLogEventArgs e)
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

            Assert.That(Client.Log(new Uri("http://svn.apache.org/repos/asf/subversion/trunk/"),
                delegate (object sender, SvnLogEventArgs e)
                {
                    foreach (SvnChangeItem item in e.ChangedPaths)
                    {
                        Uri uri = new Uri(new Uri("http://svn.collab.net/repos/asf/"), item.Path.TrimStart('/'));

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
            uris.Add(new Uri("http://svn.apache.org/repos/asf/subversion/trunk/README"));
            uris.Add(new Uri("http://svn.apache.org/repos/asf/subversion/trunk/INSTALL"));
            SvnLogArgs args = new SvnLogArgs();
            args.Start = 863074;
            args.End = 859074;
            args.Limit = 100;
            args.OperationalRevision = 863074;
            touched = false;
            Client.Log(uris, args, delegate (object sender, SvnLogEventArgs e)
            {
                Assert.That(e.LogOrigin, Is.Not.Null);
                touched = true;
            });

            Assert.That(touched);
        }

        [TestMethod, ExpectedException(typeof(ArgumentException))]
        public void Log_TestNoMultiUris()
        {
            Collection<SvnLogEventArgs> result;
            SvnLogArgs la = new SvnLogArgs();
            Client.GetLog(new Uri[0], la, out result);
        }

        [TestMethod]
        public void Log_OldLog()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.AnkhSvnCases);
            Uri repos = sbox.RepositoryUri;

            string dir = sbox.Wc;

            string file;
            File.WriteAllText(file = Path.Combine(dir, "qwqwqw.q"), "fds gsdfgsfdgdsf");
            Client.Add(file);
            Client.Commit(file);

            Client.Log(repos,
                delegate (object sender, SvnLogEventArgs e)
                {
                    if (e.Revision != 0)
                        foreach (SvnChangeItem ci in e.ChangedPaths)
                        {
                            SvnNodeKind expected;
                            if (!string.IsNullOrEmpty(Path.GetExtension(ci.Path)))
                                expected = SvnNodeKind.File;
                            else
                                expected = SvnNodeKind.Directory;
                            Assert.That(ci.NodeKind, Is.EqualTo(SvnNodeKind.Unknown).Or.EqualTo(expected), "{0}", ci.Path);
                        }
                });
        }

        [TestMethod]
        public void Log_LogCreate()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            string WcPath = sbox.Wc;
            Uri WcUri = new Uri(CollabReposUri, "trunk/");

            Client.CheckOut(WcUri, WcPath);

            string logFile = Path.Combine(WcPath, "LogTestFileBase");
            TouchFile(logFile);
            Client.Add(logFile);
            SvnCommitArgs a = new SvnCommitArgs();
            a.LogMessage = "Commit 1\rWith\nSome\r\nRandom\n\rNewlines\nAdded\n\r\n";
            Client.Commit(WcPath, a);

            File.AppendAllText(logFile, Guid.NewGuid().ToString());
            a.LogMessage = "Commit 2";
            Client.SetProperty(logFile, "TestProperty", "TestValue");
            Client.Commit(WcPath, a);

            string renamedLogFile = Path.Combine(WcPath, "LogTestFileDest");
            Client.Move(logFile, renamedLogFile);
            a.LogMessage = "Commit 3" + Environment.NewLine + "With newline";
            Client.Commit(WcPath, a);

            int n = 0;
            SvnLogArgs la = new SvnLogArgs();
            la.StrictNodeHistory = false;


            EventHandler<SvnLogEventArgs> verify = delegate (object sender, SvnLogEventArgs e)
            {
                SvnChangeItem ci;
                SvnChangeItem ci2;

                Assert.That(e.Author, Is.EqualTo(Environment.UserName));
                Assert.That(e.Cancel, Is.False);
                Assert.That(e.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 5, 0)));
                Assert.That(e.HasChildren, Is.False);
                Assert.That(e.RevisionProperties, Is.Not.Null);
                Assert.That(e.RevisionProperties.Count, Is.GreaterThanOrEqualTo(3));
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

            Client.Log(new Uri(WcUri, "LogTestFileDest"), verify);
            Assert.That(n, Is.EqualTo(3));

            n = 0;

            Client.Log(Path.Combine(WcPath, "LogTestFileDest"), verify);
            Assert.That(n, Is.EqualTo(3));
        }



        [TestMethod, ExpectedException(typeof(SvnFileSystemException))]
        public void Log_ExpectLogException()
        {
            using (SvnClient client = new SvnClient())
            {
                // Throws an SvnRepositoryIOException in 1.5.x@29330

                List<Uri> uris = new List<Uri>();
                uris.Add(new Uri("http://svn.apache.org/repos/asf/subversion/README"));
                uris.Add(new Uri("http://svn.apache.org/repos/asf/subversion/INSTALL"));
                SvnLogArgs args = new SvnLogArgs();
                client.Log(uris, args, delegate (object sender, SvnLogEventArgs e)
                { });
            }
        }

        private void LogCallback(object sender, SvnLogEventArgs e)
        {
            e.Detach();
            this.logMessages.Add(e);
        }

        #region SVN Client verifier
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
        #endregion

        private List<SvnLogEventArgs> logMessages = new List<SvnLogEventArgs>();
    }
}
