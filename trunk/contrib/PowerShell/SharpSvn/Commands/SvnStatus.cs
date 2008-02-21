using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnStatus)]
    public sealed class SvnStatus:SvnSingleTargetCommand<SvnStatusArgs>
    {
        protected override void ProcessRecord()
        {
            Collection<SvnStatusEventArgs> statuses;
            Client.GetStatus(Target, SvnArguments, out statuses);
            WriteObject(SvnArguments, /*enumerateCollection*/ true);
        }
    }
}
