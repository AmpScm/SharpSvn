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

namespace SharpSvn.UI.Authentication
{
	partial class SslServerCertificateTrustDialog
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.issuer = new System.Windows.Forms.Label();
            this.fingerprint = new System.Windows.Forms.Label();
            this.validFrom = new System.Windows.Forms.Label();
            this.validTo = new System.Windows.Forms.Label();
            this.hostname = new System.Windows.Forms.Label();
            this.certificateBox = new System.Windows.Forms.TextBox();
            this.caUnknownLabel = new System.Windows.Forms.Label();
            this.serverMismatchLabel = new System.Windows.Forms.Label();
            this.errorsGroupBox = new System.Windows.Forms.GroupBox();
            this.invalidDateImage = new System.Windows.Forms.PictureBox();
            this.invalidDateLabel = new System.Windows.Forms.Label();
            this.caUnknownImage = new System.Windows.Forms.PictureBox();
            this.serverMismatchImage = new System.Windows.Forms.PictureBox();
            this.cancelButton = new System.Windows.Forms.Button();
            this.okButton = new System.Windows.Forms.Button();
            this.rememberCheck = new System.Windows.Forms.CheckBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.errorsGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.invalidDateImage)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.caUnknownImage)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.serverMismatchImage)).BeginInit();
            this.SuspendLayout();
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 164);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(58, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Hostname:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 184);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(59, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Fingerprint:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 204);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Valid from:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 224);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(45, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Valid to:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 244);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(38, 13);
            this.label5.TabIndex = 10;
            this.label5.Text = "Issuer:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 287);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(57, 13);
            this.label6.TabIndex = 12;
            this.label6.Text = "Certificate:";
            // 
            // issuer
            // 
            this.issuer.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.issuer.Location = new System.Drawing.Point(72, 244);
            this.issuer.Name = "issuer";
            this.issuer.Size = new System.Drawing.Size(311, 37);
            this.issuer.TabIndex = 11;
            this.issuer.Text = "issuerLabel";
            // 
            // fingerprint
            // 
            this.fingerprint.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.fingerprint.Location = new System.Drawing.Point(72, 184);
            this.fingerprint.Name = "fingerprint";
            this.fingerprint.Size = new System.Drawing.Size(311, 16);
            this.fingerprint.TabIndex = 5;
            this.fingerprint.Text = "fingerprintLabel";
            // 
            // validFrom
            // 
            this.validFrom.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.validFrom.Location = new System.Drawing.Point(72, 204);
            this.validFrom.Name = "validFrom";
            this.validFrom.Size = new System.Drawing.Size(311, 16);
            this.validFrom.TabIndex = 7;
            this.validFrom.Text = "validFromLabel";
            // 
            // validTo
            // 
            this.validTo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.validTo.Location = new System.Drawing.Point(72, 224);
            this.validTo.Name = "validTo";
            this.validTo.Size = new System.Drawing.Size(311, 16);
            this.validTo.TabIndex = 9;
            this.validTo.Text = "validToLabel";
            // 
            // hostname
            // 
            this.hostname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.hostname.Location = new System.Drawing.Point(72, 164);
            this.hostname.Name = "hostname";
            this.hostname.Size = new System.Drawing.Size(311, 16);
            this.hostname.TabIndex = 3;
            this.hostname.Text = "hostnameLabel";
            // 
            // certificateBox
            // 
            this.certificateBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.certificateBox.BackColor = System.Drawing.SystemColors.Control;
            this.certificateBox.Location = new System.Drawing.Point(75, 284);
            this.certificateBox.Multiline = true;
            this.certificateBox.Name = "certificateBox";
            this.certificateBox.ReadOnly = true;
            this.certificateBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.certificateBox.Size = new System.Drawing.Size(308, 75);
            this.certificateBox.TabIndex = 13;
            this.certificateBox.Text = "certificateTextBox";
            // 
            // caUnknownLabel
            // 
            this.caUnknownLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.caUnknownLabel.AutoSize = true;
            this.caUnknownLabel.Location = new System.Drawing.Point(62, 25);
            this.caUnknownLabel.Name = "caUnknownLabel";
            this.caUnknownLabel.Size = new System.Drawing.Size(185, 13);
            this.caUnknownLabel.TabIndex = 0;
            this.caUnknownLabel.Text = "The Certificate Authority(CA) is trusted";
            // 
            // serverMismatchLabel
            // 
            this.serverMismatchLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.serverMismatchLabel.AutoSize = true;
            this.serverMismatchLabel.Location = new System.Drawing.Point(62, 65);
            this.serverMismatchLabel.Name = "serverMismatchLabel";
            this.serverMismatchLabel.Size = new System.Drawing.Size(234, 13);
            this.serverMismatchLabel.TabIndex = 2;
            this.serverMismatchLabel.Text = "The certificate\'s hostname matches the server\'s.";
            // 
            // errorsGroupBox
            // 
            this.errorsGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.errorsGroupBox.Controls.Add(this.serverMismatchLabel);
            this.errorsGroupBox.Controls.Add(this.invalidDateImage);
            this.errorsGroupBox.Controls.Add(this.caUnknownLabel);
            this.errorsGroupBox.Controls.Add(this.invalidDateLabel);
            this.errorsGroupBox.Controls.Add(this.caUnknownImage);
            this.errorsGroupBox.Controls.Add(this.serverMismatchImage);
            this.errorsGroupBox.Location = new System.Drawing.Point(9, 61);
            this.errorsGroupBox.Name = "errorsGroupBox";
            this.errorsGroupBox.Size = new System.Drawing.Size(374, 97);
            this.errorsGroupBox.TabIndex = 1;
            this.errorsGroupBox.TabStop = false;
            this.errorsGroupBox.Text = "There are some problems with this server\'s certificate:";
            // 
            // invalidDateImage
            // 
            this.invalidDateImage.BackColor = System.Drawing.Color.Transparent;
            this.invalidDateImage.Location = new System.Drawing.Point(40, 45);
            this.invalidDateImage.Name = "invalidDateImage";
            this.invalidDateImage.Size = new System.Drawing.Size(16, 16);
            this.invalidDateImage.TabIndex = 3;
            this.invalidDateImage.TabStop = false;
            // 
            // invalidDateLabel
            // 
            this.invalidDateLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.invalidDateLabel.AutoSize = true;
            this.invalidDateLabel.Location = new System.Drawing.Point(63, 45);
            this.invalidDateLabel.Name = "invalidDateLabel";
            this.invalidDateLabel.Size = new System.Drawing.Size(185, 13);
            this.invalidDateLabel.TabIndex = 1;
            this.invalidDateLabel.Text = "The server certificate has a valid date";
            // 
            // caUnknownImage
            // 
            this.caUnknownImage.Location = new System.Drawing.Point(40, 25);
            this.caUnknownImage.Name = "caUnknownImage";
            this.caUnknownImage.Size = new System.Drawing.Size(16, 16);
            this.caUnknownImage.TabIndex = 1;
            this.caUnknownImage.TabStop = false;
            // 
            // serverMismatchImage
            // 
            this.serverMismatchImage.BackColor = System.Drawing.Color.Transparent;
            this.serverMismatchImage.Location = new System.Drawing.Point(40, 65);
            this.serverMismatchImage.Name = "serverMismatchImage";
            this.serverMismatchImage.Size = new System.Drawing.Size(16, 16);
            this.serverMismatchImage.TabIndex = 2;
            this.serverMismatchImage.TabStop = false;
            // 
            // cancelButton
            // 
            this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(308, 370);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 16;
            this.cancelButton.Text = "&Reject";
            // 
            // okButton
            // 
            this.okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(227, 370);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 15;
            this.okButton.Text = "&Accept";
            // 
            // rememberCheck
            // 
            this.rememberCheck.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.rememberCheck.AutoSize = true;
            this.rememberCheck.Location = new System.Drawing.Point(75, 374);
            this.rememberCheck.Name = "rememberCheck";
            this.rememberCheck.Size = new System.Drawing.Size(100, 17);
            this.rememberCheck.TabIndex = 14;
            this.rememberCheck.Text = "&Save certificate";
            this.rememberCheck.UseVisualStyleBackColor = true;
            // 
            // panel1
            // 
            this.panel1.BackgroundImage = global::SharpSvn.UI.Properties.Resources.AuthenticationBanner;
            this.panel1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(395, 56);
            this.panel1.TabIndex = 0;
            // 
            // SslServerCertificateTrustDialog
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(395, 405);
            this.Controls.Add(this.rememberCheck);
            this.Controls.Add(this.errorsGroupBox);
            this.Controls.Add(this.certificateBox);
            this.Controls.Add(this.hostname);
            this.Controls.Add(this.validTo);
            this.Controls.Add(this.validFrom);
            this.Controls.Add(this.fingerprint);
            this.Controls.Add(this.issuer);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.panel1);
            this.Name = "SslServerCertificateTrustDialog";
            this.ShowIcon = false;
            this.Text = "Title";
            this.errorsGroupBox.ResumeLayout(false);
            this.errorsGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.invalidDateImage)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.caUnknownImage)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.serverMismatchImage)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label caUnknownLabel;
		private System.Windows.Forms.Label serverMismatchLabel;
		private System.Windows.Forms.GroupBox errorsGroupBox;
		private System.Windows.Forms.Label invalidDateLabel;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button cancelButton;
		private System.Windows.Forms.Button okButton;
		internal System.Windows.Forms.Label issuer;
		internal System.Windows.Forms.Label fingerprint;
		internal System.Windows.Forms.Label validFrom;
		internal System.Windows.Forms.Label validTo;
		internal System.Windows.Forms.Label hostname;
		internal System.Windows.Forms.TextBox certificateBox;
		internal System.Windows.Forms.PictureBox serverMismatchImage;
		internal System.Windows.Forms.PictureBox caUnknownImage;
		internal System.Windows.Forms.PictureBox invalidDateImage;
		internal System.Windows.Forms.CheckBox rememberCheck;

	}
}