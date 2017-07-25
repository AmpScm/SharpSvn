// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

using System;
using System.Collections.Generic;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;

namespace SharpSvn.PowerShell
{
    // TODO: Globbing support

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
            get { return _target ?? SessionState.Path.CurrentLocation.Path; }
            set { _target = value; }
        }

        public TTarget GetTarget<TTarget>() where TTarget : SvnTarget
        {
            SvnTarget rslt;
            if (SvnTarget.TryParse(Target, out rslt))
            {
                return rslt as TTarget;
            }
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
                return _targets ??
                  new string[] { SessionState.Path.CurrentLocation.Path };
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
