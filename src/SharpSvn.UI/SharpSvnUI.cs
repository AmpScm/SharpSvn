// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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
