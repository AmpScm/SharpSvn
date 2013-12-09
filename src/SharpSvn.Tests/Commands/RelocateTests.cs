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
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;
using NUnit.Framework;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// A test for Client.Relocate
    /// </summary>
    [TestFixture]
    public class RelocateTests : TestBase
    {
    public RelocateTests()
    {
        UseEmptyRepositoryForWc = false;
    }

        [Test]
        public void SvnServeRelocate()
        {
            // start a svnserve process on this repos
            Process svnserve = this.StartSvnServe(this.ReposPath.Replace('\\', '/'));

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
                    Thread.Sleep(3000);
                }

                Assert.That(svnServeAvailable);

                Assert.That(Client.Relocate(this.WcPath, ReposUrl, localUri));

                Collection<SvnInfoEventArgs> list;
                SvnInfoArgs a = new SvnInfoArgs();

                Assert.That(Client.GetInfo(WcPath, a, out list));

                Assert.That(list.Count, Is.GreaterThan(0));
                Assert.That(list[0].Uri.ToString().StartsWith(localUri.ToString()));
            }
            finally
            {
                System.Threading.Thread.Sleep(500);
                if (!svnserve.HasExited)
                {
                    svnserve.Kill();
                    svnserve.WaitForExit();
                }
            }
        }


    }
}
