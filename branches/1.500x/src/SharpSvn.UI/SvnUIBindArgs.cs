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
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms.Design;

namespace SharpSvn.UI
{
	/// <summary>
	/// Container for <see cref="SvnUI.Bind(SvnClient, SvnUIBindArgs)"/>
	/// </summary>
	public class SvnUIBindArgs
	{
		IWin32Window _parentWindow;
		ISynchronizeInvoke _synchronizer;
		Image _headerImage;
		IUIService _uiService;

		/// <summary>
		/// Initializes a new instance of the <see cref="SvnUIBindArgs"/> class.
		/// </summary>
		public SvnUIBindArgs()
		{
		}

		/// <summary>
		/// Gets or sets the parent window.
		/// </summary>
		/// <value>The parent window.</value>
		public IWin32Window ParentWindow
		{
			get { return _parentWindow; }
			set { _parentWindow = value; }
		}

		/// <summary>
		/// Gets or sets the synchronizer.
		/// </summary>
		/// <value>The synchronizer.</value>
		public ISynchronizeInvoke Synchronizer
		{
			get { return _synchronizer; }
			set { _synchronizer = value; }
		}

		/// <summary>
		/// Gets or sets the header image.
		/// </summary>
		/// <value>The header image.</value>
		public Image HeaderImage
		{
			get { return _headerImage; }
			set { _headerImage = value; }
		}

		/// <summary>
		/// Gets or sets the UI service.
		/// </summary>
		/// <remarks>The UI service allows redirecting dialog processing to the specific UI service. 
		/// (You should use this within .Net development environments (Like MS Visual Studio) implementing this interface)</remarks>
		/// <value>The UI service.</value>
		public IUIService UIService
		{
			get { return _uiService; }
			set { _uiService = value; }
		}
	}
}
