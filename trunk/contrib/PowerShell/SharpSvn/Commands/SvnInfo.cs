using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnInfo)]
    public sealed class SvnInfo : SvnSingleTargetCommand<SvnInfoArgs>
    {
        [Parameter]
        public SvnDepth Depth
        {
            get { return SvnArguments.Depth; }
            set { SvnArguments.Depth = value; }
        }
        protected override void ProcessRecord()
        {
            Collection<SvnInfoEventArgs> infoItems;
            Client.GetInfo(GetTarget<SvnTarget>(), SvnArguments, out infoItems);
            WriteObject(infoItems, /*enumerateCollection*/ true);
        }
    }
}
