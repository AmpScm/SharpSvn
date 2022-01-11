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
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn;
using SharpSvn.Security;
using SharpSvn.Tests.LookCommands;
using System.IO.Compression;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::Resolve
    /// </summary>
    [TestClass]
    public class ResolveTests : TestBase
    {
        /// <summary>
        ///Attempts to resolve a conflicted file.
        /// </summary>
        [TestMethod]
        public void Resolve_ResolveFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.AnkhSvnCases);

            UnzipToFolder(Path.Combine(ProjectBase, "Zips/conflictwc.zip"), sbox.Wc);
            RawRelocate(sbox.Wc, new Uri("file:///tmp/repos/"), ReposUrl);
            Client.Upgrade(sbox.Wc);
            RenameAdminDirs(sbox.Wc);

            string filePath = Path.Combine(sbox.Wc, "Form.cs");

            this.Client.Resolved(filePath);

            Assert.That(this.GetSvnStatus(filePath), Is.EqualTo(SvnStatus.Modified), "Resolve didn't work!");
        }

        /// <summary>
        ///Attempts to resolve a conflicted directory recursively.
        /// </summary>
        [TestMethod]
        public void Resolve_ResolveDirectory()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.AnkhSvnCases);

            UnzipToFolder(Path.Combine(ProjectBase, "Zips/conflictwc.zip"), sbox.Wc);
            RawRelocate(sbox.Wc, new Uri("file:///tmp/repos/"), ReposUrl);
            Client.Upgrade(sbox.Wc);
            RenameAdminDirs(sbox.Wc);

            SvnResolveArgs a = new SvnResolveArgs();
            a.Depth = SvnDepth.Infinity;

            this.Client.Resolve(sbox.Wc, SvnAccept.Merged, a);

            Assert.That(this.GetSvnStatus(sbox.Wc), Is.EqualTo(SvnStatus.None),
                " Resolve didn't work! Directory still conflicted");
            Assert.That(this.GetSvnStatus(Path.Combine(sbox.Wc, "Form.cs")), Is.EqualTo(SvnStatus.Modified),
                "Resolve didn't work! File still conflicted");
        }

        [TestMethod]
        public void Resolve_RepeatedEventHookUp_SOC_411()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri projectRoot = new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn/trunk/scripts");

            using (var svnClient = NewSvnClient(false, false))
            {
                for (int i = 0; i < 3; i++)
                {
                    string workingcopy = sbox.GetTempDir();
                    Directory.CreateDirectory(workingcopy);

                    svnClient.Authentication.UserNamePasswordHandlers += DoNowt;
                    try
                    {
                        SvnUpdateResult svnUpdateResult;
                        SvnCheckOutArgs ca = new SvnCheckOutArgs() { Depth = SvnDepth.Files };
                        Assert.IsTrue(svnClient.CheckOut(projectRoot, workingcopy, ca, out svnUpdateResult));
                        Assert.IsNotNull(svnUpdateResult);
                        Assert.IsTrue(svnUpdateResult.HasRevision);
                    }
                    finally
                    {
                        svnClient.Authentication.UserNamePasswordHandlers -= DoNowt;
                    }
                }
            }
        }

        private void DoNowt(object sender, SvnUserNamePasswordEventArgs e)
        {
            Assert.Fail("Didn't expect to be called here, make sure your credentials are in the cache");
        }

        protected static void RawRelocate(string wc, Uri from, Uri to)
        {
            foreach (DirectoryInfo adm_dir in new DirectoryInfo(wc).GetDirectories(".svn", SearchOption.AllDirectories))
            {
                string entries = Path.Combine(adm_dir.FullName, "entries");
                string txt = File.ReadAllText(entries).Replace(from.AbsoluteUri.TrimEnd('/'), to.AbsoluteUri.TrimEnd('/'));

                File.SetAttributes(entries, FileAttributes.Normal);
                File.WriteAllText(entries, txt);
                File.SetAttributes(entries, FileAttributes.ReadOnly);
            }
        }

        protected static void UnzipToFolder(string zipFile, string toFolder)
        {
            ZipFile.ExtractToDirectory(zipFile, toFolder);
        }
    }

}
