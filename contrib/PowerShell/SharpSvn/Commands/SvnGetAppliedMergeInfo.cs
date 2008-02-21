using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnAppliedMerges)]
    public sealed class SvnGetAppliedMergeInfo : SvnSingleTargetCommand<SvnGetAppliedMergeInfoArgs>
    {
        protected override void ProcessRecord()
        {
            SvnAppliedMergeInfo info;
            Client.GetAppliedMergeInfo(Target, SvnArguments, out info);
            WriteObject(info);
        }
    }
}
