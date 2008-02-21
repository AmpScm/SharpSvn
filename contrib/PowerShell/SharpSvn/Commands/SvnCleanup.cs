using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(SvnVerbs.Cleanup, SvnNouns.Item)]
    public sealed class SvnCleanup : SvnSingleTargetCommand<SvnCleanUpArgs>
    {
        protected override void ProcessRecord()
        {
            Client.CleanUp(Target, SvnArguments);
        }
    }
}
