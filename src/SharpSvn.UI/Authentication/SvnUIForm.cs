using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

namespace SharpSvn.UI.Authentication
{
	/// <summary>
	/// 
	/// </summary>
	public class SvnUIForm : Form
	{
		/// <summary>
		/// Initializes a new instance of the <see cref="SharpSvnUIForm"/> class.
		/// </summary>
		protected SvnUIForm()
		{
			ShowInTaskbar = false;
			MinimizeBox = false;
			MaximizeBox = false;
		}

		/// <summary>
		/// Gets or sets a value indicating whether the form is displayed in the Windows taskbar.
		/// </summary>
		/// <value></value>
		/// <returns>true to display the form in the Windows taskbar at run time; otherwise, false. The default is true.
		/// </returns>
		/// <PermissionSet>
		/// 	<IPermission class="System.Security.Permissions.EnvironmentPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// 	<IPermission class="System.Security.Permissions.FileIOPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// 	<IPermission class="System.Security.Permissions.SecurityPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Flags="UnmanagedCode, ControlEvidence"/>
		/// 	<IPermission class="System.Diagnostics.PerformanceCounterPermission, System, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// </PermissionSet>
		[DefaultValue(false), Localizable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public new bool ShowInTaskbar
		{
			get { return base.ShowInTaskbar; }
			set { base.ShowInTaskbar = value; }
		}

		/// <summary>
		/// Gets or sets a value indicating whether the Minimize button is displayed in the caption bar of the form.
		/// </summary>
		/// <value></value>
		/// <returns>true to display a Minimize button for the form; otherwise, false. The default is true.
		/// </returns>
		/// <PermissionSet>
		/// 	<IPermission class="System.Security.Permissions.EnvironmentPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// 	<IPermission class="System.Security.Permissions.FileIOPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// 	<IPermission class="System.Security.Permissions.SecurityPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Flags="UnmanagedCode, ControlEvidence"/>
		/// 	<IPermission class="System.Diagnostics.PerformanceCounterPermission, System, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// </PermissionSet>
		[DefaultValue(false), Localizable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public new bool MinimizeBox
		{
			get { return base.MinimizeBox; }
			set { base.MinimizeBox = value; }
		}

		/// <summary>
		/// Gets or sets a value indicating whether the Maximize button is displayed in the caption bar of the form.
		/// </summary>
		/// <value></value>
		/// <returns>true to display a Maximize button for the form; otherwise, false. The default is true.
		/// </returns>
		/// <PermissionSet>
		/// 	<IPermission class="System.Security.Permissions.EnvironmentPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// 	<IPermission class="System.Security.Permissions.FileIOPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// 	<IPermission class="System.Security.Permissions.SecurityPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Flags="UnmanagedCode, ControlEvidence"/>
		/// 	<IPermission class="System.Diagnostics.PerformanceCounterPermission, System, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Unrestricted="true"/>
		/// </PermissionSet>
		[DefaultValue(false), Localizable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public new bool MaximizeBox
		{
			get { return base.MaximizeBox; }
			set { base.MaximizeBox = value; }
		}
	}
}
