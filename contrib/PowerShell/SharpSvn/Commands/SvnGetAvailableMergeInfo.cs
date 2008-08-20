using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnAvailableMerges)]
    public sealed class SvnGetAvailableMergeInfo : SvnSingleTargetCommand<SvnGetAvailableMergeInfoArgs>
    {
        Uri _sourceUri;

        [Parameter(Mandatory=true, Position=1)]
        [ValidateNotNullOrEmpty]
        public Uri SourceUri
        {
            get { return _sourceUri; }
            set { _sourceUri = value; }
        }

        protected override void ProcessRecord()
        {
            SvnAvailableMergeInfo info;
            Client.GetAvailableMergeInfo(Target, SourceUri, SvnArguments, out info);
            WriteObject(info);
        }
    }
}
