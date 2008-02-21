using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnLog)]
    public sealed class SvnLog : SvnMultipleTargetCommand<SvnLogArgs>
    {

        [Parameter]
        public int Limit
        {
            get { return SvnArguments.Limit; }
            set { SvnArguments.Limit = value; }
        }
      

        protected override void ProcessRecord()
        {
            Collection<SvnLogEventArgs> logItems;
            Client.GetLog(Targets, SvnArguments, out logItems);
            WriteObject(logItems, true);
        }
    }
}
