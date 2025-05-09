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
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn;
using SharpSvn.Security;
using SharpSvn.Tests.Commands;

namespace SharpSvn.Tests.Misc
{
    /// <summary>
    /// Tests the Authentication class.
    /// </summary>
    [TestClass]
    public class AuthenticationTests : TestBase
    {
        [TestInitialize]
        public void AuthSetup()
        {
            _serverTrustTicked = false;
            _userNamePasswordTicked = false;
            _userArgs = null;
        }

        [TestMethod]
        public void Authentication_TestSimpleProvider()
        {
            Client.Authentication.Clear();

            bool arrived = false;
            SvnLogArgs a = new SvnLogArgs();
            a.Limit = 1;
            //Assert.That(Client.Log(new Uri("svn://svn.tartarus.org/sgt/putty-0.60/misc.c"),
            //    delegate(object sender, SvnLogEventArgs e)
            //    {
            //        arrived = true;
            //    }));

            //Assert.That(arrived);

            arrived = false;
            Assert.That(Client.Info(new Uri("http://svn.apache.org/repos/asf/subversion/trunk/"),
                delegate(object sender, SvnInfoEventArgs e)
                {
                    arrived = true;
                }));

            arrived = false;
            Client.Authentication.SslServerTrustHandlers += new EventHandler<SvnSslServerTrustEventArgs>(Authenticator_SslServerTrustHandlersAllow);
            Assert.That(Client.List(new Uri("https://svn.apache.org/repos/asf/apr/"),
                delegate(object sender, SvnListEventArgs e)
                {
                    arrived = true;
                }));

            Assert.That(arrived);

            SvnClient cl2 = new SvnClient();
            cl2.Authentication.CopyAuthenticationCache(Client);
            cl2.Authentication.ClearAuthenticationCache();
        }

        bool _serverTrustTicked;
        bool _userNamePasswordTicked;
        SvnUserNamePasswordEventArgs _userArgs;

        [TestMethod]
        public void Authentication_SimpleSslCert()
        {
            using (SvnClient client = new SvnClient())
            {
                client.Authentication.Clear();
                client.Authentication.SslServerTrustHandlers += Authenticator_SslServerTrustHandlers;
                client.Authentication.UserNamePasswordHandlers += Authenticator_UserNamePasswordHandlers;
                bool arrived = false;
                SvnInfoArgs a = new SvnInfoArgs();
                a.ThrowOnCancel = false;
                a.ThrowOnError = false;

                Assert.That(client.Info(new Uri("https://svn.apache.org/repos/private/committers"), a,
                    delegate(object sender, SvnInfoEventArgs e)
                    {
                        arrived = true;
                    }), Is.False);

                Assert.That(a.LastException, Is.Not.Null);
                Assert.That(a.LastException, Is.InstanceOf(typeof(SvnException)));
                Assert.That(arrived, Is.False);
                Assert.That(_serverTrustTicked);
                Assert.That(_userNamePasswordTicked);

                Assert.That(_userArgs, Is.Not.Null);
                Assert.That(_userArgs.InitialUserName, Is.Not.Null);
                Assert.That(_userArgs.Realm, Is.EqualTo("<https://svn.apache.org:443> ASF Members"));
                Assert.That(_userArgs.RealmUri, Is.EqualTo(new Uri("https://svn.apache.org/")));
            }
        }

        [TestMethod]
        public void Authentication_ASFCertSafe()
        {
            using (SvnClient client = new SvnClient())
            {
                client.Authentication.Clear();

                bool arrived = false;
                SvnInfoArgs ia = new SvnInfoArgs();
                ia.AddExpectedError(SvnErrorCode.SVN_ERR_AUTHN_NO_PROVIDER);
                ia.AddExpectedError(SvnErrorCode.SVN_ERR_RA_CANNOT_CREATE_SESSION);
                Assert.That(client.Info(new Uri("https://svn.apache.org/repos/asf/"), ia,
                    delegate(object sender, SvnInfoEventArgs e)
                    {
                        arrived = true;
                    }), Is.False);

                Assert.That(arrived, Is.False);
                Assert.That(ia.LastException, Is.Not.Null, "Has exception");
                Assert.That(ia.LastException.ContainsError(SvnErrorCode.SVN_ERR_AUTHN_NO_PROVIDER), "Right error code in chain");
            }

            using (SvnClient client = new SvnClient())
            {
                client.Authentication.Clear();
                client.Authentication.SslAuthorityTrustHandlers += SvnAuthentication.SubversionWindowsSslAuthorityTrustHandler;
                client.Authentication.SslServerTrustHandlers += SvnAuthentication.SubversionWindowsSslServerTrustHandler;

                bool arrived = false;
                Assert.That(client.Info(new Uri("https://svn.apache.org/repos/asf/"),
                    delegate(object sender, SvnInfoEventArgs e)
                    {
                        arrived = true;
                    }));

                Assert.That(arrived);
            }
        }

        void Authenticator_UserNamePasswordHandlers(object sender, SharpSvn.Security.SvnUserNamePasswordEventArgs e)
        {
            GC.KeepAlive(e.InitialUserName);
            GC.KeepAlive(e.Realm);
            _userArgs = e;
            _userNamePasswordTicked = true;
            e.Cancel = true;
            e.Break = true;
        }

        void Authenticator_SslServerTrustHandlers(object sender, SharpSvn.Security.SvnSslServerTrustEventArgs e)
        {
            Assert.That(e.Break, Is.False);
            Assert.That(e.Cancel, Is.False);
            Assert.That(e.CommonName.EndsWith(".apache.org"));
            //Assert.That(DateTime.Parse(e.ValidFrom), Is.LessThan(DateTime.Now));
            //Assert.That(DateTime.Parse(e.ValidUntil), Is.GreaterThan(DateTime.Now));
            _serverTrustTicked = true;
            e.AcceptedFailures = e.Failures;
        }

        void Authenticator_SslServerTrustHandlersAllow(object sender, SharpSvn.Security.SvnSslServerTrustEventArgs e)
        {
            Assert.That(e.Break, Is.False);
            Assert.That(e.Cancel, Is.False);
            _serverTrustTicked = true;
            e.AcceptedFailures = e.Failures;
        }
    }
}
