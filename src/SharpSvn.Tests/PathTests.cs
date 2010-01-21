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
using System.IO;

using System.Runtime.InteropServices;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class PathTests
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

        [TestFixtureTearDown]
        public void DeleteTemp()
        {
            if (File.Exists(_casedFile))
                File.Delete(_casedFile);
        }

        [Test]
        public void FileExists()
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


        [Test]
        public void FixCasing()
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

        [Test]
        public void TestNormalizationTesters()
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

        }

        [Test]
        public void TestUriNormalization()
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

        [Test, ExpectedException(typeof(PathTooLongException))]
        public void NormalizePathNormal()
        {
            Assert.That(Path.GetFullPath("c:\\a\\..\\123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                Is.Null, "Should never complete");
        }

        [Test]
        public void NormalizePathSharp()
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


            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\asdasdashdadhjadjahjdhasjhajhfasdjsdf\\sdsdfsdfsdfgsdjhfsda hjsdsdf sdaf\\sad sad sad f\\"),
                Is.EqualTo("C:\\asdasdashdadhjadjahjdhasjhajhfasdjsdf\\sdsdfsdfsdfgsdjhfsda hjsdsdf sdaf"));

            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("C:"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("C:\\\\"), Is.Null);
            Assert.That(SvnTools.GetNormalizedDirectoryName("c:\\a"), Is.EqualTo("C:\\"));
        }

        [Test]//, ExpectedException(typeof(PathTooLongException), MatchType = MessageMatch.Contains, ExpectedMessage = "rooted")]
        public void NormalizeUnrooted()
        {
            GC.KeepAlive(SvnTools.GetNormalizedFullPath("123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"));
        }

        [Test, ExpectedException(typeof(ArgumentException))]
        public void NormalizePathSharpFail()
        {
            Assert.That(SvnTools.GetNormalizedFullPath("c:\\<>\\..\\123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890" +
                "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                Is.Null, "Should never complete");
        }

        [Test]
        public void NormalizePrefixesArway()
        {
            Assert.That(SvnTools.GetNormalizedFullPath(@"\\?\C:\Windows\Q"), Is.EqualTo(@"C:\Windows\Q"));
            Assert.That(SvnTools.GetNormalizedFullPath(@"\\?\UNC\server\Share\Windows\Q"), Is.EqualTo(@"\\server\Share\Windows\Q"));
        }

        [Test]
        public void TestReallyLong()
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
                Assert.That(SvnTools.GetFullTruePath(temp), Is.Not.Null, "Full true path valid for extremely long path");

                string t2 = temp.Replace("\\", "\\.\\");

                Assert.That(SvnTools.GetNormalizedFullPath(t2), Is.EqualTo(SvnTools.GetNormalizedFullPath(temp)), "True path completed");
                Assert.That(SvnTools.GetTruePath(t2), Is.EqualTo(SvnTools.GetFullTruePath(temp)));
                Assert.That(SvnTools.GetFullTruePath(t2), Is.EqualTo(SvnTools.GetFullTruePath(temp)));


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

        [Test]
        public void TryParseShouldReturnFalse()
        {
            SvnPathTarget pt;
            Assert.That(SvnPathTarget.TryParse("http://qqn.nl/2233234", out pt), Is.False);
        }

        [Test]
        public void SshUsernameTests()
        {
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("http://user@host.server/"), "/trunk")).AbsoluteUri, Is.EqualTo("http://user@host.server/trunk"));

            SvnUriTarget target = new SvnUriTarget(new Uri("http://user@host.server/home/user/repos/"), 1234);

            Assert.That(target.Revision.RevisionType, Is.EqualTo(SvnRevisionType.Number));
            Assert.That(target.Uri.AbsoluteUri, Is.EqualTo("http://user@host.server/home/user/repos"));

            SvnUriTarget target2 = new SvnUriTarget(new Uri("http://user@host.server:123/home/user/repos/"), SvnRevisionType.Head);

            Assert.That(target2.Revision, Is.EqualTo(SvnRevision.Head));
            Assert.That(target2.Uri.AbsoluteUri, Is.EqualTo("http://user@host.server:123/home/user/repos"));
        }

        [Test]
        public void TestNormalizeUri()
        {
            Assert.That(SvnTools.GetNormalizedUri(new Uri("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23/")).AbsoluteUri,
                Is.EqualTo("https://svn.apache.org/repos/asf/incubator/lucene.net/trunk/C%23"));


            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test/")).AbsoluteUri, Is.EqualTo("http://localhost/test"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("http://localhost/test//")).AbsoluteUri, Is.EqualTo("http://localhost/test"));


            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a b/")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a%20b")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri(new Uri("file:///c:/"), "a%20b/")).AbsoluteUri, Is.EqualTo("file:///c:/a%20b"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:///e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file://e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));
            Assert.That(SvnTools.GetNormalizedUri(new Uri("file:////e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));

            Assert.That(SvnTools.GetNormalizedUri(new Uri("e:/")).AbsoluteUri, Is.EqualTo("file:///e:/"));

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

        }

        [Test]
        public void TestPathToUri()
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

        [Test]
        public void UriStrangeness()
        {
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

        [Test]
        public void ParsePaths()
        {
            SvnUriTarget ut;
            SvnPathTarget pt;
            SvnTarget st;
            Assert.That(SvnUriTarget.TryParse("http://svn.collab.net/repos/svn/", out ut));
            Assert.That(ut.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(SvnUriTarget.TryParse("http://svn.collab.net/repos/svn/@123", out ut));
            Assert.That(ut.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(ut.TargetName.Contains("@"));
            Assert.That(SvnUriTarget.TryParse("http://svn.collab.net/repos/svn/@123", true, out ut));
            Assert.That(ut.Revision, Is.EqualTo((SvnRevision)123L));

            Assert.That(SvnPathTarget.TryParse("C:\\A", out pt));
            Assert.That(pt.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(SvnPathTarget.TryParse("C:\\A@123", out pt));
            Assert.That(pt.Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(pt.TargetName.Contains("@"));
            Assert.That(SvnPathTarget.TryParse("C:\\@123", true, out pt));
            Assert.That(pt.Revision, Is.EqualTo((SvnRevision)123L));

            Assert.That(SvnTarget.TryParse("http://svn.collab.net/repos/svn/", out st));
            Assert.That(st, Is.InstanceOfType(typeof(SvnUriTarget)));
            Assert.That(SvnTarget.TryParse("http://svn.collab.net/repos/svn/@123", out st));
            Assert.That(st, Is.InstanceOfType(typeof(SvnUriTarget)));
            Assert.That(SvnTarget.TryParse("http://svn.collab.net/repos/svn/@123", true, out st));
            Assert.That(st, Is.InstanceOfType(typeof(SvnUriTarget)));

            Assert.That(SvnTarget.TryParse("C:\\A", out st));
            Assert.That(st, Is.InstanceOfType(typeof(SvnPathTarget)));
            Assert.That(SvnTarget.TryParse("C:\\A@123", out st));
            Assert.That(st, Is.InstanceOfType(typeof(SvnPathTarget)));
            Assert.That(SvnTarget.TryParse("C:\\@123", true, out st));
            Assert.That(st, Is.InstanceOfType(typeof(SvnPathTarget)));
        }
    }
}
