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

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnBlame)]
    public class SvnBlame : SvnSingleTargetCommand<SvnBlameArgs>
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
            Client.Blame(GetTarget<SvnTarget>(), SvnArguments, delegate(object sender, SvnBlameEventArgs e)
            {
                WriteObject(e);
            });
        }
    }
}
