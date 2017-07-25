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

/// <summary>
/// Tests the Client::RevPropGet
/// </summary>
namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class GetRevisionPropertyTests : TestBase
    {
        /// <summary>
        ///Attempts to Get Properties on a directory in the repository represented by url.
        /// </summary>
        [TestMethod]
        public void TestRevPropGetDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.AnkhSvnCases);
            sbox.InstallRevpropHook(ReposUrl);

            this.RunCommand("svn", "ps --revprop -r HEAD cow moo " + ReposUrl);

            string value;
            Assert.That(Client.GetRevisionProperty(ReposUrl, SvnRevision.Head, "cow", out value));

            Assert.That(value, Is.EqualTo("moo"), "Wrong property value");
        }
    }
}
