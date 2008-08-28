using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace SharpSvn.UI
{	
	/// <summary>
	/// Helper methods to bind a user interface to an <see cref="SvnClient"/>
	/// </summary>
	[Obsolete("SharpSvn cleanup: Please use the Bind method on the SvnUI class instead")]
	public static class SharpSvnUI
	{
		/// <summary>
		/// Binds the default userinterface to the <see cref="SvnClient"/> using <paramref name="parentWindow"/> as parent for the popup windows
		/// </summary>
		/// <param name="client">The client to bind.</param>
		/// <param name="parentWindow">The parent window.</param>
		public static void Bind(SvnClient client, IWin32Window parentWindow)
		{
            Bind(client, parentWindow, null);
		}

		/// <summary>
		/// Binds the default userinterface to the <see cref="SvnClient"/> using <paramref name="parentWindow"/> as parent for the popup windows
		/// </summary>
		/// <param name="client">The client.</param>
		/// <param name="parentWindow">The parent window.</param>
		/// <param name="headerImage">The header image.</param>
        public static void Bind(SvnClient client, IWin32Window parentWindow, Image headerImage)
        {
            Bind(client, parentWindow, null, headerImage);
        }

		/// <summary>
		/// Binds the default userinterface to the <see cref="SvnClient"/> using <paramref name="parentWindow"/> as parent for the popup windows
		/// </summary>
		/// <param name="client">The client.</param>
		/// <param name="parentWindow">The parent window.</param>
		/// <param name="synchronizer">The synchronizer.</param>
		/// <param name="headerImage">The header image.</param>
		[Obsolete("Please use Bind(SvnClient, SharpSvnUIBindArgs)")]
        public static void Bind(SvnClient client, IWin32Window parentWindow, ISynchronizeInvoke synchronizer, Image headerImage)
        {
            if (client == null)
                throw new ArgumentNullException("svnClient");

			SvnUIBindArgs args = new SvnUIBindArgs();
			args.ParentWindow = parentWindow;
			args.Synchronizer = synchronizer;
			args.HeaderImage = headerImage;

			SvnClientUIHandler handler = new SvnClientUIHandler(args.ParentWindow);
            handler.Bind(client, args);
        }		
	}
}
