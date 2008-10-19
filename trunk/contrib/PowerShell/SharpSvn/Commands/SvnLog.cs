using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnLog)]
    public sealed class SvnLog : SvnSingleTargetCommand<SvnLogArgs>
    {
        [Parameter]
        public SvnRevision Start
        {
            get { return SvnArguments.Start; }
            set { SvnArguments.Start = value; }
        }
        [Parameter]
        public SvnRevision End
        {
            get { return SvnArguments.End; }
            set { SvnArguments.End = value; }
        }

        [Parameter]
        public int Limit
        {
            get { return SvnArguments.Limit; }
            set { SvnArguments.Limit = value; }
        }


        protected override void ProcessRecord()
        {
            Uri u;
            if (TryGetUri(out u))
                Client.Log(u, SvnArguments, new EventHandler<SvnLogEventArgs>(HandleLog));
            else
                Client.Log(Target, SvnArguments, new EventHandler<SvnLogEventArgs>(HandleLog));
        }

        void HandleLog(object sender, SvnLogEventArgs e)
        {
            WriteObject(e);
        }
    }
}
