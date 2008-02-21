using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace SharpSvn.UI
{
	/// <summary>
	/// Helper methods to bind a user interface to an <see cref="SvnClient"/>
	/// </summary>
	public static class SharpSvnUI
	{
		/// <summary>
		/// Binds the default userinterface to the <see cref="SvnClient"/> using <paramref name="parentWindow"/> as parent for the popup windows
		/// </summary>
		/// <param name="svnClient"></param>
		/// <param name="parentWindow"></param>
		public static void Bind(SvnClient svnClient, IWin32Window parentWindow)
		{
            Bind(svnClient, parentWindow, null);
		}

        /// <summary>
        /// Binds the default userinterface to the <see cref="SvnClient"/> using <paramref name="parentWindow"/> as parent for the popup windows
        /// </summary>
        /// <param name="svnClient"></param>
        /// <param name="parentWindow"></param>
        /// <param name="headerImage"></param>
        public static void Bind(SvnClient svnClient, IWin32Window parentWindow, Image headerImage)
        {
            if (svnClient == null)
                throw new ArgumentNullException("svnClient");

            SvnClientUIHandler handler = new SvnClientUIHandler(parentWindow);

            handler.Image = headerImage;
            handler.Bind(svnClient);
        }
	}
}
