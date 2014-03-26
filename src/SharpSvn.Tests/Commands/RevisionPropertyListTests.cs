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
    /// Tests Client::RevPropList
    /// </summary>
    [TestClass]
    public class RevPropListTests : TestBase
    {
        /// <summary>
        /// Sets two properties on a repos and tries to retrieve them with Client::RevPropList
        /// </summary>
        [TestMethod]
        public void RevProp_TestBasic()
        {
            SvnSandBox sbox = new SvnSandBox(this);

            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Empty);
            sbox.InstallRevpropHook(ReposUrl);

            this.RunCommand("svn", "ps --revprop -r HEAD foo bar " + ReposUrl);
            this.RunCommand("svn", "ps --revprop -r HEAD kung foo " + ReposUrl);

            SvnPropertyCollection spc;
            Assert.That(Client.GetRevisionPropertyList(ReposUrl, SvnRevision.Head, out spc));

            Assert.That(spc["foo"].ToString(), Is.EqualTo("bar"), "Wrong property value");
            Assert.That(spc["kung"].ToString(), Is.EqualTo("foo"), "Wrong property value");
        }
    }
}
