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
using NUnit.Framework;
using System.Reflection;
using System.IO;
using NUnit.Framework.SyntaxHelpers;
using System.Collections.ObjectModel;
using SharpSvn.SourceIndexer.Tools;

namespace SharpSvn.SourceIndexer.Tests
{
    [TestFixture]
    public class SrcToolTests
    {
        public SrcToolTests()
        {
            GC.KeepAlive(typeof(SourceServerTools));
            GC.KeepAlive(typeof(SvnClient));
        }
        [Test]
        public void SrcToolFound()
        {
            Assert.That(SourceServerTools.Available, "Source server tools available");
        }

        string[] _pdbs;
        public string[] Pdbs
        {
            get { return _pdbs ?? (_pdbs = GetMyPdbs()); }
        }

        string[] GetMyPdbs()
        {
            List<string> pdbs = new List<string>();;
            Assembly myAsm = typeof(SrcToolTests).Assembly;

            Uri uri;
            if (!Uri.TryCreate(myAsm.CodeBase, UriKind.Absolute, out uri))
                return pdbs.ToArray();;

            string path = Path.GetDirectoryName(Path.GetFullPath(uri.LocalPath)).TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;
            
            foreach (Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                string codebase = assembly.CodeBase;

                if (!Uri.TryCreate(codebase, UriKind.Absolute, out uri) || !uri.IsFile)
                    continue;

                string file = uri.LocalPath;

                if (file.StartsWith(path, StringComparison.OrdinalIgnoreCase))
                {
                    string pdb = Path.ChangeExtension(file, ".pdb");

                    if (File.Exists(pdb))
                        pdbs.Add(pdb);
                }
            }

            return pdbs.ToArray();
        }

        [Test]
        public void HasPdbs()
        {
            Assert.That(Pdbs.Length, Is.GreaterThan(2));
        }

        [Test]
        public void GetUnindexedFiles()
        {
            string pdb = Pdbs[0];

            Assert.That(File.Exists(pdb));

            Collection<string> files;
            Assert.That(SourceServerTools.TryGetNotIndexedFiles(pdb, out files), "Can get files from pdb");

            Assert.That(files.Count, Is.GreaterThanOrEqualTo(1), "At least 1 file per pdb");
        }

        [Test]
        public void GetAllFiles()
        {
            string pdb = Pdbs[0];

            Assert.That(File.Exists(pdb));

            Collection<string> files;
            Assert.That(SourceServerTools.TryGetAllFiles(pdb, out files), "Can get files from pdb");

            Assert.That(files.Count, Is.GreaterThanOrEqualTo(1), "At least 1 file per pdb");
        }

        [Test]
        public void NonIndexed()
        {
            foreach (string pdb in Pdbs)
            {
                Assert.That(SourceServerTools.IsSourceServerIndexed(pdb), Is.False);
            }
        }

        [Test]
        public void TestExtract()
        {
            TextReader tr = SourceServerTools.GetPdbStream(@"f:\QQn\sharpsvn-dist\release\SharpSvn\SharpPlink-Win32.pdb", "srcsrv");

            Assert.That(tr, Is.Not.Null);
        }
    }
}
