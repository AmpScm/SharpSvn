// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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
