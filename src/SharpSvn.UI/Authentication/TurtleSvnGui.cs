using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Runtime.InteropServices;
using System.Drawing;
using SharpSvn.UI.Properties;

namespace SharpSvn.UI.Authentication
{
	/// <summary>
	/// Public api used by TurtelSvn
	/// </summary>
	public static class TurtleSvnGui
	{
		class ParentProvider : System.Windows.Forms.IWin32Window
		{
			readonly IntPtr _handle;

			public ParentProvider(IntPtr handle)
			{
				_handle = handle;
			}

			public IntPtr Handle
			{
				get { return _handle; }
			}
		}

		static Regex _realmRegex;
		static string MakeDescription(string realm, string format)
		{
			if(null == _realmRegex)
				_realmRegex = new Regex("^\\<(?<server>[a-z]+://[^ >]+)\\> (?<realm>.*)$", RegexOptions.ExplicitCapture | RegexOptions.Singleline);

			Match m = _realmRegex.Match(realm);
			Uri uri;
			if (m.Success && Uri.TryCreate(m.Groups[1].Value, UriKind.Absolute, out uri))
			{
				return string.Format(format, uri, m.Groups[2].Value);
			}
			else
				return string.Format(format, "", realm);
		}

		public static bool AskUsername(IntPtr parentHandle, string title, string realm, bool maySave, out string username, out bool save)
		{
			using (UsernameDialog dlg = new UsernameDialog())
			{
				dlg.Text = title;
				dlg.descriptionBox.Text = MakeDescription(realm, Strings.TheServerXatYRequiresAUsername);
				dlg.descriptionBox.Visible = true;
				dlg.rememberCheck.Enabled = maySave;

				DialogResult r = (parentHandle != IntPtr.Zero) ? dlg.ShowDialog(new ParentProvider(parentHandle)) : dlg.ShowDialog();

				if (r != DialogResult.OK)
				{
					save = dlg.rememberCheck.Checked;
					username = null;
					return false;
				}

				username = dlg.usernameBox.Text;
				save = maySave && dlg.rememberCheck.Checked;

				return true;
			}
		}

		public static bool AskUsernamePassword(IntPtr parentHandle, string title, string realm, string initialUsername, bool maySave, out string username, out string password, out bool save)
		{
			string description = MakeDescription(realm, Strings.TheServerXatYRequiresAUsernameAndPassword);
			if (Environment.OSVersion.Platform == PlatformID.Win32NT && Environment.OSVersion.Version >= new Version(5, 1))
			{
				// If Windows XP/Windows 2003 or higher: Use the windows password dialog
				NativeMethods.CREDUI_INFO info = new NativeMethods.CREDUI_INFO();
				info.pszCaptionText = title;
				info.pszMessageText = description;
				info.hwndParent = parentHandle;
				info.cbSize = Marshal.SizeOf(typeof(NativeMethods.CREDUI_INFO));

				StringBuilder sbUsername = new StringBuilder(initialUsername ?? "", 1024);
				StringBuilder sbPassword = new StringBuilder("", 1024);

				bool dlgSave = false;

				int flags = (int)NativeMethods.CREDUI_FLAGS.GENERIC_CREDENTIALS;
				if(!maySave)
					flags |= (int)NativeMethods.CREDUI_FLAGS.DO_NOT_PERSIST;

				switch (NativeMethods.CredUIPromptForCredentials(ref info, realm, IntPtr.Zero, 0, sbUsername, 1024, sbPassword, 1024, ref dlgSave,
					(NativeMethods.CREDUI_FLAGS)flags))
				{
					case NativeMethods.CredUIReturnCodes.NO_ERROR:
						username = sbUsername.ToString();
						password = sbPassword.ToString();
						save = maySave && dlgSave;
						return true;
					case NativeMethods.CredUIReturnCodes.ERROR_CANCELLED:
						username = null;
						password = null;
						save = false;
						return false;

				}
			}

			using (UsernamePasswordDialog dlg = new UsernamePasswordDialog())
			{
				dlg.Text = title;
				dlg.descriptionBox.Text = description;
				dlg.descriptionBox.Visible = true;
				dlg.rememberCheck.Enabled = maySave;
				if (!string.IsNullOrEmpty(initialUsername))
				{
					dlg.usernameBox.Text = initialUsername;
					dlg.passwordBox.Select();
				}

				DialogResult r = (parentHandle != IntPtr.Zero) ? dlg.ShowDialog(new ParentProvider(parentHandle)) : dlg.ShowDialog();

				if (r != DialogResult.OK)
				{
					save = dlg.rememberCheck.Checked;
					username = password = null;
					return false;
				}

				username = dlg.usernameBox.Text;
				password = dlg.passwordBox.Text;
				save = maySave && dlg.rememberCheck.Checked;

				return true;
			}
		}

		public class ServerCertificateInfo
		{
			string _hostname;

			public string Hostname
			{
				get { return _hostname; }
				set { _hostname = value; }
			}
			string _fingerprint;

			public string Fingerprint
			{
				get { return _fingerprint; }
				set { _fingerprint = value; }
			}
			string _validFrom;

			public string ValidFrom
			{
				get { return _validFrom; }
				set { _validFrom = value; }
			}
			string _validTo;

			public string ValidTo
			{
				get { return _validTo; }
				set { _validTo = value; }
			}
			string _issuer;

			public string Issuer
			{
				get { return _issuer; }
				set { _issuer = value; }
			}
			string _certificate;

			public string Certificate
			{
				get { return _certificate; }
				set { _certificate = value; }
			}

			bool _noTrustedIssuer;

			public bool NoTrustedIssuer
			{
				get { return _noTrustedIssuer; }
				set { _noTrustedIssuer = value; }
			}
			bool _timeError;

			public bool TimeError
			{
				get { return _timeError; }
				set { _timeError = value; }
			}
			bool _invalidCommonName;

			public bool InvalidCommonName
			{
				get { return _invalidCommonName; }
				set { _invalidCommonName = value; }
			}
		}

		public static bool AskServerCertificateTrust(IntPtr parentHandle, string title, string realm, ServerCertificateInfo info, bool maySave, out bool accept, out bool save)
		{
			using (SslServerCertificateTrustDialog dlg = new SslServerCertificateTrustDialog())
			{
				dlg.Text = title ;

				dlg.hostname.Text = info.Hostname;
				dlg.fingerprint.Text = info.Fingerprint;
				dlg.validFrom.Text = info.ValidFrom;
				dlg.validTo.Text = info.ValidTo;
				dlg.issuer.Text = info.Issuer;
				dlg.certificateBox.Text = info.Certificate;

				Bitmap Ok = Resources.Ok.ToBitmap();
				Bitmap Error = Resources.Error.ToBitmap();

				dlg.caUnknownImage.Image = info.NoTrustedIssuer ? Error : Ok;
				dlg.invalidDateImage.Image = info.TimeError ? Error : Ok;
				dlg.serverMismatchImage.Image = info.InvalidCommonName ? Error : Ok;

				DialogResult r = (parentHandle != IntPtr.Zero) ? dlg.ShowDialog(new ParentProvider(parentHandle)) : dlg.ShowDialog();

				if (r == DialogResult.OK)
				{
					save = dlg.rememberCheck.Checked;
					accept = true;
					return true;
				}
				accept = false;
				save = false;
				return false;
			}
		}

		public static bool AskClientCertificateFile(IntPtr parentHandle, string title, string realm, bool maySave, out string filename, out bool save)
		{
			filename = null;
			save = false;
			return false;
		}

		public static bool AskClientCertificatePassPhrase(IntPtr parentHandle, string title, string realm, bool maySave, out string passPhrase, out bool save)
		{
			using (SslClientCertificatePassPhraseDialog dlg = new SslClientCertificatePassPhraseDialog())
			{
				dlg.Text = title;
				dlg.descriptionBox.Text = MakeDescription(realm, Strings.ThePassPhraseForTheClientCertificateForXIsRequired);
				dlg.descriptionBox.Visible = true;
				dlg.rememberCheck.Enabled = maySave;

				DialogResult r = (parentHandle != IntPtr.Zero) ? dlg.ShowDialog(new ParentProvider(parentHandle)) : dlg.ShowDialog();

				if (r != DialogResult.OK)
				{
					save = false;
					passPhrase = null;
					return false;
				}

				passPhrase = dlg.passPhraseBox.Text;
				save = maySave && dlg.rememberCheck.Checked;

				return true;
			}
		}
	}
}
