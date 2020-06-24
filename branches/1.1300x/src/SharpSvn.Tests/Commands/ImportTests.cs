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
using System.Collections.Generic;
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
    /// Tests for the Client::Import method
    /// </summary>
    [TestClass]
    public class ImportTests : TestBase
    {
        /// <summary>
        /// Tests importing an unverioned file into the repository with the new entry :
        /// testfile2.txt.
        /// </summary>
        [TestMethod]
        public void TestImportFile()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Empty);
            string WcPath = sbox.Wc;

            string truePath = CreateTextFile(WcPath, "testfile.txt");
            Uri trueDstUrl = new Uri(ReposUrl, "testfile.txt");

            SvnImportArgs a = new SvnImportArgs();
            a.Depth = SvnDepth.Empty;

            Assert.That(Client.RemoteImport(truePath, trueDstUrl, a));

            String cmd = this.RunCommand("svn", "list " + ReposUrl.ToString());
            Assert.That(cmd.IndexOf("testfile.txt") >= 0, "File wasn't imported ");
        }

        /// <summary>
        /// Tests importing an unversioned directory into the repository recursively
        /// with the new entry: newDir2.
        /// </summary>
        [TestMethod]
        public void TestImportDir()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            string WcPath = sbox.Wc;
            Uri ReposUrl = sbox.CreateRepository(SandBoxRepository.Empty);

            string dir1, dir2, testFile1, testFile2;
            CreateSubdirectories(WcPath, out dir1, out dir2, out testFile1, out testFile2);

            Uri trueDstUrl = new Uri(ReposUrl + "newDir2/");
            SvnImportArgs a = new SvnImportArgs();
            a.Depth = SvnDepth.Infinity;

            Assert.That(Client.Import(dir1, trueDstUrl, a));

            String cmd = this.RunCommand("svn", "list " + ReposUrl);
            Assert.That(cmd.IndexOf("newDir2") >= 0, "File wasn't imported");

        }

        static void CreateSubdirectories(string WcPath, out string dir1, out string dir2, out string testFile1, out string testFile2)
        {
            dir1 = Path.Combine(WcPath, "subdir");
            Directory.CreateDirectory(dir1);

            dir2 = Path.Combine(dir1, "subsubdir");
            Directory.CreateDirectory(dir2);

            testFile1 = CreateTextFile(WcPath, @"subdir\testfile.txt");
            testFile2 = CreateTextFile(WcPath, @"subdir\subsubdir\testfile2.txt");
        }
    }

}
