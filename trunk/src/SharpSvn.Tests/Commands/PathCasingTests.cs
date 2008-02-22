using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.IO;

using System.Runtime.InteropServices;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class PathCasingTests
	{
		readonly string _casedFile;

		[DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
		static extern int GetLongPathName(
			string lpszShortPath,
			[Out] StringBuilder lpszLongPath,
			int cchBuffer);


		public PathCasingTests()
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

			Assert.That(Path.GetFileName(SvnUtils.GetTruePath(path)), Is.EqualTo(Path.GetFileName(_casedFile)));

			string dir = SvnUtils.GetTruePath(Path.GetDirectoryName(_casedFile));

			Assert.That(SvnUtils.GetTruePath(_casedFile.ToUpperInvariant()), Is.EqualTo(Path.Combine(dir, Path.GetFileName(_casedFile))));
		}

	}
}
