using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

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
			if (svnClient == null)
				throw new ArgumentNullException("svnClient");

			SvnClientUIHandler handler = new SvnClientUIHandler(parentWindow);

			handler.Bind(svnClient);
		}
	}
}
