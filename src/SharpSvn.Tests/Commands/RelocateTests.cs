﻿//
// Copyright 2008-2025 The SharpSvn Project
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
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// A test for Client.Relocate
    /// </summary>
    [TestClass]
    public class RelocateTests : TestBase
    {
        [TestMethod]
        public void Relocate_SvnServeRelocate()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default, false);

            // start a svnserve process on this repos
            Process svnserve = this.StartSvnServe(sbox.RepositoryUri.AbsolutePath);

            try
            {
                Uri localUri = new Uri(String.Format("svn://127.0.0.1:{0}/", PortNumber));

                bool svnServeAvailable = false;
                for (int i = 0; i < 10; i++)
                {
                    SvnInfoArgs ia = new SvnInfoArgs();
                    ia.ThrowOnError = false;

                    // This test also checks whether "svn://127.0.0.1:{0}/" is correctly canonicalized to "svn://127.0.0.1:{0}"
                    Client.Info(localUri, ia,
                        delegate(object sender, SvnInfoEventArgs e)
                        {
                            svnServeAvailable = true;
                        });

                    if (svnServeAvailable)
                        break;
                    Thread.Sleep(100);
                }

                Assert.That(svnServeAvailable);

                Assert.That(Client.Relocate(sbox.Wc, sbox.RepositoryUri, localUri));

                Collection<SvnInfoEventArgs> list;
                SvnInfoArgs a = new SvnInfoArgs();

                Assert.That(Client.GetInfo(sbox.Wc, a, out list));

                Assert.That(list.Count, Is.GreaterThan(0));
                Assert.That(list[0].Uri.ToString().StartsWith(localUri.ToString()));
            }
            finally
            {
                System.Threading.Thread.Sleep(100);
                if (!svnserve.HasExited)
                {
                    svnserve.Kill();
                    svnserve.WaitForExit();
                }
            }
        }


        /// <summary>
        /// Starts a svnserve instance.
        /// </summary>
        /// <param name="root">The root directory to use for svnserve.</param>
        /// <returns></returns>
        protected Process StartSvnServe(string root)
        {
            ProcessStartInfo psi = new ProcessStartInfo(Path.GetFullPath(Path.Combine(ProjectBase, "..\\..\\imports\\release\\bin\\svnserve.exe")),
                String.Format("--daemon --root {0} --listen-host 127.0.0.1 --listen-port {1}", root,
                PortNumber));

            psi.CreateNoWindow = true;
            psi.UseShellExecute = false;

            return Process.Start(psi);
        }

        protected static readonly int PortNumber = 7777 + new Random().Next(5000);
    }
}
