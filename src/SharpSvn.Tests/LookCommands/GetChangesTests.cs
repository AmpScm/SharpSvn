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
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn.Tests.Commands;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.LookCommands
{
    [TestClass]
    public class GetChangesTests : HookTestBase
    {

        [TestMethod]
        [DoNotParallelize]
        public void ChangedDirs()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri uri = sbox.CreateRepository(SandBoxRepository.Empty);
            using (InstallHook(uri, SvnHookType.PreCommit, OnChangedDirs))
            {
                using (SvnClient cl = new SvnClient())
                {
                    SvnCreateDirectoryArgs da = new SvnCreateDirectoryArgs();
                    da.CreateParents = true;
                    da.LogMessage = "Created!";
                    cl.RemoteCreateDirectories(
                    new Uri[]
                        {
                        new Uri(uri, "a/b/c/d/e/f"),
                        new Uri(uri, "a/b/c/d/g/h"),
                        new Uri(uri, "i/j/k"),
                        new Uri(uri, "l/m/n"),
                        new Uri(uri, "l/m/n/o/p")
                        }, da);
                }
            }

            using (InstallHook(uri, SvnHookType.PreCommit, OnCopyDir))
            {
                using (SvnClient cl = new SvnClient())
                {
                    SvnCopyArgs ca = new SvnCopyArgs();
                    ca.CreateParents = true;
                    ca.LogMessage = "Created!";
                    cl.RemoteCopy(
                    new SvnUriTarget[]
                        {
                        new Uri(uri, "a/b/c/d"),
                        new Uri(uri, "i/j")
                        }, uri, ca);
                }
            }
        }

        private void OnChangedDirs(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient lc = new SvnLookClient())
            {
                SvnChangedArgs ca = new SvnChangedArgs();
                ca.Transaction = e.HookArgs.TransactionName;

                Collection<SvnChangedEventArgs> list;
                Assert.That(lc.GetChanged(e.HookArgs.LookOrigin, ca, out list));

                Assert.That(list.Count, Is.EqualTo(16));
                Assert.That(list[0].Name, Is.EqualTo("a"));
                Assert.That(list[0].Path, Is.EqualTo("/a/"));
                Assert.That(list[1].Name, Is.EqualTo("b"));
                Assert.That(list[1].Path, Is.EqualTo("/a/b/"));
                Assert.That(list[2].Name, Is.EqualTo("c"));
                Assert.That(list[2].Path, Is.EqualTo("/a/b/c/"));
            }
        }

        private void OnCopyDir(object sender, ReposHookEventArgs e)
        {
            using (SvnLookClient lc = new SvnLookClient())
            {
                SvnChangedArgs ca = new SvnChangedArgs();
                ca.Transaction = e.HookArgs.TransactionName;

                Collection<SvnChangedEventArgs> list;
                Assert.That(lc.GetChanged(e.HookArgs.LookOrigin, ca, out list));
                Assert.That(list.Count, Is.EqualTo(2));

                Assert.That(list[0].Name, Is.EqualTo("d"));
                Assert.That(list[0].Path, Is.EqualTo("/d/"));
                Assert.That(list[0].CopyFromPath, Is.EqualTo("/a/b/c/d"));
                Assert.That(list[0].CopyFromRevision, Is.EqualTo(1L));
                Assert.That(list[1].Name, Is.EqualTo("j"));
                Assert.That(list[1].Path, Is.EqualTo("/j/"));
                Assert.That(list[1].CopyFromPath, Is.EqualTo("/i/j"));
                Assert.That(list[1].CopyFromRevision, Is.EqualTo(1L));
            }
        }

        [TestMethod]
        [DoNotParallelize]
        public void PostCommitErrorTest()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri uri = sbox.CreateRepository(SandBoxRepository.Empty);
            using (InstallHook(uri, SvnHookType.PostCommit, OnPostCommit))
            {
                using (SvnClient cl = new SvnClient())
                {
                    SvnCommitResult cr;
                    SvnCreateDirectoryArgs da = new SvnCreateDirectoryArgs();
                    da.CreateParents = true;
                    da.LogMessage = "Created!";
                    cl.RemoteCreateDirectory(new Uri(uri, "a/b/c/d/e/f"), da, out cr);

                    Assert.That(cr, Is.Not.Null);
                    Assert.That(cr.PostCommitError.Contains(Environment.NewLine));
                    Assert.That(cr.PostCommitError.Substring(
                                                    cr.PostCommitError.IndexOf(Environment.NewLine, StringComparison.OrdinalIgnoreCase)
                                                    + Environment.NewLine.Length),
                                            Is.EqualTo("The Post Commit Warning"));
                }
            }
        }

        private void OnPostCommit(object sender, ReposHookEventArgs e)
        {
            e.ErrorText = "The Post Commit Warning";
            e.ExitCode = 1;
        }
    }
}
