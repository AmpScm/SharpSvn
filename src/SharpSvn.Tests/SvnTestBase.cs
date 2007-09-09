using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace SharpSvn.Tests
{
	public class SvnTestBase
	{
		string _testPath;
		public string TestPath
		{
			get
			{
				if (_testPath == null)
				{
					_testPath = Path.Combine(Path.GetTempPath(), "SvnTest\\" + GetType().FullName);

					if (!Directory.Exists(_testPath))
						Directory.CreateDirectory(_testPath);
				}
				return _testPath;
			}
		}

        public static void ForcedDeleteDirectory(string dir)
        {
            foreach (FileInfo fif in new DirectoryInfo(dir).GetFiles("*", SearchOption.AllDirectories))
            {
                if ((int)(fif.Attributes & FileAttributes.ReadOnly) != 0)
                    fif.Attributes = FileAttributes.Normal;
            }
            Directory.Delete(dir, true);
        }
	}
}
