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
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

using SharpSvn;

/// <summary>
/// Tests the Client::RevPropSet
/// </summary>
namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class RevisionPropertyTests : TestBase
    {
        /// <summary>
        ///Attempts to Set Properties on a file in the repository represented by url.
        /// </summary>
        [TestMethod]
        public void RevisionProperty_RevSetPropDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Empty);
            InstallRevpropHook(ReposUrl);

            byte[] propval = Encoding.UTF8.GetBytes("moo");

            this.Client.SetRevisionProperty(ReposUrl, SvnRevision.Head, "cow", propval);

            Assert.That(this.RunCommand("svn", "propget cow --revprop -r head " + ReposUrl).Trim(), Is.EqualTo("moo"),
                "Couldn't set prop on selected Repos!");
        }

        [TestMethod]
        public void RevisionProperty_SetLog()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri reposUri = sbox.CreateRepository(SandBoxRepository.DefaultBranched);
            string reposPath = reposUri.AbsolutePath;

            InstallRevpropHook(reposPath);

            SvnRevision rev = 2;

            Client.SetRevisionProperty(reposUri, rev, SvnPropertyNames.SvnDate, DateTime.UtcNow.ToString("o"));

            Client.SetRevisionProperty(new Uri(reposUri, "trunk"), rev, SvnPropertyNames.SvnDate, SvnPropertyNames.FormatDate(DateTime.UtcNow));
        }


        protected void InstallRevpropHook(string reposPath)
        {
            string bat = Path.ChangeExtension(SvnHookArguments.GetHookFileName(reposPath, SvnHookType.PreRevPropChange), ".bat");

            File.WriteAllText(bat, "exit 0");
        }
    }
}
