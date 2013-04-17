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

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(SvnVerbs.Add, SvnNouns.Items)]
    public sealed class SvnAdd : SvnMultipleTargetCommand<SvnAddArgs>
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
            {
                Client.Add(target, SvnArguments);
            }
        }
    }
}
