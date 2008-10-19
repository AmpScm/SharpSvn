using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnAvailableMerges)]
    public sealed class SvnGetAvailableMergeInfo : SvnSingleTargetCommand<SvnMergesEligibleArgs>
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
            Collection<SvnMergesEligibleEventArgs> info;
            Client.GetMergesEligible(Target, SourceUri, SvnArguments, out info);
            WriteObject(info);
        }
    }
}
