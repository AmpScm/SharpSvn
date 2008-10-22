using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace SharpSvn.UI
{
	/// <summary>
	/// 
	/// </summary>
	public static class SvnUI
	{
		/// <summary>
		/// Binds the specified client to the user interface defined by args
		/// </summary>
		/// <param name="client">The client to bind.</param>
		/// <param name="args">The args.</param>
		public static void Bind(SvnClient client, SvnUIBindArgs args)
		{
			if (client == null)
				throw new ArgumentNullException("client");
			else if (args == null)
				throw new ArgumentNullException("args");

			SvnClientUIHandler handler = new SvnClientUIHandler(args.ParentWindow);

			handler.Bind(client, args);
		}

		/// <summary>
		/// Binds the specified client to the user interface defined by args
		/// </summary>
		/// <param name="client">The client to bind.</param>
        /// <param name="parentWindow">The parent window.</param>
		public static void Bind(SvnClient client, IWin32Window parentWindow)
		{
			if (client == null)
				throw new ArgumentNullException("client");

			SvnUIBindArgs args = new SvnUIBindArgs();
			args.ParentWindow = parentWindow;

			Bind(client, args);
		}
	}
}
