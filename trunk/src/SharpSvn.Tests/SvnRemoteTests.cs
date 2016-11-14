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
using SharpSvn.Remote;
using SharpSvn.UI;

namespace SharpSvn.Tests
{
    [TestClass]
    public class SvnRemoteTests
    {
        [TestMethod]
        public void RemoteList()
        {
            SvnClient cl = new SvnClient();
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
                cl.Info(new Uri("svn+builtin-ssh://sharpsvn.open.collab.net:80"),
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

        [TestMethod]
        public void AAAAAA()
        {
            string id = Guid.NewGuid().ToString();
            Uri uri_bad = new Uri(string.Format("svn+ssh://Z-{0}.vipp.alh.net.qqn.nl/hoqme/sqvt", id));
            Uri uri = new Uri(string.Format("svn+ssh://Z-{0}.vipp.alh.net.qqn.nl/home/svt", id));

            SvnClient svn = new SvnClient();
            svn.Configuration.LogMessageRequired = false;
            svn.Configuration.SshOverride = Implementation.SvnSshOverride.ForceInternalAfterConfig;

            svn.Authentication.SshServerTrustHandlers += delegate (object sender, Security.SvnSshServerTrustEventArgs e)
            {
                e.AcceptedFailures = e.Failures;
                Assert.That(e.Save, Is.False);
            };

            if (true)
            {
                svn.Authentication.UserNameHandlers += delegate (object sender, Security.SvnUserNameEventArgs e)
                {
                    e.UserName = "svq";
                    Assert.That(e.MaySave, Is.True);
                    Assert.That(e.Save, Is.False);
                };

                svn.Authentication.UserNamePasswordHandlers += delegate (object sender, Security.SvnUserNamePasswordEventArgs e)
                {
                    e.UserName = "svt";
                    e.Password = "svnsvn";
                    Assert.That(e.MaySave, Is.True);
                    //e.Save = true;
                    Assert.That(e.Save, Is.False);
                };
            }
            else
            {
                SvnUI.Bind(svn, new SvnUIBindArgs());
            }

            svn.RepositoryOperation(uri,
                delegate (SvnMultiCommandClient mucc)
                {
                    mucc.SetProperty("", "A", Guid.NewGuid().ToString());
                });

            svn = new SvnClient();
            svn.Configuration.LogMessageRequired = false;
            svn.Configuration.SshOverride = Implementation.SvnSshOverride.ForceInternalAfterConfig;

            svn.RepositoryOperation(uri,
                delegate (SvnMultiCommandClient mucc)
                {
                    mucc.SetProperty("", "A", Guid.NewGuid().ToString());
                });

            svn = new SvnClient();
            svn.Configuration.LogMessageRequired = false;
            svn.Configuration.SshOverride = Implementation.SvnSshOverride.ForceInternalAfterConfig;
            SvnUI.Bind(svn, new SvnUIBindArgs());

            svn.RepositoryOperation(uri,
                delegate (SvnMultiCommandClient mucc)
                {
                    mucc.SetProperty("", "A", Guid.NewGuid().ToString());
                });
        }
    }
}
