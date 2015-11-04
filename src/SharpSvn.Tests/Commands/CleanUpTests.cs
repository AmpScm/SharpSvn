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

// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests the Client::Cleanup method
    /// </summary>
    [TestClass]
    public class CleanupTests : TestBase
    {
        [TestMethod]
        public void Cleanup_Basic()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            using (SvnClient client = NewSvnClient(false, false))
            {
                client.CleanUp(sbox.Wc);
            }
        }

    }
}
