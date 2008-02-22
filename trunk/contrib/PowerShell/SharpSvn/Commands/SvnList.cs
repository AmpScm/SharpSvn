using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
	[Cmdlet(VerbsCommon.Get, SvnNouns.SvnList)]
	public sealed class SvnList : SvnSingleTargetCommand<SvnListArgs>
	{
		[Parameter]
		public SvnDepth Depth
		{
			get { return SvnArguments.Depth; }
			set { SvnArguments.Depth = value; }
		}

		[Parameter]
		public bool IgnoreLocks
		{
			get { return !SvnArguments.FetchLocks; }
			set { SvnArguments.FetchLocks = !value; }
		}

		[Parameter]
		public SvnRevision Revision
		{
			get { return SvnArguments.Revision; }
			set { SvnArguments.Revision = value; }
		}

		protected override void ProcessRecord()
		{
			SvnArguments.EntryItems = SvnDirEntryItems.AllFieldsV15;
			Client.List(GetTarget<SvnTarget>(), SvnArguments, delegate(object sender, SvnListEventArgs e)
			{
				WriteObject(e);
			});
		}
	}
}
