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
    public sealed class SvnMerge : SvnSingleTargetCommand<SvnMergeArgs>
    {
        string _source;
        SvnMergeRange[] _mergeRanges;

        [Parameter(Position = 1, Mandatory = true)]
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
            Client.Merge(Target, GetSource<SvnTarget>(), new List<SvnMergeRange>(MergeRanges), SvnArguments);
        }
    }
}
