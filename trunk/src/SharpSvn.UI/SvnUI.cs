// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
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

		/// <summary>
		/// Binds the specified client to the user interface defined by args
		/// </summary>
		/// <param name="client">The client to bind.</param>
		/// <param name="args">The args.</param>
		public static void Bind(SvnRemoteSession client, SvnUIBindArgs args)
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
		public static void Bind(SvnRemoteSession client, IWin32Window parentWindow)
		{
			if (client == null)
				throw new ArgumentNullException("client");

			SvnUIBindArgs args = new SvnUIBindArgs();
			args.ParentWindow = parentWindow;

			Bind(client, args);
		}
	}
}
