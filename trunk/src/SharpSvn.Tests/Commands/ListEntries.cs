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
using System.ComponentModel;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class ListEntries : TestBase
    {
        [TestMethod]
        public void ListEntries_WalkEntries()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            string tmpDir = sbox.Wc;

            using (SvnWorkingCopyClient wcc = new SvnWorkingCopyClient())
            {
                SvnWorkingCopyEntriesArgs a = new SvnWorkingCopyEntriesArgs();
                a.RetrieveHidden = true;
                //a.Depth = SvnDepth.Infinity;

                bool touched = false;
                Assert.That(wcc.ListEntries(tmpDir, a,
                    delegate(object sender, SvnWorkingCopyEntryEventArgs e)
                    {
                        touched = true;
                    }), Is.True);


                Assert.That(touched);
            }
        }
    }
}
