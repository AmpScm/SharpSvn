using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;

namespace SharpSvn.PowerShell
{
    public abstract class SvnSingleTargetCommand<TArgument> :
        SvnCommandBase<TArgument>
        where TArgument : SvnClientArgs, new()
    {
        string _target;

        protected SvnSingleTargetCommand()
            : base()
        { }

        [Parameter(Position = 0, ValueFromPipeline = true, ValueFromPipelineByPropertyName = true)]
        public string Target
        {
            get { return _target; }
            set { _target = value; }
        }
    }
    public abstract class SvnMultipleTargetCommand<TArgument> : 
        SvnCommandBase<TArgument> 
        where TArgument : SvnClientArgs, new()
    {
        string[] _targets;

        protected SvnMultipleTargetCommand()
            :base()
        {}

        [Parameter(Position = 0, ValueFromPipeline = true, ValueFromPipelineByPropertyName = true)]
        public string[] Targets
        {
            get
            {
                return _targets == null ?
                  new string[] { SessionState.Path.CurrentLocation.Path } :
                  _targets;
            }
            set { _targets = value; }
        }
    }
}
