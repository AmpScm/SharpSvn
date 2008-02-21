// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SharpSvn.UI.Authentication
{
	/// <summary>
	///
	/// </summary>
	public partial class SslServerCertificateTrustDialog : Form
	{
		/// <summary>
		/// Initializes a new instance of the <see cref="SslServerCertificateTrustDialog"/> class.
		/// </summary>
		public SslServerCertificateTrustDialog()
		{
			InitializeComponent();
		}

        internal void SetImage(Image image)
        {
            panel1.BackgroundImage = image;
        }
    }
}