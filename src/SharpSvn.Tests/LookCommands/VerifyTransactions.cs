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

using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using System.ComponentModel;
using System.Threading;
using System.CodeDom.Compiler;
using System.IO;
using SharpSvn;

namespace SharpSvn.Tests.LookCommands
{
    [TestFixture]
    public class VerifyTransactions : HookTestBase
    {
        void OnPreCommit(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();

                SvnChangeInfoEventArgs i;
                Assert.That(cl.GetChangeInfo(e.HookArgs.LookOrigin, ia, out i));

                Assert.That(i.Revision, Is.LessThan(0L));
                Assert.That(i.Author, Is.EqualTo(Environment.UserName));
                Assert.That(i.LogMessage, Is.EqualTo("Blaat!\r\nQWQQ\r\n"));

                using (MemoryStream ms = new MemoryStream())
                {
                    SvnLookWriteArgs wa = new SvnLookWriteArgs();

                    cl.Write(e.HookArgs.LookOrigin, "trunk/Pre.txt", ms, wa);
                    ms.Position = 0;

                    using (StreamReader sr = new StreamReader(ms))
                    {
                        Assert.That(sr.ReadToEnd(), Is.EqualTo("AllTheData"));
                    }

                    string v;
                    cl.GetProperty(e.HookArgs.LookOrigin, "trunk/Pre.txt", "boe", out v);

                    Assert.That(v, Is.EqualTo("blaat"));

                    SvnPropertyCollection props;
                    cl.GetPropertyList(e.HookArgs.LookOrigin, "trunk/Pre.txt", out props);

                    Assert.That(props, Is.Not.Null);
                    Assert.That(props.Count, Is.EqualTo(1));
                }

                Guid id;
                Assert.That(cl.TryGetRepositoryId(e.HookArgs.LookOrigin, out id));
                Assert.That(id, Is.Not.EqualTo(Guid.Empty));
            }
        }

        [Test]
        public void TestPreCommit()
        {
            Uri uri = PathToUri(CreateRepos(TestReposType.CollabRepos), true);

            using (InstallHook(uri, SvnHookType.PreCommit, OnPreCommit))
            {
                string dir = GetTempDir();
                Client.CheckOut(new Uri(uri, "trunk/"), dir);

                string pre = Path.Combine(dir, "Pre.txt");

                File.WriteAllText(pre, "AllTheData");

                Client.Add(pre);
                Client.SetProperty(pre, "boe", "blaat");
                SvnCommitArgs ca = new SvnCommitArgs();
                ca.LogMessage = "Blaat!\nQWQQ\n";
                Client.Commit(dir, ca);
            }
        }
        void OnPostCommit(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient cl = new SvnLookClient())
            {
                SvnChangeInfoArgs ia = new SvnChangeInfoArgs();

                SvnChangeInfoEventArgs i;
                Assert.That(cl.GetChangeInfo(e.HookArgs.LookOrigin, ia, out i));

                GC.KeepAlive(i);
                Assert.That(i.Revision, Is.GreaterThanOrEqualTo(0L));
                Assert.That(i.Author, Is.EqualTo(Environment.UserName));
            }
        }

        [Test]
        public void TestPostCommit()
        {
            Uri uri = PathToUri(CreateRepos(TestReposType.CollabRepos), true);

            using (InstallHook(uri, SvnHookType.PostCommit, OnPostCommit))
            {
                string dir = GetTempDir();
                Client.CheckOut(new Uri(uri, "trunk/"), dir);

                TouchFile(Path.Combine(dir, "Post.txt"));
                Client.Add(Path.Combine(dir, "Post.txt"));
                SvnCommitArgs ca = new SvnCommitArgs();
                ca.LogMessage = "Blaat!\nQWQQ\n";
                Client.Commit(dir, ca);
            }
        }
    }
}
