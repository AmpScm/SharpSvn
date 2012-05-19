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
using SharpSvn.Security;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests for the Client::Checkout method
    /// </summary>
    [TestFixture]
    public class CheckOutTests : TestBase
    {
        [Test]
        public void TestPegCheckOuts()
        {
            Uri repos = new Uri(GetReposUri(TestReposType.CollabRepos), "trunk/");

            SvnUpdateResult result;
            Assert.That(Client.CheckOut(repos, GetTempDir(), out result));

            Assert.That(result, Is.Not.Null);
            long head = result.Revision;

            Assert.That(Client.CheckOut(new SvnUriTarget(repos, head - 10), GetTempDir(), out result));
            Assert.That(result.Revision, Is.EqualTo(head - 10));

            SvnCheckOutArgs a = new SvnCheckOutArgs();
            a.Revision = head - 5;

            Assert.That(Client.CheckOut(new SvnUriTarget(repos, head - 10), GetTempDir(), a, out result));
            Assert.That(result.Revision, Is.EqualTo(head - 5));

            Assert.That(Client.CheckOut(repos, GetTempDir(), a, out result));
            Assert.That(result.Revision, Is.EqualTo(head - 5));
        }


        /// <summary>
        /// Test a standard checkout operation
        /// </summary>
        [Test]
        public void TestBasicCheckout()
        {
            string newWc = GetTempDir();
            SvnCheckOutArgs a = new SvnCheckOutArgs();
            this.Client.CheckOut(this.ReposUrl, newWc, a);

            Assert.That(File.Exists(Path.Combine(newWc, "Form.cs")),
                "Checked out file not there");
            Assert.That(Directory.Exists(Path.Combine(newWc, SvnClient.AdministrativeDirectoryName)),
                "No admin directory found");
            Assert.That(this.RunCommand("svn", "st \"" + newWc + "\"").Trim(), Is.EqualTo(""),
                "Wrong status");
        }

        [Test]
        public void TestProgressEvent()
        {
            string newWc = GetTempDir();
            SvnCheckOutArgs a = new SvnCheckOutArgs();
            a.Progress += new EventHandler<SvnProgressEventArgs>(Client_Progress);
            a.Depth = SvnDepth.Empty;
            this.Client.CheckOut(new Uri("http://svn.apache.org/repos/asf/subversion/"), newWc, a);

            Assert.That(progressCalled, "Progress delegate not called");
        }

        [Test]
        public void SerfCheckout()
        {
            string cfgDir = GetTempDir();
            using (SvnClient client = new SvnClient())
                client.LoadConfiguration(cfgDir, true);

            string serversFile = Path.Combine(cfgDir, "servers");
            string serversData = File.ReadAllText(serversFile);

            foreach (string httpLibrary in new string[] { "", "serf", "neon" })
            {
                string servers = serversData;
                if (!string.IsNullOrEmpty(httpLibrary))
                    servers += Environment.NewLine + "http-library=" + httpLibrary + Environment.NewLine;

                File.WriteAllText(serversFile, servers);
                bool once = false;

                using (SvnClient cl = new SvnClient())
                {
                    cl.LoadConfiguration(cfgDir, false);
                    cl.Authentication.UserNamePasswordHandlers +=
                        delegate(object sender, SvnUserNamePasswordEventArgs e)
                        {
                            Assert.That(!once);
                            once = true;
                            e.UserName = "guest";
                        };

                    cl.CheckOut(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk/imports/scripts"), GetTempDir());
                    Assert.That(once, "Asked for password");
                }
            }
        }

        void Client_Progress(object sender, SvnProgressEventArgs args)
        {
            progressCalled = true;
        }

        private bool progressCalled = false;
    }
}
