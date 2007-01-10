using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

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

		public static bool AskUsername(IntPtr parentHandle, string title, string description, string realm, bool maySave, out string username, out bool save)
		{
			using (UsernameDialog dlg = new UsernameDialog())
			{
				dlg.Text = title;
				dlg.descriptionBox.Text = description;
				dlg.rememberCheck.Enabled = maySave;

				DialogResult r = (parentHandle != IntPtr.Zero) ? dlg.ShowDialog(new ParentProvider(parentHandle)) : dlg.ShowDialog();

				if (r != DialogResult.OK)
				{
					save = false;
					username = null;
					return false;
				}

				username = dlg.usernameBox.Text;
				save = maySave && dlg.rememberCheck.Checked;

				return true;
			}
		}

		public static bool AskUsernamePassword(IntPtr parentHandle, string title, string description, string realm, string initialUsername, bool maySave, out string username, out string password, out bool save)
		{
			using (UsernamePasswordDialog dlg = new UsernamePasswordDialog())
			{
				dlg.Text = title;
				dlg.descriptionBox.Text = description;
				dlg.rememberCheck.Enabled = maySave;
				if (!string.IsNullOrEmpty(initialUsername))
					dlg.usernameBox.Text = initialUsername;

				DialogResult r = (parentHandle != IntPtr.Zero) ? dlg.ShowDialog(new ParentProvider(parentHandle)) : dlg.ShowDialog();

				if (r != DialogResult.OK)
				{
					save = false;
					username = password = null;
					return false;
				}

				username = dlg.usernameBox.Text;
				password = dlg.passwordBox.Text;
				save = maySave && dlg.rememberCheck.Checked;

				return true;
			}
		}

		public static bool AskServerCertificateTrust(IntPtr parentHandle, string title, string description, string realm, out bool accept, out bool save)
		{
			accept = false;
			save = false;
			return false;
		}

		public static bool AskClientCertificateFile(IntPtr parentHandle, string title, string description, string realm, out string filename, out bool save)
		{
			filename = null;
			save = false;
			return false;
		}

		public static bool AskClientCertificatePassPhrase(IntPtr parentHandle, string title, string description, string realm, out string passPhrase, out bool save)
		{
			passPhrase = null;
			save = false;
			return false;
		}
	}
}
