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
    public sealed class SvnCopy : SvnMultipleTargetCommand<SvnCopyArgs>
    {
        string _toPath;

        [Parameter(Position = 1, Mandatory = true)]
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
