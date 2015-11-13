//
// Copyright 2015 The SharpSvn Project
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
using SharpSvn.Tests.Commands;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using System.Collections.ObjectModel;
using System.IO;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests.LookCommands
{
    [TestClass]
    public class GetLockTests : HookTestBase
    {
        [TestMethod]
        public void LookGetLock_Test()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.DefaultBranched);
            int n = 0;

            Client.RemoteLock(new Uri(sbox.Uri, "src/file3.cpp"), "LOCKED!");

            using (InstallHook(sbox.RepositoryUri, SvnHookType.PreLock,
                delegate (object sender, ReposHookEventArgs e)
                {
                    using (SvnLookClient look = new SvnLookClient())
                    {
                        SvnLockInfo info;
                        look.GetLock(e.HookArgs.LookOrigin, e.HookArgs.Path, out info);
                        n++;
                    }
                }))
            using (InstallHook(sbox.RepositoryUri, SvnHookType.PostLock,
                delegate (object sender, ReposHookEventArgs e)
                {
                    using (SvnLookClient look = new SvnLookClient())
                    {
                        SvnLockInfo info;
                        foreach (string path in e.HookArgs.Paths)
                        {
                            look.GetLock(e.HookArgs.LookOrigin, path, out info);
                            Assert.That(info, Is.Not.Null);
                            n++;
                        }
                        n++; // 1 more
                    }
                }))
            using (InstallHook(sbox.RepositoryUri, SvnHookType.PreUnlock,
                delegate (object sender, ReposHookEventArgs e)
                {
                    using (SvnLookClient look = new SvnLookClient())
                    {
                        SvnLockInfo info;
                        look.GetLock(e.HookArgs.LookOrigin, e.HookArgs.Path, out info);
                        n++;
                    }
                }))
            using (InstallHook(sbox.RepositoryUri, SvnHookType.PostUnlock,
                delegate (object sender, ReposHookEventArgs e)
                {
                    using (SvnLookClient look = new SvnLookClient())
                    {
                        SvnLockInfo info;
                        foreach (string path in e.HookArgs.Paths)
                        {
                            look.GetLock(e.HookArgs.LookOrigin, path, out info);
                            Assert.That(info, Is.Null);
                            n++;
                        }
                        n++; // 1 more
                    }
                }))
            {

                using (SvnLookClient look = new SvnLookClient())
                {
                    SvnLockInfo info;
                    look.GetLock(new SvnLookOrigin(sbox.RepositoryUri.LocalPath), "trunk/src/file3.cpp", out info);
                    Assert.That(info, Is.Not.Null);
                    Assert.That(info.Comment, Is.EqualTo("LOCKED!"));
                }
                Client.Lock(Path.Combine(sbox.Wc, "src", "file1.cs"), "Can I?");

                Client.Unlock(Path.Combine(sbox.Wc, "src", "file1.cs"));

                string[] twopaths = new string[]
                {
                     Path.Combine(sbox.Wc, "src", "file1.cs"),
                     Path.Combine(sbox.Wc, "src", "file2.vb")
                };
                Client.Lock(twopaths, "Can I?");
                Client.Unlock(twopaths);
            }

            Assert.That(n, Is.EqualTo(16));
        }
    }
}
