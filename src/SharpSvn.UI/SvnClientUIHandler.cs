// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using SharpSvn.Security;
using SharpSvn.UI.Authentication;

namespace SharpSvn.UI
{
	sealed class SvnClientUIHandler
	{
		readonly IWin32Window _window;

		sealed class ZeroWindowHandle : IWin32Window
		{
			public IntPtr Handle
			{
				get { return IntPtr.Zero; }
			}
		}

		public SvnClientUIHandler(IWin32Window window)
		{
			_window = window ?? new ZeroWindowHandle();
		}

		internal void Bind(SvnClient svnClient)
		{
			svnClient.Authenticator.UsernameHandlers += new EventHandler<SharpSvn.Security.SvnUsernameEventArgs>(DialogUsernameHandler);
			svnClient.Authenticator.UsernamePasswordHandlers += new EventHandler<SharpSvn.Security.SvnUsernamePasswordEventArgs>(DialogUsernamePasswordHandler);
			svnClient.Authenticator.SslClientCertificateHandlers += new EventHandler<SharpSvn.Security.SvnSslClientCertificateEventArgs>(DialogSslClientCertificateHandler);
			svnClient.Authenticator.SslClientCertificatePasswordHandlers += new EventHandler<SharpSvn.Security.SvnSslClientCertificatePasswordEventArgs>(DialogSslClientCertificatePasswordHandler);
			svnClient.Authenticator.SslServerTrustHandlers += new EventHandler<SharpSvn.Security.SvnSslServerTrustEventArgs>(DialogSslServerTrustHandler);
		}

		void DialogUsernameHandler(Object sender, SvnUsernameEventArgs e)
		{
			String username;
			bool save;

			if (SharpSvnGui.AskUsername(_window.Handle, "Connect to Subversion", e.Realm, e.MaySave, out username, out save))
			{
				e.Username = username;
				e.Save = save && e.MaySave;
			}
			else
				e.Break = true;
		}

		void DialogUsernamePasswordHandler(Object sender, SvnUsernamePasswordEventArgs e)
		{
			String username;
			String password;
			bool save;

			if (SharpSvnGui.AskUsernamePassword(_window.Handle, "Connect to Subversion", e.Realm, e.InitialUsername, e.MaySave, out username, out password, out save))
			{
				e.Username = username;
				e.Password = password;
				e.Save = save && e.MaySave;
			}
			else
				e.Break = true;
		}

		void DialogSslServerTrustHandler(Object sender, SvnSslServerTrustEventArgs e)
		{
			bool save;

			ServerCertificateInfo sci = new ServerCertificateInfo();

			sci.InvalidCommonName = (0 != (int)(e.Failures & SvnCertificateTrustFailures.CommonNameMismatch));
			sci.NoTrustedIssuer = 0 != (int)(e.Failures & SvnCertificateTrustFailures.UnknownCertificateAuthority);
			sci.TimeError = 0 != (int)(e.Failures & (SvnCertificateTrustFailures.CertificateExpired | SvnCertificateTrustFailures.CertificateNotValidYet));

			sci.Hostname = e.CommonName;
			sci.Fingerprint = e.FingerPrint;
			sci.Certificate = e.CertificateValue;
			sci.Issuer = e.Issuer;
			sci.ValidFrom = e.ValidFrom;
			sci.ValidTo = e.ValidUntil;

			bool accept = false;
			if (SharpSvnGui.AskServerCertificateTrust(_window.Handle, "Connect to Subversion", e.Realm, sci, e.MaySave, out accept, out save))
			{
				e.AcceptedFailures = accept ? e.Failures : SvnCertificateTrustFailures.None;
				e.Save = save && e.MaySave;
			}
			else
				e.Break = true;
		}

		void DialogSslClientCertificateHandler(Object sender, SvnSslClientCertificateEventArgs e)
		{
			String file;
			bool save;

			if (SharpSvnGui.AskClientCertificateFile(_window.Handle, "Connect to Subversion", e.Realm, e.MaySave, out file, out save))
			{
				e.CertificateFile = file;
				e.Save = save && e.MaySave;
			}
			else
				e.Break = true;
		}

		void DialogSslClientCertificatePasswordHandler(Object sender, SvnSslClientCertificatePasswordEventArgs e)
		{
			String password;
			bool save;

			if (SharpSvnGui.AskClientCertificatePassPhrase(_window.Handle, "Connect to Subversion", e.Realm, e.MaySave, out password, out save))
			{
				e.Password = password;
				e.Save = save && e.MaySave;
			}
			else
				e.Break = true;
		}
	}
}
