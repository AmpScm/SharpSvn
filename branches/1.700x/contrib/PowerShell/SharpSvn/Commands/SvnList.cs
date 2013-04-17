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
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnList)]
    public sealed class SvnList : SvnSingleTargetCommand<SvnListArgs>
    {
        [Parameter]
        public SvnDepth Depth
        {
            get { return SvnArguments.Depth; }
            set { SvnArguments.Depth = value; }
        }

        [Parameter]
        public bool IgnoreLocks
        {
            get { return !SvnArguments.RetrieveLocks; }
            set { SvnArguments.RetrieveLocks = !value; }
        }

        [Parameter]
        public SvnRevision Revision
        {
            get { return SvnArguments.Revision; }
            set { SvnArguments.Revision = value; }
        }

        protected override void ProcessRecord()
        {
            SvnArguments.RetrieveEntries = SvnDirEntryItems.AllFieldsV15;
            Client.List(GetTarget<SvnTarget>(), SvnArguments, delegate(object sender, SvnListEventArgs e)
            {
                WriteObject(e);
            });
        }
    }
}
