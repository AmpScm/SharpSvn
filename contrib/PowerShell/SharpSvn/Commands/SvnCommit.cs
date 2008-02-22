using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(SvnVerbs.Commit, SvnNouns.Items)]
    public sealed class SvnCommit : SvnMultipleTargetCommand<SvnCommitArgs>
    {
        [Parameter]
        public SvnDepth Depth
        {
            get { return SvnArguments.Depth; }
            set { SvnArguments.Depth = value; }
        }

        [Parameter]
        public bool KeepChangeList
        {
            get { return SvnArguments.KeepChangeList; }
            set { SvnArguments.KeepChangeList = value; }
        }

        [Parameter]
        public bool KeepLocks
        {
            get { return SvnArguments.KeepLocks; }
            set { SvnArguments.KeepLocks = value; }
        }

        [Parameter(Mandatory = true)]
        [ValidateNotNullOrEmpty]
        public string LogMessage
        {
            get { return SvnArguments.LogMessage; }
            set { SvnArguments.LogMessage = value; }
        }

        protected override void ProcessRecord()
        {
			SvnCommitInfo info;
			if (Client.Commit(Targets, SvnArguments, out info))
				WriteObject(info);
        }
    }
}
