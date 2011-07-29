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
using System.Diagnostics;
using SharpSvn.Implementation;

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
            foreach (string pdb in Pdbs)
            {
                Assert.That(File.Exists(pdb));

                Collection<string> files;
                Assert.That(SourceServerTools.TryGetAllFiles(pdb, out files), "Can get files from pdb");

                Assert.That(files.Count, Is.GreaterThanOrEqualTo(1), "At least 1 file per pdb");
                using (SvnPdbClient pdbClient = new SvnPdbClient())
                {
                    pdbClient.Open(Path.ChangeExtension(pdb, ".dll"));

                    IList<string> directFiles;
                    pdbClient.GetAllSourceFiles(out directFiles);

                    Assert.That(directFiles.Count, Is.EqualTo(files.Count));
                    Assert.That(directFiles, Is.EquivalentTo(files));
                }
            }
        }

        [Test]
        public void TestPdbStreams()
        {
            foreach(FileInfo fi in new DirectoryInfo(@"g:\AnkhSvn-Daily-2.1.10296.19-symbols").GetFiles("*.pdb"))
            {
                string pdb = fi.FullName;
                using (SvnMsfClient msfClient = new SvnMsfClient())
                {
                    msfClient.Open(pdb);
                    Debug.WriteLine(string.Format("OK: {0}", pdb));
                }
            }
        }

        [Test]
        public void TestPdbStreams2()
        {
            using (SvnMsfClient baseClient = new SvnMsfClient())
            {
                string bn = @"G:\pdb-tests\SharpSvn.pdb";
                baseClient.Open(bn);

                foreach (FileInfo fi in new DirectoryInfo(@"g:\pdb-tests").GetFiles("SharpSvn.*.pdb"))
                {
                    string pdb = fi.FullName;

                    using (SvnMsfClient tstClient = new SvnMsfClient())
                    {
                        tstClient.Open(pdb);
                        if (tstClient.StreamCount != baseClient.StreamCount)
                            Debug.WriteLine(string.Format("Nr of streams {0} vs {1}\n", baseClient.StreamCount, tstClient.StreamCount));
                        for (int i = 0; i < baseClient.StreamCount; i++)
                        {
                            GC.KeepAlive(tstClient.GetStreamNames());

                            using (Stream s1 = baseClient.GetStream(i))
                            using (Stream s2 = tstClient.GetStream(i))
                            {
                                bool different = false;
                                if (s1.Length == s2.Length)
                                {
                                    for (int n = 0; n < s1.Length; n++)
                                    {
                                        if (s1.ReadByte() != s2.ReadByte())
                                        {
                                            Debug.WriteLine(string.Format("Byte difference on stream {0} in {1} at {2}", i, pdb, n));
                                            different = true;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    Debug.WriteLine(string.Format("Length mismatch on stream {0} in {1} ({2} vs {3})", i, pdb, s1.Length, s2.Length));
                                    different = true;
                                }

                                if (different)
                                {
                                    s1.Position = 0;
                                    s2.Position = 0;

                                    byte[] buffer = new byte[s1.Length];
                                    s1.Read(buffer, 0, buffer.Length);
                                    File.WriteAllBytes(bn + "." + i.ToString() +".bin", buffer);
                                    using(StreamWriter sw = File.CreateText(bn + "." + i.ToString()+".txt"))
                                    {
                                        foreach(byte b in buffer)
                                            sw.WriteLine(string.Format("0x{0:X2}", b));
                                    }
                                    buffer = new byte[s2.Length];
                                    s2.Read(buffer, 0, buffer.Length);
                                    File.WriteAllBytes(pdb + "." + i.ToString()+".bin", buffer);
                                    using(StreamWriter sw = File.CreateText(pdb + "." + i.ToString()+".txt"))
                                    {
                                        foreach(byte b in buffer)
                                            sw.WriteLine(string.Format("0x{0:X2}", b));
                                    }
                                }
                            }
                        }
                    }
                }
            }
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
            TextReader tr = SourceServerTools.GetPdbStream(@"g:\dist\release\SharpSvn\SharpSvn.pdb", "srcsrv");

            Assert.That(tr, Is.Not.Null);

            int lines = 0;
            string line;

            while (null != (line = tr.ReadLine()))
            {
                lines++;
                GC.KeepAlive(line);
            }

            Assert.That(lines, Is.GreaterThan(50));
        }

        [Test]
        public void TestAltExtract()
        {
            string tst = @"g:\dist\release\SharpSvn\SharpSvn.pdb";
            TextReader tr = SourceServerTools.GetPdbStream(tst, "srcsrv");
            string allText = tr.ReadToEnd();
            tr.Close();

            string allText2;
            using (SvnMsfClient msf = new SvnMsfClient())
            {
                msf.Open(tst);
                tr = new StreamReader(msf.GetStream("srcsrv"));
                allText2 = tr.ReadToEnd();
                tr.Close();

            }

            Assert.That(allText2, Is.EqualTo(allText.Replace("\r\r", "\r")));
        }
    }
}
