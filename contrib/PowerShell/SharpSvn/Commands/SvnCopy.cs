using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    public sealed class SvnCopy : SvnMultipleTargetCommand<SvnCopyArgs>
    {
        string _toPath;

        [Parameter(Position = 1, Mandatory=true)]
        public string ToPath
        {
            get { return _toPath; }
            set { _toPath = value; }
        }

        [Parameter]
        public bool AlwaysCopyAsChild
        {
            get { return SvnArguments.AlwaysCopyAsChild; }
            set { SvnArguments.AlwaysCopyAsChild = true; }
        }

        [Parameter]
        public bool MakeParents
        {
            get { return SvnArguments.MakeParents; }
            set { SvnArguments.MakeParents = value; }
        }

        protected override void ProcessRecord()
        {
            Client.Copy(GetSvnTargets<SvnTarget>(), ToPath, SvnArguments);
        }
    }
}
