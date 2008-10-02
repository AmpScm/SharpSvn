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
			if(File.Exists(_casedFile))
				File.Decrypt(_casedFile);
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
			else if(len >= sb.Capacity-1)
			{
				sb = new StringBuilder(len+16);
				len = GetLongPathName(path, sb, sb.Capacity-1);

				if(len >= sb.Capacity-1)
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

            Assert.That(Path.GetFullPath("C:\\"), Is.Not.EqualTo(Path.GetFullPath("c:\\")));
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
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\\t.svn"), Is.False);
            Assert.That(SvnTools.IsNormalizedFullPath("A:\\.....svn"), Is.True);


            Assert.That(SvnTools.IsAbsolutePath("a:\\"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath("A:\\"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath("a:/sdfsdfsd"), Is.True);
            Assert.That(SvnTools.IsAbsolutePath("A:\\dfsdfds"), Is.True);

            Assert.That(SvnTools.IsNormalizedFullPath(@"\\SERVER\path"), Is.True, @"\\SERVER\path");
            Assert.That(SvnTools.IsNormalizedFullPath(@"\\server\path\"), Is.False, @"\\server\path\");
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
        }

        [Test, ExpectedException(typeof(PathTooLongException), MatchType = MessageMatch.Contains, ExpectedMessage = "rooted")]
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


	}
}
