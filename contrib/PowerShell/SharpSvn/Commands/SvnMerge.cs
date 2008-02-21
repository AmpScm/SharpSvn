using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    public sealed class SvnMerge : SvnSingleTargetCommand<SvnMergeArgs>
    {
        string _source;
        SvnMergeRange[] _mergeRanges;

        [Parameter(Position = 1, Mandatory=true)]
        [ValidateNotNullOrEmpty]
        public string Source
        {
            get { return _source; ; }
            set { _source = value; }
        }

        [Parameter]
        public SvnMergeRange[] MergeRanges
        {
            get { return _mergeRanges; }
            set { _mergeRanges = value; }
        }

        public TSource GetSource<TSource>() where TSource : SvnTarget
        {
            SvnTarget rslt;
            if (SvnTarget.TryParse(Source, out rslt))
                return rslt as TSource;
            return null;
        }

        protected override void ProcessRecord()
        {
            Client.Merge(Target, GetSource<SvnTarget>(), MergeRanges, SvnArguments);
        }
    }
}
