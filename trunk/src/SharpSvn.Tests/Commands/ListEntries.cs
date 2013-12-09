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
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class ListEntries : TestBase
    {
        [Test]
        public void WalkEntries()
        {
            string tmpDir = GetTempDir();
            Client.CheckOut(new Uri(CollabReposUri, "trunk/"), tmpDir);
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
