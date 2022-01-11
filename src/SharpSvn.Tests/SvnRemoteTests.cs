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

using System.Collections.ObjectModel;
using SharpSvn.Tests.Commands;

namespace SharpSvn.Tests
{
    [TestClass]
    public class SvnRemoteTests : TestBase
    {
        [TestMethod]
        public void RemoteList()
        {
            SvnClient cl = NewSvnClient(false, false);
            bool found = false;
        SvnListArgs la = new SvnListArgs();
        la.RetrieveEntries = SvnDirEntryItems.AllFieldsV15;

                    cl.List(new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn/trunk"), la, delegate(object Sender, SvnListEventArgs e)
                    {
                            Assert.That(e.Entry, Is.Not.Null);
                            Assert.That(e.Entry.Revision, Is.GreaterThan(0L));
                            Assert.That(e.Entry.Author, Is.Not.Null);
                            found = true;
                    });

                    Assert.That(found);

                    Collection<SvnListEventArgs> ee;
        cl.GetList(new Uri("https://ctf.open.collab.net/svn/repos/sharpsvn/trunk"), out ee);
                    Assert.That(ee, Is.Not.Null);
                    Assert.That(ee[0].Entry.Author, Is.Not.Null);
        }

        [TestMethod,Ignore]
        public void TestSsh()
        {
            SvnClient cl = new SvnClient();
            bool found = false;

            //cl.KeepSession = true;

            cl.Authentication.SshServerTrustHandlers += delegate(object sender, Security.SvnSshServerTrustEventArgs e)
                    {
                        e.AcceptedFailures = e.Failures;
                    };

            cl.Authentication.UserNameHandlers += delegate(object sender, Security.SvnUserNameEventArgs e)
                    {
                        e.UserName = "bert";
                        e.Save = true;
                    };
            cl.List(new Uri("svn+libssh2://vip/home/svn/repos/ankh-test"), delegate(object Sender, SvnListEventArgs e)
            {
                Assert.That(e.Entry, Is.Not.Null);
                Assert.That(e.Entry.Revision, Is.GreaterThan(0L));
                Assert.That(e.Entry.Author, Is.Not.Null);
                found = true;
            });

            Assert.That(found);

            found = false;

            cl.List(new Uri("svn+libssh2://bert@vip/home/svn/repos/ankh-test"), delegate(object Sender, SvnListEventArgs e)
            {
                Assert.That(e.Entry, Is.Not.Null);
                Assert.That(e.Entry.Revision, Is.GreaterThan(0L));
                Assert.That(e.Entry.Author, Is.Not.Null);
                found = true;
            });

            Assert.That(found);
        }

        void Authentication_SshServerTrustHandlers(object sender, Security.SvnSshServerTrustEventArgs e)
        {
            e.AcceptedFailures = e.Failures;
        }

        [TestMethod]
        public void TestSshConnectError()
        {
            SvnClient cl = new SvnClient();
            SvnRepositoryIOException rio = null;
            try
            {
                cl.Info(new Uri("svn+builtin-ssh://github.com:80"),
                    delegate(object Sender, SvnInfoEventArgs e)
                    { });
            }
            catch (SvnRepositoryIOException e)
            {
                rio = e;
            }

            Assert.That(rio, Is.Not.Null);

            SvnSshException sshEx = rio.GetCause<SvnSshException>();

            Assert.That(sshEx, Is.Not.Null);
            Assert.That(sshEx.SshErrorCode, Is.EqualTo(SvnSshErrorCode.LIBSSH2_ERROR_SOCKET_DISCONNECT));
        }
    }
}
