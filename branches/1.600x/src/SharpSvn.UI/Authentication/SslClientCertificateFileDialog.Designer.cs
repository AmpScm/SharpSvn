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
    partial class SslClientCertificateFileDialog
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
            this.descriptionBox = new System.Windows.Forms.Label();
            this.usernameLabel = new System.Windows.Forms.Label();
            this.fileBox = new System.Windows.Forms.TextBox();
            this.rememberCheck = new System.Windows.Forms.CheckBox();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.fileBrowseButton = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // descriptionBox
            // 
            this.descriptionBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.descriptionBox.Location = new System.Drawing.Point(12, 73);
            this.descriptionBox.Name = "descriptionBox";
            this.descriptionBox.Size = new System.Drawing.Size(360, 36);
            this.descriptionBox.TabIndex = 1;
            this.descriptionBox.Text = "Description";
            // 
            // usernameLabel
            // 
            this.usernameLabel.AutoSize = true;
            this.usernameLabel.Location = new System.Drawing.Point(12, 116);
            this.usernameLabel.Name = "usernameLabel";
            this.usernameLabel.Size = new System.Drawing.Size(76, 13);
            this.usernameLabel.TabIndex = 2;
            this.usernameLabel.Text = "Certificate &File:";
            // 
            // fileBox
            // 
            this.fileBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.fileBox.Location = new System.Drawing.Point(94, 113);
            this.fileBox.Name = "fileBox";
            this.fileBox.Size = new System.Drawing.Size(236, 20);
            this.fileBox.TabIndex = 3;
            // 
            // rememberCheck
            // 
            this.rememberCheck.AutoSize = true;
            this.rememberCheck.Location = new System.Drawing.Point(92, 139);
            this.rememberCheck.Name = "rememberCheck";
            this.rememberCheck.Size = new System.Drawing.Size(158, 17);
            this.rememberCheck.TabIndex = 5;
            this.rememberCheck.Text = "&Remember my certificate file";
            this.rememberCheck.UseVisualStyleBackColor = true;
            // 
            // okButton
            // 
            this.okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(216, 177);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 6;
            this.okButton.Text = "OK";
            // 
            // cancelButton
            // 
            this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(297, 177);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 7;
            this.cancelButton.Text = "Cancel";
            // 
            // fileBrowseButton
            // 
            this.fileBrowseButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.fileBrowseButton.Location = new System.Drawing.Point(336, 111);
            this.fileBrowseButton.Name = "fileBrowseButton";
            this.fileBrowseButton.Size = new System.Drawing.Size(36, 23);
            this.fileBrowseButton.TabIndex = 4;
            this.fileBrowseButton.Text = "&...";
            this.fileBrowseButton.UseVisualStyleBackColor = true;
            this.fileBrowseButton.Click += new System.EventHandler(this.fileBrowseButton_Click);
            // 
            // panel1
            // 
            this.panel1.BackgroundImage = global::SharpSvn.UI.Properties.Resources.AuthenticationBanner;
            this.panel1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(384, 56);
            this.panel1.TabIndex = 0;
            // 
            // SslClientCertificateFileDialog
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(384, 212);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.fileBrowseButton);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.rememberCheck);
            this.Controls.Add(this.fileBox);
            this.Controls.Add(this.usernameLabel);
            this.Controls.Add(this.descriptionBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "SslClientCertificateFileDialog";
            this.Text = "Title";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label usernameLabel;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        internal System.Windows.Forms.Label descriptionBox;
        internal System.Windows.Forms.TextBox fileBox;
        internal System.Windows.Forms.CheckBox rememberCheck;
        private System.Windows.Forms.Button fileBrowseButton;
        private System.Windows.Forms.Panel panel1;
    }
}