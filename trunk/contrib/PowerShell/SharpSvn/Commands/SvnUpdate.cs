using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(SvnVerbs.Update, SvnNouns.Items)]
    public sealed class SvnUpdate : SvnMultipleTargetCommand<SvnUpdateArgs>
    {
        [Parameter]
        public SvnDepth Depth
        {
            get { return SvnArguments.Depth; }
            set { SvnArguments.Depth = value; }
        }

        [Parameter]
        public bool IgnoreExternals
        {
            get { return SvnArguments.IgnoreExternals; }
            set { SvnArguments.IgnoreExternals = value; }
        }

        [Parameter]
        public bool AllowObstructions
        {
            get { return SvnArguments.AllowObstructions; }
            set { SvnArguments.AllowObstructions = value; }
        }

        [Parameter]
        public bool KeepDepth
        {
            get { return SvnArguments.KeepDepth; }
            set { SvnArguments.KeepDepth = value; }
        }

        [Parameter]
        public SvnRevision Revision
        {
            get { return SvnArguments.Revision; }
            set { SvnArguments.Revision = value; }
        }

        protected override void ProcessRecord()
        {
            SvnUpdateResult result;
            if (Client.Update(Targets, SvnArguments, out result))
            {
                WriteObject(result);
            }
        }
    }
}
