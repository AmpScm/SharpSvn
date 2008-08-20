using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(SvnVerbs.Delete, SvnNouns.Items)]
    public sealed class SvnDelete : SvnMultipleTargetCommand<SvnDeleteArgs>
    {
        protected override void ProcessRecord()
        {
            Client.Delete(Targets, SvnArguments);
        }
    }
}
