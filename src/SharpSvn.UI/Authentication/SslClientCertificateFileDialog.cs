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