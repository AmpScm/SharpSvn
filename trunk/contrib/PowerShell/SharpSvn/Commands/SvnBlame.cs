using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnBlame)]
    public class SvnBlame:SvnSingleTargetCommand<SvnBlameArgs>
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
        public bool IncludeMergedRevisions
        {
            get { return SvnArguments.IncludeMergedRevisions; }
            set { SvnArguments.IncludeMergedRevisions = value; }
        }
        [Parameter]
        public bool IgnoreMimeType
        {
            get { return SvnArguments.IgnoreMimeType; }
            set { SvnArguments.IgnoreMimeType = value; }
        }
        [Parameter]
        public bool IgnoreLineEndings
        {
            get { return SvnArguments.IgnoreLineEndings; }
            set { SvnArguments.IgnoreLineEndings = value; }
        }

        protected override void ProcessRecord()
        {
            Collection<SvnBlameEventArgs> blameResults;
			Client.Blame(GetTarget<SvnTarget>(), SvnArguments, delegate(object sender, SvnBlameEventArgs e)
			{
				WriteObject(e);
			});
        }
    }
}
