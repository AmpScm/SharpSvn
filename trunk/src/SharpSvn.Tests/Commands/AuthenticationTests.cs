// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.IO;
using NUnit.Framework;

using SharpSvn;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn.Security;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the Authentication class.
	/// </summary>
	[TestFixture]
	public class AuthenticationTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractRepos();

			_serverTrustTicked = false;
			_userNamePasswordTicked = false;
			_userArgs = null;
		}

		[Test]
		public void TestSimpleProvider()
		{
			Client.Authenticator.Clear();

			bool arrived = false;
			SvnLogArgs a = new SvnLogArgs();
			a.Limit = 1;
			Assert.That(Client.Log(new Uri("svn://svn.tartarus.org/sgt/putty-0.60/misc.c"),
				delegate(object sender, SvnLogEventArgs e)
				{
					arrived = true;
				}));

			Assert.That(arrived);

			arrived = false;
			Assert.That(Client.Info(new Uri("http://sharpsvn.googlecode.com/svn/trunk/"),
				delegate(object sender, SvnInfoEventArgs e)
				{
					arrived = true;
				}));


			arrived = false;
			Client.Authenticator.SslServerTrustHandlers += new EventHandler<SvnSslServerTrustEventArgs>(Authenticator_SslServerTrustHandlersAllow);
			Assert.That(Client.List(new Uri("https://svn.apache.org/repos/asf/apr/"),
				delegate(object sender, SvnListEventArgs e)
				{
					arrived = true;
				}));

			Assert.That(arrived);
		}

		bool _serverTrustTicked;
		bool _userNamePasswordTicked;
		SvnUserNamePasswordEventArgs _userArgs;

		[Test]
		public void TestSimpleSslCert()
		{
			Client.Authenticator.Clear();
			Client.Authenticator.SslServerTrustHandlers += new EventHandler<SharpSvn.Security.SvnSslServerTrustEventArgs>(Authenticator_SslServerTrustHandlers);
			Client.Authenticator.UserNamePasswordHandlers += new EventHandler<SharpSvn.Security.SvnUserNamePasswordEventArgs>(Authenticator_UserNamePasswordHandlers);
			bool arrived = false;
			SvnInfoArgs a = new SvnInfoArgs();
			a.ThrowOnCancel = false;
			a.ThrowOnError = false;

			Assert.That(Client.Info(new Uri("https://sharpsvn.googlecode.com/svn/trunk/"), a,
				delegate(object sender, SvnInfoEventArgs e)
				{
					arrived = true;
				}), Is.False);

			Assert.That(a.LastException, Is.Not.Null);
			Assert.That(a.LastException, Is.InstanceOfType(typeof(SvnException)));
			Assert.That(a.LastException.RootCause, Is.InstanceOfType(typeof(SvnAuthorizationException)));
			Assert.That(arrived, Is.False);
			Assert.That(_serverTrustTicked);
			Assert.That(_userNamePasswordTicked);

			Assert.That(_userArgs, Is.Not.Null);
			Assert.That(_userArgs.InitialUserName, Is.Not.Null);
			Assert.That(_userArgs.Realm.Contains("Google Code Subversion"));
			Assert.That(_userArgs.RealmUri, Is.EqualTo(new Uri("https://sharpsvn.googlecode.com/")));
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
			Assert.That(e.CommonName, Is.EqualTo("*.googlecode.com"));
			Assert.That(DateTime.Parse(e.ValidFrom), Is.LessThan(DateTime.Now));
			Assert.That(DateTime.Parse(e.ValidUntil), Is.GreaterThan(DateTime.Now));
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
