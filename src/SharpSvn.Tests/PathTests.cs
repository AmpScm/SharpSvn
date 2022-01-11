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
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using System.IO;

using System.Runtime.InteropServices;
using NUnit.Framework;
using SharpSvn.Tests.Commands;

namespace SharpSvn.Tests
{
    [TestClass]
    public class PathTests : TestBase, IHasTestContext
    {
        readonly string _casedFile;

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern int GetLongPathName(
            string lpszShortPath,
            [Out] StringBuilder lpszLongPath,
            int cchBuffer);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool CreateDirectory(string path, IntPtr secdata);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool RemoveDirectory(string path);

        public PathTests()
        {
            _casedFile = Path.Combine(Path.GetTempPath(), Guid.NewGuid() + "-CaSeD.TxT");
            File.WriteAllText(_casedFile, "hi!");
        }

        [TestCleanup]
        public void DeleteTemp()
        {
            if (File.Exists(_casedFile))
                File.Delete(_casedFile);
        }

        [TestMethod]
        public void Path_FileExists()
        {
            Assert.That(File.Exists(_casedFile), "File exists");
            Assert.That(File.Exists(_casedFile.ToUpperInvariant()), "File exists in uppercase");
            Assert.That(File.Exists(_casedFile.ToLowerInvariant()), "File exists in lowercase");
        }

        string MakeLong(string path)
        {
            StringBuilder sb = new StringBuilder(512);
            int len = GetLongPathName(path, sb, sb.Capacity);

            if (len <= 0)
                return null;
            else if (len >= sb.Capacity - 1)
            {
                sb = new StringBuilder(len + 16);
                len = GetLongPathName(path, sb, sb.Capacity - 1);

                if (len >= sb.Capacity - 1)
                    return null;
            }

            return sb.ToString(0, len);
        }


        [TestMethod]
        public void Path_FixCasing()
        {
            string path = MakeLong(_casedFile.ToUpperInvariant());

            Assert.That(File.Exists(path), "Fixed path exists");

            Assert.That(Path.GetFullPath(path), Is.Not.EqualTo(Path.GetFileName(_casedFile)));

            FileInfo fif = new FileInfo(path);
            Assert.That(fif.Name, Is.Not.EqualTo(Path.GetFileName(_casedFile)));

            Assert.That(Path.GetFileName(SvnTools.GetTruePath(path)), Is.EqualTo(Path.GetFileName(_casedFile)));

            string dir = SvnTools.GetTruePath(Path.GetDirectoryName(_casedFile));

            Assert.That(SvnTools.GetTruePath(_casedFile.ToUpperInvariant()), Is.EqualTo(Path.Combine(dir, Path.GetFileName(_casedFile))));

            Assert.That(SvnTools.GetTruePath("c:\\"), Is.EqualTo(SvnTools.GetTruePath("C:\\")));
            Assert.That(SvnTools.GetTruePath("c:\\-never-exists-"), Is.EqualTo(SvnTools.GetTruePath("C:\\-never-exists-")));
        }

        [TestMethod]
        public void Path_TestNormalizationTesters()
        {
            Assert.That(SvnTools.IsNormalizedFullPath("a:\\"), Is.False, "a:\\ is not normalized");
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\"), Is.True, "A:\\ is normalized");
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\\\"), Is.False, "A:\\\\ is not normalized");
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\.\\"), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\..\\"), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\...\\"), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\..."), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\.svn"), Is.True);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\.svn\\"), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\\\t.svn"), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\.....svn"), Is.True);

            Assert.That(SvnTools.GetTruePath("c:\\"), Is.EqualTo("C:\\"));
            Assert.That(SvnTools.GetTruePath("c:\\" + Guid.NewGuid()), Is.Null);
            Assert.That(SvnTools.GetTruePath("c:\\-never-exists-", true), Is.Not.Null);

            Assert.That(SvnTools.IsAbsolutePath("a:\\"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath("A:\\"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath("A:\\\\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath("A:\\B"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath("A:\\B\\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath("a:/sdfsdfsd"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath("A:\\dfsdfds"), Is.True);

            Assert.That(SvnTools.IsAbsolutePath("a:"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath("A:file"), Is.False);
            Assert.That(Path.IsPathRooted("a:"), Is.True);
            Assert.That(Path.IsPathRooted("A:file"), Is.True);

            Assert.That(SvnTools.IsNormalizedFullPath(@"\\SERVER\path"), Is.False, @"\\SERVER\path");
            Assert.That(SvnTools.IsNormalizedFullPath(@"\\server\path\"), Is.False, @"\\server\path\");
            Assert.That(SvnTools.IsNormalizedFullPath(@"\\server\path"), Is.True, @"\\server\path");
            Assert.That(SvnTools.IsNormalizedFullPath(@"\\server\Path"), Is.True, @"\\server\Path");
            Assert.That(SvnTools.IsNormalizedFullPath(@"\\server\path\file"), Is.True);

            Assert.That(SvnTools.IsAbsolutePath(@"\\\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\path"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\path\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\path\\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\path\dir"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\path\dir\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\path\dir\\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\\server\\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath(@"\server"), Is.False);
        }

        [TestMethod]
        public void Path_TestUriNormalization()
        {
            Assert.That(SvnTools.GetNormalizedUri(new Uri("\\\\server\\repos")).AbsoluteUri, Is.EqualTo("file://server/repos"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("\\\\server\\repos\\file")).AbsoluteUri, Is.EqualTo("file://server/repos/file"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://host/")).AbsoluteUri, Is.EqualTo("http://host/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://host/svn/")).AbsoluteUri, Is.EqualTo("http://host/svn"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://user@host/")).AbsoluteUri, Is.EqualTo("http://user@host/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://user@host/svn/")).AbsoluteUri, Is.EqualTo("http://user@host/svn"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("file://server//repos//qqq")).AbsoluteUri, Is.EqualTo("file://server/repos/qqq"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://sErVeR/")).AbsoluteUri, Is.EqualTo("http://server/"));
            Assert.That((new Uri("http://sErVeR/")).AbsoluteUri, Is.EqualTo("http://server/"));
            Assert.That((new Uri("http://sErVeR/")).ToString(), Is.EqualTo("http://server/"));

            // TODO: Maybe ensure
            //Assert.That(SvnTools.GetNormalizedUri(new Uri("f:\\repos")).AbsoluteUri, Is.EqualTo("file:///F:/repos"));
        }

        [TestMethod]
        public void Path_NormalizePathNormal()
        {
            PathTooLongException ptl = null;
            try
            {
                Assert.That(Path.GetFullPath("c:\\a\\..\\123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                    Is.Not.Null);
            }
            catch (PathTooLongException e)
            {
                ptl = e;
            }

            if (IsCore())
                Assert.That(ptl, Is.Null, "Expected no error on Core");
            else if (Environment.Version.Major < 4)
                Assert.That(ptl, Is.Not.Null, "Expected error in v2.0");
            else
            {
                // In .Net 4.x this is configurable on system and app level
            }
        }

        [TestMethod]
        public void Path_NormalizePathSharp()
        {
            Assert.That(SvnTools.GetNormalizedFullPath("c:\\a\\..\\123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\\"),
                Is.EqualTo("C:\\123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));

            Assert.That(SvnTools.GetNormalizedFullPath("C:\\123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                Is.EqualTo("C:\\123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"), "Shortcut route via IsNormalized");

            if (File.Exists("C:\\Progra~1"))
                Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\Progra~1"), Is.EqualTo("C:\\Program Files"));

            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\asdasdashdadhjadjahjdhasjhajhfasdjsdf\\sdsdfsdfsdfgsdjhfsda hjsdsdf sdaf\\sad sad sad f\\"),
                Is.EqualTo("C:\\asdasdashdadhjadjahjdhasjhajhfasdjsdf\\sdsdfsdfsdfgsdjhfsda hjsdsdf sdaf"));

            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\\\"), Is.Null);
            string drive = Environment.CurrentDirectory.Substring(0, 2).ToLowerInvariant();
            var curDir = Environment.CurrentDirectory;
            if (curDir.Contains("~"))
            {
                // This case triggers on the GitHub bot
                curDir = SvnTools.GetNormalizedFullPath(curDir);
            }
            Assert.That(SvnTools.GetNormalizedDirectoryName(drive), Is.EqualTo(drive.ToUpperInvariant()+Path.GetDirectoryName(curDir).Substring(2))); // CWD on current drive
            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("c:\\a"), Is.EqualTo("C:\\"));

            Assert.That(Path.GetDirectoryName(@"\\Server\Share\Path"), Is.EqualTo(@"\\Server\Share"));
            Assert.That(Path.GetDirectoryName(@"\\Server\Share"), Is.Null);

            Assert.That(Path.GetDirectoryName(@"C:\Dir\Sub\"), Is.EqualTo(@"C:\Dir\Sub"));
            Assert.That(Path.GetDirectoryName(@"C:\Dir\Sub"), Is.EqualTo(@"C:\Dir"));

            Assert.That(SvnTools.GetNormalizedDirectoryName(@"\\Server\Share\Path"), Is.EqualTo(@"\\server\Share"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"\\Server\Share"), Is.Null);

            Assert.That(Path.GetDirectoryName(@"\\server\c$"), Is.Null);
            Assert.That(Path.GetDirectoryName(@"\\server\c$\\"), Is.EqualTo(@"\\server\c$"));

            Assert.That(SvnTools.GetNormalizedFullPath(@"\\server\c$"), Is.EqualTo(@"\\server\c$"));
            Assert.That(SvnTools.GetNormalizedFullPath(@"\\seRver\c$\"), Is.EqualTo(@"\\server\c$"));

            Assert.That(SvnTools.GetNormalizedDirectoryName(@"\\server\c$"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"\\server\c$\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"\\seRver\c$\\"), Is.Null);

            string share = @"\\" + Environment.MachineName.ToLowerInvariant() + @"\C$";
            string shaUC = @"\\" + Environment.MachineName.ToUpperInvariant() + @"\C$";

            Assert.That(SvnTools.IsNormalizedFullPath(share));
            Assert.That(!SvnTools.IsNormalizedFullPath(shaUC));
            Assert.That(!SvnTools.IsNormalizedFullPath(share + "\\"));
            Assert.That(!SvnTools.IsNormalizedFullPath(shaUC + "\\"));

            Assert.That(SvnTools.GetTruePath(share + "\\A\\B", true), Is.EqualTo(share + "\\A\\B"));
            Assert.That(SvnTools.GetTruePath(share + "\\A", true), Is.EqualTo(share + "\\A"));
            Assert.That(SvnTools.GetTruePath(share, true), Is.EqualTo(share));
            Assert.That(SvnTools.GetTruePath(share + "\\", true), Is.EqualTo(share));

            Assert.That(SvnTools.GetTruePath("C:\\A", true), Is.EqualTo("C:\\A"));

            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\dir"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\dir"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\dir\\"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\dir\\"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\sub\dir"), Is.EqualTo(@"C:\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\sub\dir"), Is.EqualTo(@"C:\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\sub\dir\\"), Is.EqualTo(@"C:\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\sub\dir\\"), Is.EqualTo(@"C:\sub"));

            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\dir"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\dir"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\dir\\"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\dir\\"), Is.EqualTo(@"C:\"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\sub\dir"), Is.EqualTo(@"C:\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\sub\dir"), Is.EqualTo(@"C:\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"C:\sub\dir\\"), Is.EqualTo(@"C:\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(@"c:\sub\dir\\"), Is.EqualTo(@"C:\sub"));

            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\dir"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\dir"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\dir\\"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\dir\\"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\sub\dir"), Is.EqualTo(share + @"\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\sub\dir"), Is.EqualTo(share + @"\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\sub\dir\\"), Is.EqualTo(share + @"\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\sub\dir\\"), Is.EqualTo(share + @"\sub"));

            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\dir"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\dir"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\dir\\"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\dir\\"), Is.EqualTo(share));
            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\sub\dir"), Is.EqualTo(share + @"\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\sub\dir"), Is.EqualTo(share + @"\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(share + @"\sub\dir\\"), Is.EqualTo(share + @"\sub"));
            Assert.That(SvnTools.GetNormalizedDirectoryName(shaUC + @"\sub\dir\\"), Is.EqualTo(share + @"\sub"));

            Assert.That(SvnTools.GetNormalizedFullPath("c:\\"), Is.EqualTo("C:\\"));
            Assert.That(SvnTools.GetNormalizedFullPath("C:\\"), Is.EqualTo("C:\\"));
            Assert.That(SvnTools.GetNormalizedFullPath("C:"), Does.StartWith("C:\\"));
            Assert.That(SvnTools.GetNormalizedFullPath("c:"), Does.StartWith("C:\\"));

        }

        [TestMethod]//, ExpectedException(typeof(PathTooLongException), MatchType = MessageMatch.Contains, ExpectedMessage = "rooted")]
        public void Path_NormalizeUnrooted()
        {
            GC.KeepAlive(SvnTools.GetNormalizedFullPath("123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
        }

        [TestMethod]
        public void Path_NormalizePathSharpFail()
        {
            string result = null;
            bool gotException = false;
            try
            {
                result = SvnTools.GetNormalizedFullPath("c:\\<>\\..\\123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                    "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
            }
            catch (ArgumentException)
            {
                Assert.That(Environment.Version.Major, Is.LessThan(4));
                gotException = true;
            }

            if (Environment.Version.Major >= 4 || IsCore())
                Assert.That(result, Is.Not.Null);
            else
                Assert.That(gotException, "Got exception");
        }

        [TestMethod]
        public void Path_NormalizePrefixesArway()
        {
            Assert.That(SvnTools.GetNormalizedFullPath(@"\\?\C:\Windows\Q"), Is.EqualTo(@"C:\Windows\Q"));
            Assert.That(SvnTools.GetNormalizedFullPath(@"\\?\UNC\server\Share\Windows\Q"), Is.EqualTo(@"\\server\Share\Windows\Q"));
        }

        [TestMethod]
        public void Path_TestReallyLong()
        {
            string temp = Path.GetTempPath() + "\\" + Guid.NewGuid() + new String('a', 200);
            temp = temp.Replace("\\\\", "\\");

            if (temp.Length < 256)
                temp += "bbbbbbbbbbbbbbbbbbbb";

            Assert.That(CreateDirectory("\\\\?\\" + temp, IntPtr.Zero), Is.True, "Created directory {0}", temp);

            try
            {
                Assert.That(SvnTools.GetNormalizedFullPath(temp), Is.Not.Null, "Normalized path valid for extremely long path");
                Assert.That(SvnTools.GetTruePath(temp), Is.Not.Null, "True path valid for extremely long path");

                string t2 = temp.Replace("\\", "\\.\\");

                Assert.That(SvnTools.GetNormalizedFullPath(t2), Is.EqualTo(SvnTools.GetNormalizedFullPath(temp)), "True path completed");
                Assert.That(SvnTools.GetTruePath(t2), Is.EqualTo(SvnTools.GetTruePath(temp)));

                string sd = Guid.NewGuid().ToString() + ".tMp";

                string notExisting = Path.Combine(temp, sd);
                string notExistingUpper = Path.Combine(temp.ToUpperInvariant(), sd);

                Assert.That(SvnTools.GetTruePath(notExistingUpper), Is.Null);
                Assert.That(SvnTools.GetTruePath(notExistingUpper, false), Is.Null);
                Assert.That(SvnTools.GetTruePath(notExistingUpper, true), Is.EqualTo(notExisting));

                notExisting = Path.Combine(notExisting, "a\\b.tmp");
                notExistingUpper = Path.Combine(notExistingUpper, "a\\b.tmp");

                Assert.That(SvnTools.GetTruePath(notExistingUpper), Is.Null);
                Assert.That(SvnTools.GetTruePath(notExistingUpper, false), Is.Null);
                Assert.That(SvnTools.GetTruePath(notExistingUpper, true), Is.EqualTo(notExisting));
            }
            finally
            {
                RemoveDirectory("\\\\?\\" + temp);
            }
        }

        [TestMethod]
        public void Path_TryParseShouldReturnFalse()
        {
            SvnPathTarget pt;
            Assert.That(SvnPathTarget.TryParse("http://qqn.nl/2233234", out pt), Is.False);
        }

        [TestMethod]
        public void Path_SshUsernameTests()
        {
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("http://user@host.server/"), "/trunk")).AbsoluteUri, Is.EqualTo("http://user@host.server/trunk"));

            SvnUriTarget target = new SvnUriTarget(new Uri("http://user@host.server/home/user/repos/"), 1234);

            Assert.That(target.Revision.RevisionType, Is.EqualTo(SvnRevisionType.Number));
            Assert.That(target.Uri.AbsoluteUri, Is.EqualTo("http://user@host.server/home/user/repos"));

            SvnUriTarget target2 = new SvnUriTarget(new Uri("http://user@host.server:123/home/user/repos/"), SvnRevisionType.Head);

            Assert.That(target2.Revision, Is.EqualTo(SvnRevision.Head));
            Assert.That(target2.Uri.AbsoluteUri, Is.EqualTo("http://user@host.server:123/home/user/repos"));
        }

        [TestMethod]
        public void Path_TestNormalizeUri()
        {
            Assert.That(SvnTools.GetNormalizedUri(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/")).AbsoluteUri,
                Is.EqualTo("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23"));


            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test/")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test//")).AbsoluteUri, Is.EqualTo("http://localhost/test"));


            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b")).AbsoluteUri, Is.EqualTo("file:///C:/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b/")).AbsoluteUri, Is.EqualTo("file:///C:/a%20b"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a%20b")).AbsoluteUri, Is.EqualTo("file:///C:/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a%20b/")).AbsoluteUri, Is.EqualTo("file:///C:/a%20b"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:///e:/")).AbsoluteUri, Is.EqualTo("file:///E:/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file://e:/")).AbsoluteUri, Is.EqualTo("file:///E:/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:////e:/")).AbsoluteUri, Is.EqualTo("file:///E:/"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:///e:/dir")).AbsoluteUri, Is.EqualTo("file:///E:/dir"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file://e:/dir/")).AbsoluteUri, Is.EqualTo("file:///E:/dir"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:////e:/dir")).AbsoluteUri, Is.EqualTo("file:///E:/dir"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("e:/")).AbsoluteUri, Is.EqualTo("file:///E:/"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("E:\\")).AbsoluteUri, Is.EqualTo("file:///E:/"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share")).AbsoluteUri, Is.EqualTo("file://server/share"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share\")).AbsoluteUri, Is.EqualTo("file://server/share"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share\a")).AbsoluteUri, Is.EqualTo("file://server/share/a"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\server\share\a\")).AbsoluteUri, Is.EqualTo("file://server/share/a"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("HTTP://localhost/test")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("hTTp://uSeR@localhost/test/")).AbsoluteUri, Is.EqualTo("http://uSeR@localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("httP://localhost/test//")).AbsoluteUri, Is.EqualTo("http://localhost/test"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\SERVER\share\a\")).AbsoluteUri, Is.EqualTo("file://server/share/a"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(@"\\SERVER\Share\a\")).AbsoluteUri, Is.EqualTo("file://server/Share/a"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("file://localhost/C:/Repositories/testrepo/TraceStart")).AbsoluteUri, Is.EqualTo("file://localhost/C:/Repositories/testrepo/TraceStart"));
            Assert.That(SvnUriTarget.FromUri(new Uri("file://localhost/C:/Repositories/testrepo/TraceStart")).Uri.AbsoluteUri, Is.EqualTo("file://localhost/C:/Repositories/testrepo/TraceStart"));
        }

        [TestMethod]
        public void Path_TestPathToUri()
        {
            Uri root = new Uri("http://server/q/");
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("\\a b\\test")).AbsoluteUri, Is.EqualTo("http://server/a%20b/test"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("\\a b\\test\\")).AbsoluteUri, Is.EqualTo("http://server/a%20b/test/"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("\\c#\\test\\")).AbsoluteUri, Is.EqualTo("http://server/c%23/test/"));

            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("a b\\test")).AbsoluteUri, Is.EqualTo("http://server/q/a%20b/test"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("a b\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/a%20b/test/"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("c#\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/c%23/test/"));

            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("r\\a b\\test")).AbsoluteUri, Is.EqualTo("http://server/q/r/a%20b/test"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("r\\a b\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/r/a%20b/test/"));
            Assert.That(new Uri(root, SvnTools.PathToRelativeUri("r\\c#\\test\\")).AbsoluteUri, Is.EqualTo("http://server/q/r/c%23/test/"));
        }

        [TestMethod]
        public void Path_UriStrangeness()
        {
            // Somehow the behavior reverts to 2.0 like for the VS Test runner?
            if (Environment.Version.Major < 4 && !IsCore() /* Or .Net 4.0 like runtime, but not .Net 4.5+ */)
            {
                // This is where we wrote this test for
                Assert.That(new Uri("http://server/file.").AbsoluteUri, Is.EqualTo("http://server/file"));
                Assert.That(new Uri("http://server/dir./file.").AbsoluteUri, Is.EqualTo("http://server/dir/file"));
                Assert.That(new Uri("http://server/file.").OriginalString, Is.EqualTo("http://server/file."));
                Assert.That(new Uri("http://server/dir./file.").OriginalString, Is.EqualTo("http://server/dir./file."));
                Assert.That(new Uri("http://server/").AbsoluteUri, Is.EqualTo("http://server/"));
                Assert.That(new Uri("http://server/").OriginalString, Is.EqualTo("http://server/"));
                Assert.That(new Uri("http://server").AbsoluteUri, Is.EqualTo("http://server/"));
                Assert.That(new Uri("http://server").OriginalString, Is.EqualTo("http://server"));

                Assert.That(new Uri("http://server/file%2E").AbsoluteUri, Is.EqualTo("http://server/file"));

            }
            else
            {
                // And in .Net 4.0 many strange cases are gone
                Assert.That(new Uri("http://server/file.").AbsoluteUri, Is.EqualTo("http://server/file."));
                Assert.That(new Uri("http://server/dir./file.").AbsoluteUri, Is.EqualTo("http://server/dir./file."));
                Assert.That(new Uri("http://server/file.").OriginalString, Is.EqualTo("http://server/file."));
                Assert.That(new Uri("http://server/dir./file.").OriginalString, Is.EqualTo("http://server/dir./file."));
                Assert.That(new Uri("http://server/").AbsoluteUri, Is.EqualTo("http://server/"));
                Assert.That(new Uri("http://server/").OriginalString, Is.EqualTo("http://server/"));
                Assert.That(new Uri("http://server").AbsoluteUri, Is.EqualTo("http://server/"));
                Assert.That(new Uri("http://server").OriginalString, Is.EqualTo("http://server"));

                Assert.That(new Uri("http://server/file%2E").AbsoluteUri, Is.EqualTo("http://server/file."));
            }
        }

        [TestMethod]
        public void Path_SvnUriTargetConstructor()
        {
            Assert.That(new SvnUriTarget("file:///C:/some/repos").Uri.OriginalString, Is.EqualTo("file:///C:/some/repos"));
            Assert.That(new SvnUriTarget("file:///c:/some/repos").Uri.OriginalString, Is.EqualTo("file:///C:/some/repos"));
            Assert.That(new SvnUriTarget("file:///C:/some/repos/").Uri.OriginalString, Is.EqualTo("file:///C:/some/repos"));
            Assert.That(new SvnUriTarget("file:///c:/some/repos/").Uri.OriginalString, Is.EqualTo("file:///C:/some/repos"));
            Assert.That(new SvnUriTarget("file://localhost/C:/some/repos").Uri.OriginalString, Is.EqualTo("file://localhost/C:/some/repos"));
            Assert.That(new SvnUriTarget("file://localhost/c:/some/repos").Uri.OriginalString, Is.EqualTo("file://localhost/C:/some/repos"));
            Assert.That(new SvnUriTarget("file://localhost/C:/some/repos/").Uri.OriginalString, Is.EqualTo("file://localhost/C:/some/repos"));
            Assert.That(new SvnUriTarget("file://localhost/c:/some/repos/").Uri.OriginalString, Is.EqualTo("file://localhost/C:/some/repos"));
        }

        [TestMethod]
        public void Path_PathToUri()
        {
            Assert.That(SvnTools.LocalPathToUri(@"c:\TemP\file", false).AbsoluteUri, Is.EqualTo("file:///C:/TemP/file"));
            //Assert.That(SvnTools.SvnTools.LocalPathToUri(@"\\QUAD\public\temp", false), Is.EqualTo("file://quad/public/temp"));
        }

        [TestMethod]
        public void Path_ParsePaths()
        {
            SvnUriTarget ut;
            SvnPathTarget pt;
            SvnTarget st;
            Assert.That(SvnUriTarget.TryParse("http://svn.apache.org/repos/asf/subversion/", out ut));
            Assert.That(ut.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(SvnUriTarget.TryParse("http://svn.apache.org/repos/asf/subversion/@123", out ut));
            Assert.That(ut.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(ut.TargetName.Contains("@"));
            Assert.That(SvnUriTarget.TryParse("http://svn.apache.org/repos/asf/subversion/@123", true, out ut));
            Assert.That(ut.Revision, Is.EqualTo((SvnRevision)123L));

            Assert.That(SvnPathTarget.TryParse("C:\\A", out pt));
            Assert.That(pt.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(SvnPathTarget.TryParse("C:\\A@123", out pt));
            Assert.That(pt.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(pt.TargetName.Contains("@"));
            Assert.That(SvnPathTarget.TryParse("C:\\@123", true, out pt));
            Assert.That(pt.Revision, Is.EqualTo((SvnRevision)123L));

            Assert.That(SvnTarget.TryParse("http://svn.apache.org/repos/asf/subversion/", out st));
            Assert.That(st, Is.InstanceOf(typeof(SvnUriTarget)));
            Assert.That(SvnTarget.TryParse("http://svn.apache.org/repos/asf/subversion/@123", out st));
            Assert.That(st, Is.InstanceOf(typeof(SvnUriTarget)));
            Assert.That(SvnTarget.TryParse("http://svn.apache.org/repos/asf/subversion/@123", true, out st));
            Assert.That(st, Is.InstanceOf(typeof(SvnUriTarget)));

            Assert.That(SvnTarget.TryParse("C:\\A", out st));
            Assert.That(st, Is.InstanceOf(typeof(SvnPathTarget)));
            Assert.That(SvnTarget.TryParse("C:\\A@123", out st));
            Assert.That(st, Is.InstanceOf(typeof(SvnPathTarget)));
            Assert.That(SvnTarget.TryParse("C:\\@123", true, out st));
            Assert.That(st, Is.InstanceOf(typeof(SvnPathTarget)));
        }

        [TestMethod]
        public void Path_UncLocalDriveTests()
        {
            string sysDir = SvnTools.GetTruePath(System.Environment.GetFolderPath(Environment.SpecialFolder.System));
            string testPath = "\\\\" + Environment.MachineName.ToLowerInvariant() + "\\" + sysDir[0] + "$" + sysDir.Substring(2);

            Assert.That(SvnTools.IsAbsolutePath(testPath));

            Assert.That(SvnTools.GetNormalizedFullPath(testPath), Is.EqualTo(testPath),"Fetch normalized");
            Assert.That(new SvnPathTarget(testPath).TargetName, Is.EqualTo(testPath), "PathTarget");

            Assert.That(SvnTools.GetTruePath(testPath, true), Is.EqualTo(testPath), "Fetch truepath");
        }

        [TestMethod]
        public void Path_NormalizingPathsDot()
        {
            Assert.That(SvnTools.IsNormalizedFullPath("C:\\source\\."), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("C:\\source\\.\\"), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("C:\\source\\.\\dump"), Is.False);

            Assert.That(SvnTools.IsAbsolutePath("C:\\source\\."), Is.False);
            Assert.That(SvnTools.IsAbsolutePath("C:\\source\\.\\"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath("C:\\source\\.\\dump"), Is.False);

            Assert.That(SvnPathTarget.FromString("C:\\source\\.").TargetPath, Is.EqualTo("C:\\source"));
            Assert.That(SvnPathTarget.FromString("C:\\source\\.\\").TargetPath, Is.EqualTo("C:\\source"));
            Assert.That(SvnPathTarget.FromString("C:\\source\\.\\dump").TargetPath, Is.EqualTo("C:\\source\\dump"));

            Assert.That(SvnTools.IsNormalizedFullPath("c:\\source"), Is.False);
            Assert.That(SvnTools.IsAbsolutePath("c:\\source"), Is.True);

            Assert.That(SvnPathTarget.FromString("c:\\source\\.").TargetPath, Is.EqualTo("C:\\source"));
            Assert.That(SvnPathTarget.FromString("c:\\source\\.\\").TargetPath, Is.EqualTo("C:\\source"));
            Assert.That(SvnPathTarget.FromString("c:\\source\\.\\dump").TargetPath, Is.EqualTo("C:\\source\\dump"));
        }
    }
}
