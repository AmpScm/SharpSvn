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
using System.Drawing;
using System.Runtime.InteropServices;
using SharpSvn.UI.Properties;
using System.ComponentModel;
using System.Windows.Forms.Design;

namespace SharpSvn.UI
{
    sealed class SvnClientUIHandler
    {
        readonly IWin32Window _window;
        ISynchronizeInvoke _synchronizer;

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

        Image _bitmap;
        public Image Image
        {
            get { return _bitmap; }
            set { _bitmap = value; }
        }

        public ISynchronizeInvoke Synchronizer
        {
            get { return _synchronizer; }
            set { _synchronizer = value; }
        }

		IUIService _uiService;
		public IUIService UIService
		{
			get { return _uiService; }
			set { _uiService = value; }
		}


        internal void Bind(SvnClient svnClient, SvnUIBindArgs args)
        {
			Image = args.HeaderImage;
			UIService = args.UIService;
			Synchronizer = args.Synchronizer;			

            svnClient.Authentication.UserNameHandlers += new EventHandler<SharpSvn.Security.SvnUserNameEventArgs>(DialogUserNameHandler);
            svnClient.Authentication.UserNamePasswordHandlers += new EventHandler<SharpSvn.Security.SvnUserNamePasswordEventArgs>(DialogUserNamePasswordHandler);
            svnClient.Authentication.SslClientCertificateHandlers += new EventHandler<SharpSvn.Security.SvnSslClientCertificateEventArgs>(DialogSslClientCertificateHandler);
            svnClient.Authentication.SslClientCertificatePasswordHandlers += new EventHandler<SharpSvn.Security.SvnSslClientCertificatePasswordEventArgs>(DialogSslClientCertificatePasswordHandler);
            svnClient.Authentication.SslServerTrustHandlers += new EventHandler<SharpSvn.Security.SvnSslServerTrustEventArgs>(DialogSslServerTrustHandler);
        }

		bool Reinvoke<T>(object sender, T e, EventHandler<T> handler)
			where T : EventArgs
		{
			if (Synchronizer != null && Synchronizer.InvokeRequired)
			{
				Synchronizer.Invoke(handler, new object[] { sender, e });
				return true;
			}

			return false;
		}

		DialogResult RunDialog(Form dialog)
		{
			if (dialog == null)
				throw new ArgumentNullException("dialog");

			if (UIService != null)
				return UIService.ShowDialog(dialog);
			else if (_window != null)
				return dialog.ShowDialog(dialog);
			else
				return dialog.ShowDialog();
		}

        void DialogUserNameHandler(Object sender, SvnUserNameEventArgs e)
        {
			if (Reinvoke(sender, e, DialogUserNameHandler))
				return;            

            using (UsernameDialog dlg = new UsernameDialog())
            {
                dlg.Text = Strings.ConnectToSubversion;
                dlg.descriptionBox.Text = SharpSvnGui.MakeDescription(e.Realm, Strings.TheServerXatYRequiresAUsername);
                dlg.descriptionBox.Visible = true;
                dlg.rememberCheck.Enabled = e.MaySave;

                if (Image != null)
                    dlg.SetImage(Image);

                DialogResult r = RunDialog(dlg);

                if (r != DialogResult.OK)
                {
                    e.Cancel = e.Break = true;
                    return;
                }

                e.UserName = dlg.usernameBox.Text;
                e.Save = e.MaySave && dlg.rememberCheck.Checked;

            }
        }

        void DialogUserNamePasswordHandler(Object sender, SvnUserNamePasswordEventArgs e)
        {
			if (Reinvoke(sender, e, DialogUserNamePasswordHandler))
				return;            

            string description = SharpSvnGui.MakeDescription(e.Realm, Strings.TheServerXatYRequiresAUsernameAndPassword);
            if (Environment.OSVersion.Platform == PlatformID.Win32NT && Environment.OSVersion.Version >= new Version(5, 1))
            {
                // If Windows XP/Windows 2003 or higher: Use the windows password dialog
                GetUserNamePasswordWindows(e, description);

                return;
            }

            using (UsernamePasswordDialog dlg = new UsernamePasswordDialog())
            {
                dlg.Text = Strings.ConnectToSubversion;
                dlg.descriptionBox.Text = description;
                dlg.descriptionBox.Visible = true;
                dlg.rememberCheck.Enabled = e.MaySave;
                if (Image != null)
                    dlg.SetImage(Image);

                if (!string.IsNullOrEmpty(e.InitialUserName))
                {
                    dlg.usernameBox.Text = e.InitialUserName;
                    dlg.passwordBox.Select();
                }

				DialogResult r = RunDialog(dlg);

                if (r != DialogResult.OK)
                {
                    e.Save = dlg.rememberCheck.Checked;
                    e.UserName = e.Password = null;
                    e.Cancel = e.Break = true;
                    return;
                }

                e.UserName = dlg.usernameBox.Text;
                e.Password = dlg.passwordBox.Text;
                e.Save = e.MaySave && dlg.rememberCheck.Checked;
            }
        }

        private void GetUserNamePasswordWindows(SvnUserNamePasswordEventArgs e, string description)
        {
            NativeMethods.CREDUI_INFO info = new NativeMethods.CREDUI_INFO();
            info.pszCaptionText = Strings.ConnectToSubversion;
            info.pszMessageText = description;
            info.hwndParent = (_window != null) ? _window.Handle : IntPtr.Zero;
            info.cbSize = Marshal.SizeOf(typeof(NativeMethods.CREDUI_INFO));

            StringBuilder sbUserName = new StringBuilder(e.InitialUserName ?? "", 1024);
            StringBuilder sbPassword = new StringBuilder("", 1024);

            bool dlgSave = false;

            int flags = (int)NativeMethods.CREDUI_FLAGS.GENERIC_CREDENTIALS | (int)NativeMethods.CREDUI_FLAGS.ALWAYS_SHOW_UI | (int)NativeMethods.CREDUI_FLAGS.DO_NOT_PERSIST;
            if (e.MaySave)
                flags |= (int)NativeMethods.CREDUI_FLAGS.SHOW_SAVE_CHECK_BOX;

            switch (NativeMethods.CredUIPromptForCredentials(ref info, e.Realm, IntPtr.Zero, 0, sbUserName, 1024, sbPassword, 1024, ref dlgSave,
                (NativeMethods.CREDUI_FLAGS)flags))
            {
                case NativeMethods.CredUIReturnCodes.NO_ERROR:
                    e.UserName = sbUserName.ToString();
                    e.Password = sbPassword.ToString();
                    e.Save = e.MaySave && dlgSave;
                    break;
                case NativeMethods.CredUIReturnCodes.ERROR_CANCELLED:
                    e.UserName = null;
                    e.Password = null;
                    e.Save = false;
                    e.Cancel = e.Break = true;
                    break;
            }
        }

        void DialogSslServerTrustHandler(Object sender, SvnSslServerTrustEventArgs e)
        {
			if (Reinvoke(sender, e, DialogSslServerTrustHandler))
				return;

            using (SslServerCertificateTrustDialog dlg = new SslServerCertificateTrustDialog())
            {
                dlg.Text = Strings.ConnectToSubversion;
                if (Image != null)
                    dlg.SetImage(Image);

                dlg.hostname.Text = e.CommonName;
                dlg.fingerprint.Text = e.Fingerprint;
                dlg.validFrom.Text = e.ValidFrom;
                dlg.validTo.Text = e.ValidUntil;
                dlg.issuer.Text = e.Issuer;
                dlg.certificateBox.Text = e.CertificateValue;

                Bitmap Ok = Resources.Ok.ToBitmap();
                Bitmap Error = Resources.Error.ToBitmap();

                bool invalidCommonName = (0 != (int)(e.Failures & SvnCertificateTrustFailures.CommonNameMismatch));
                bool noTrustedIssuer = 0 != (int)(e.Failures & SvnCertificateTrustFailures.UnknownCertificateAuthority);
                bool timeError = 0 != (int)(e.Failures & (SvnCertificateTrustFailures.CertificateExpired | SvnCertificateTrustFailures.CertificateNotValidYet));

                dlg.caUnknownImage.Image = noTrustedIssuer ? Error : Ok;
                dlg.invalidDateImage.Image = timeError ? Error : Ok;
                dlg.serverMismatchImage.Image = invalidCommonName ? Error : Ok;

				DialogResult r = RunDialog(dlg);

                if (r == DialogResult.OK)
                {
                    e.Save = dlg.rememberCheck.Checked && e.MaySave;
                    e.AcceptedFailures = e.Failures;
                    return;
                }
                else
                    e.Cancel = e.Break = true;
            }
        }

        void DialogSslClientCertificateHandler(Object sender, SvnSslClientCertificateEventArgs e)
        {
			if (Reinvoke(sender, e, DialogSslClientCertificateHandler))
				return;

            using (SslClientCertificateFileDialog dlg = new SslClientCertificateFileDialog())
            {
                dlg.Text = Strings.ConnectToSubversion;
                dlg.descriptionBox.Text = SharpSvnGui.MakeDescription(e.Realm, Strings.ACertificateFileIsRequiredForAccessingServerXatY);
                dlg.descriptionBox.Visible = true;
                dlg.rememberCheck.Enabled = e.MaySave;

				DialogResult r = RunDialog(dlg);

                if (r != DialogResult.OK)
                {
                    e.Cancel = e.Break = true;
                    return;
                }

                e.CertificateFile = dlg.fileBox.Text;
                e.Save = e.MaySave && dlg.rememberCheck.Checked;
            }
        }

        void DialogSslClientCertificatePasswordHandler(Object sender, SvnSslClientCertificatePasswordEventArgs e)
        {
			if (Reinvoke(sender, e, DialogSslClientCertificatePasswordHandler))
				return;

            using (SslClientCertificatePassPhraseDialog dlg = new SslClientCertificatePassPhraseDialog())
            {
                dlg.Text = Strings.ConnectToSubversion;
                dlg.descriptionBox.Text = SharpSvnGui.MakeDescription(e.Realm, Strings.ThePassPhraseForTheClientCertificateForXIsRequired);
                dlg.descriptionBox.Visible = true;
                dlg.rememberCheck.Enabled = e.MaySave;

				DialogResult r = RunDialog(dlg);

                if (r != DialogResult.OK)
                {
                    e.Cancel = e.Break = true;
                    return;
                }

                e.Password = dlg.passPhraseBox.Text;
                e.Save = e.MaySave && dlg.rememberCheck.Checked;
            }
        }		
	}
}
