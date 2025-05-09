﻿//
// Copyright 2007-2025 The SharpSvn Project
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

        Size _autoScaleBase;
        public Size AutoScaleBaseSize
        {
            get { return _autoScaleBase; }
            set { _autoScaleBase = value; }
        }

        internal void Bind(SvnClientContext svnClient, SvnUIBindArgs args)
        {
            Image = args.HeaderImage;
            UIService = args.UIService;
            Synchronizer = args.Synchronizer;

            if (!args.AutoScaleBaseSize.IsEmpty)
                AutoScaleBaseSize = args.AutoScaleBaseSize;

            svnClient.Authentication.UserNameHandlers += DialogUserNameHandler;
            svnClient.Authentication.UserNamePasswordHandlers += DialogUserNamePasswordHandler;
            svnClient.Authentication.SslClientCertificateHandlers += DialogSslClientCertificateHandler;
            svnClient.Authentication.SslClientCertificatePasswordHandlers += DialogSslClientCertificatePasswordHandler;
            svnClient.Authentication.SslServerTrustHandlers += DialogSslServerTrustHandler;
            svnClient.Authentication.SshServerTrustHandlers += DialogSshServerTrustHandlers;
            svnClient.Authentication.BeforeEngineDialog += new EventHandler<SharpSvn.Security.SvnBeforeEngineDialogEventArgs>(BeforeEngineDialog);
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

            if (!AutoScaleBaseSize.IsEmpty)
                dialog.AutoScaleBaseSize = AutoScaleBaseSize;

            if (UIService != null)
                return UIService.ShowDialog(dialog);
            else if (_window != null)
                return dialog.ShowDialog(_window);
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
                dlg.descriptionBox.Text = SharpSvnGui.MakeDescription(e.Realm, Strings.TheServerXatYRequiresAUsername, Strings.TheServerXRequiresAUsername);
                dlg.descriptionBox.Visible = true;

                dlg.usernameBox.Text = e.InitialUserName ?? e.UserName ?? "";
                dlg.rememberCheck.Enabled = e.MaySave;

                if (Image != null)
                    dlg.SetImage(Image);

                DialogResult r = RunDialog(dlg);

                if (r != DialogResult.OK)
                {
                    e.Break = true;
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

            string description = SharpSvnGui.MakeDescription(e.Realm, Strings.TheServerXatYRequiresAUsernameAndPassword, Strings.TheServerXRequiresAUsernameAndPassword);
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
                    e.Break = true;
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
                    e.Break = true;
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
                    e.Cancel = true; // Abort setup
            }
        }

        void DialogSshServerTrustHandlers(object sender, SvnSshServerTrustEventArgs e)
        {
            if (Reinvoke(sender, e, DialogSshServerTrustHandlers))
                return;

            StringBuilder sb = new StringBuilder();

            switch (e.KeyType)
            {
                case SvnSshServerKeyType.Dss:
                    sb.Append("ssh-dds ");
                    break;
                case SvnSshServerKeyType.Rsa:
                    sb.Append("ssh-rsa ");
                    break;
                default:
                    sb.Append(e.KeyType);
                    sb.Append(" ");
                    break;
            }
            if (e.KeyBits >= 0)
                sb.AppendFormat("{0} ", e.KeyBits);

            for (int i = 0; i < e.Fingerprint.Length; i++)
            {
                sb.Append(e.Fingerprint[i]);
                if (i % 2 == 1)
                    sb.Append(':');
            }

            string caption, txt;
            MessageBoxDefaultButton defaultButton = MessageBoxDefaultButton.Button2;
            MessageBoxIcon icon = MessageBoxIcon.Warning;

            if (0 != (e.Failures & SvnSshTrustFailures.ServerKeyMismatch))
            {
                caption = Strings.SshCertificateMismatchCaption;
                txt = string.Format(Strings.SshCertificateMismatch, e.KeyType, sb);
                defaultButton = MessageBoxDefaultButton.Button3; // Cancel
            }
            else
            {
                caption = Strings.SshCertificateUnknownCaption;
                txt = string.Format(Strings.SshCertificateUnknown, e.KeyType, sb);
                defaultButton = MessageBoxDefaultButton.Button2; // No
            }

            DialogResult dr;

            if (_window != null)
                dr = MessageBox.Show(_window, txt, caption, MessageBoxButtons.YesNoCancel, icon, defaultButton);
            else
                dr = MessageBox.Show(txt, caption, MessageBoxButtons.YesNoCancel, icon, defaultButton);

            switch (dr)
            {
                case DialogResult.Yes:
                    e.AcceptedFailures = e.Failures;
                    e.Save = e.MaySave;
                    break;
                case DialogResult.No:
                    e.AcceptedFailures = e.Failures;
                    e.Save = false;
                    break;
                case DialogResult.Cancel:
                default:
                    e.Cancel = true;
                    break;
            }
        }

        void DialogSslClientCertificateHandler(Object sender, SvnSslClientCertificateEventArgs e)
        {
            if (Reinvoke(sender, e, DialogSslClientCertificateHandler))
                return;

            using (SslClientCertificateFileDialog dlg = new SslClientCertificateFileDialog())
            {
                dlg.Text = Strings.ConnectToSubversion;
                dlg.descriptionBox.Text = SharpSvnGui.MakeDescription(e.Realm, Strings.ACertificateFileIsRequiredForAccessingServerXatY, Strings.ACertificateFileIsRequiredForAccessingServerX);
                dlg.descriptionBox.Visible = true;
                dlg.rememberCheck.Enabled = e.MaySave;

                DialogResult r = RunDialog(dlg);

                if (r != DialogResult.OK)
                {
                    e.Break = true;
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
                dlg.descriptionBox.Text = SharpSvnGui.MakeDescription(e.Realm, Strings.ThePassPhraseForTheClientCertificateForXatYIsRequired, Strings.ThePassPhraseForTheClientCertificateForXIsRequired);
                dlg.descriptionBox.Visible = true;
                dlg.rememberCheck.Enabled = e.MaySave;

                DialogResult r = RunDialog(dlg);

                if (r != DialogResult.OK)
                {
                    e.Break = true;
                    return;
                }

                e.Password = dlg.passPhraseBox.Text;
                e.Save = e.MaySave && dlg.rememberCheck.Checked;
            }
        }

        void BeforeEngineDialog(Object sender, SvnBeforeEngineDialogEventArgs e)
        {
            if (Reinvoke(sender, e, BeforeEngineDialog))
                return;

            IWin32Window wnd = null;
            if (UIService != null)
                wnd = UIService.GetDialogOwnerWindow();
            else
                wnd = _window;

            if (wnd != null)
                e.Handle = wnd.Handle;
        }
    }
}
