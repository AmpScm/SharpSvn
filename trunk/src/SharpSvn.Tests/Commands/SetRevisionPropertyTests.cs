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

// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

/// <summary>
/// Tests the Client::RevPropSet
/// </summary>
namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class SetRevisionPropertyTests : TestBase
    {
        /// <summary>
        ///Attempts to Set Properties on a file in the repository represented by url.
        /// </summary>
        [Test]
        public void TestRevSetPropDir()
        {
            byte[] propval = Encoding.UTF8.GetBytes("moo");

            this.Client.SetRevisionProperty(ReposUrl, SvnRevision.Head, "cow", propval);

            Assert.That(this.RunCommand("svn", "propget cow --revprop -r head " + this.ReposUrl).Trim(), Is.EqualTo("moo"),
                "Couldn't set prop on selected Repos!");
        }

        [Test]
        public void TestSetLog()
        {
            string reposPath = GetRepos(TestReposType.CollabRepos);

            InstallRevpropHook(reposPath);

            Uri target = PathToUri(reposPath, true);
            SvnRevision rev = 2;

            Client.SetRevisionProperty(target, rev, SvnPropertyNames.SvnDate, DateTime.UtcNow.ToString("o"));

            Client.SetRevisionProperty(target, rev, SvnPropertyNames.SvnDate, SvnPropertyNames.FormatDate(DateTime.UtcNow));
        }
    }
}
