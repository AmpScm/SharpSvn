// $Id: SvnClientUIHandler.cs 632 2008-08-01 11:05:53Z rhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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
