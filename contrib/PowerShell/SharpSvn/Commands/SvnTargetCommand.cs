using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;

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

        public TTarget GetTarget<TTarget>() where TTarget : SvnTarget
        {
            SvnTarget rslt;
            if (SvnTarget.TryParse(Target, out rslt))
                return rslt as TTarget;
            return null;
        }

        public bool TryGetUri(out Uri u)
        {
            SvnUriTarget t = GetTarget<SvnUriTarget>();
            if (t != null)
            {
                u = t.Uri;
                return true;
            }
            u = null;
            return false;
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

        protected Collection<SvnTarget> GetSvnTargets<TTarget>() where TTarget : SvnTarget
        {
            Collection<SvnTarget> rslt = new Collection<SvnTarget>();
            foreach (string targetName in Targets)
            {
                SvnTarget target;
                if (SvnTarget.TryParse(targetName, out target) &&
                    target is TTarget)
                {
                    rslt.Add(target);
                }
            }
            return rslt;
        }
    }
}
