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
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnAvailableMerges)]
    public sealed class SvnGetAvailableMergeInfo : SvnSingleTargetCommand<SvnMergesEligibleArgs>
    {
        Uri _sourceUri;

        [Parameter(Mandatory=true, Position=1)]
        [ValidateNotNullOrEmpty]
        public Uri SourceUri
        {
            get { return _sourceUri; }
            set { _sourceUri = value; }
        }

        protected override void ProcessRecord()
        {
            Collection<SvnMergesEligibleEventArgs> info;
            Client.GetMergesEligible(Target, SourceUri, SvnArguments, out info);
            WriteObject(info);
        }
    }
}
