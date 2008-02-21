using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.PowerShell.Commands
{
    [Cmdlet(SvnVerbs.Add, SvnNouns.Items)]
    public sealed class SvnAdd:SvnMultipleTargetCommand<SvnAddArgs>
    {
        [Parameter]
        public bool AddParents
        {
            get { return SvnArguments.AddParents; }
            set { SvnArguments.AddParents = value; }
        }

        [Parameter]
        public SvnDepth Depth
        {
            get { return SvnArguments.Depth; }
            set { SvnArguments.Depth = value; }
        }

        [Parameter]
        public bool Force
        {
            get { return SvnArguments.Force; }
            set { SvnArguments.Force = value; }
        }

        [Parameter]
        public bool NoIgnore
        {
            get { return SvnArguments.NoIgnore; }
            set { SvnArguments.NoIgnore = value; }
        }

        protected override void ProcessRecord()
        {
            foreach (string target in Targets)
                Client.Add(target, SvnArguments);
        }
    }
}
