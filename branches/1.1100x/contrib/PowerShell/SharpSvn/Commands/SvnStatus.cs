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
using System.Collections.ObjectModel;
using System.Management.Automation;

namespace SharpSvn.PowerShell.Commands
{
    [Cmdlet(VerbsCommon.Get, SvnNouns.SvnStatus)]
    public sealed class SvnStatus : SvnSingleTargetCommand<SvnStatusArgs>
    {
        protected override void ProcessRecord()
        {
            Client.Status(Target, SvnArguments, delegate(object sender, SvnStatusEventArgs e)
            {
                WriteObject(e);
            });
        }
    }
}
