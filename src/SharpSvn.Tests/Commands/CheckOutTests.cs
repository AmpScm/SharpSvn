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
using SharpSvn.Security;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests for the Client::Checkout method
    /// </summary>
    [TestClass]
    public class CheckOutTests : TestBase
    {
        [TestMethod]
        public void CheckOut_TestPeg()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.DefaultBranched);
            Uri trunk = new Uri(reposUri, "trunk/");

            SvnUpdateResult result;
            Assert.That(Client.CheckOut(trunk, sbox.GetTempDir(), out result));

            Assert.That(result, Is.Not.Null);
            long head = result.Revision;

            Assert.That(Client.CheckOut(new SvnUriTarget(trunk, 1), GetTempDir(), out result));
            Assert.That(result.Revision, Is.EqualTo(1));

            SvnCheckOutArgs a = new SvnCheckOutArgs();
            a.Revision = 1;

            Assert.That(Client.CheckOut(new SvnUriTarget(new Uri(reposUri, "branches/trunk-r2"), 4), sbox.GetTempDir(), a, out result));
            Assert.That(result.Revision, Is.EqualTo(1));

            Assert.That(Client.CheckOut(trunk, GetTempDir(), a, out result));
            Assert.That(result.Revision, Is.EqualTo(1));
        }


        /// <summary>
        /// Test a standard checkout operation
        /// </summary>
        [TestMethod]
        public void CheckOut_BasicCheckout()
        {
            string newWc = GetTempDir();
            SvnCheckOutArgs a = new SvnCheckOutArgs();
            this.Client.CheckOut(this.ReposUrl, newWc, a);

            Assert.That(File.Exists(Path.Combine(newWc, "Form.cs")),
                "Checked out file not there");
            Assert.That(Directory.Exists(Path.Combine(newWc, SvnClient.AdministrativeDirectoryName)),
                "No admin directory found");
            Assert.That(this.RunCommand("svn", "st \"" + newWc + "\"").Trim(), Is.EqualTo(""),
                "Wrong status");
        }

        [TestMethod]
        public void CheckOut_ProgressEvent()
        {
            string newWc = GetTempDir();
            SvnCheckOutArgs a = new SvnCheckOutArgs();
            bool progressCalled = false;
            a.Progress += delegate(object sender, SvnProgressEventArgs e) { progressCalled = true; };
            a.Depth = SvnDepth.Empty;
            this.Client.CheckOut(new Uri("http://svn.apache.org/repos/asf/subversion/"), newWc, a);

            Assert.That(progressCalled, "Progress delegate not called");
        }
    }
}
