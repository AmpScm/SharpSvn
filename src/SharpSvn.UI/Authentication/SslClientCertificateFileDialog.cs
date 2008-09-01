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
	public partial class SslClientCertificateFileDialog : SvnUIForm
	{
		/// <summary>
		/// Initializes a new instance of the <see cref="SslClientCertificatePassPhraseDialog"/> class.
		/// </summary>
		public SslClientCertificateFileDialog()
		{
			InitializeComponent();
		}

        private void fileBrowseButton_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog ofd = new OpenFileDialog())
            {                
                if (!string.IsNullOrEmpty(fileBox.Text))
                    ofd.FileName = fileBox.Text;

                ofd.ValidateNames = true;
                ofd.CheckFileExists = true;
                ofd.Filter = "Certificate Files|*.cer;*.pem;*.p12|All Files|*";
                ofd.FilterIndex = 0;

                if (ofd.ShowDialog(this) == DialogResult.OK)
                    fileBox.Text = ofd.FileName;
            }
        }
	}
}