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

// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using NUnit.Framework;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::Cat
	/// </summary>
	[TestFixture]
	public class WriteTests : TestBase
	{
		/// <summary>
		/// Attemts to do a cat on a local working copy item
		/// </summary>
		[Test]
		public void TestCatFromWorkingCopy()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");

			string clientOutput = this.RunCommand("svn", "cat " + path);

			MemoryStream stream = new MemoryStream();
			this.Client.Write(path, stream);

			string wrapperOutput = Encoding.ASCII.GetString(stream.ToArray());
			Assert.That(wrapperOutput, Is.EqualTo(clientOutput),
				"String from wrapper not the same as string from client");

		}

		/// <summary>
		/// Calls cat on a repository item
		/// </summary>
		[Test]
		public void TestCatFromRepository()
		{
			Uri path = new Uri(this.ReposUrl, "Form.cs");

			string clientOutput = this.RunCommand("svn", "cat " + path);

			MemoryStream stream = new MemoryStream();
			this.Client.Write(new SvnUriTarget(path, SvnRevision.Head), stream);

			string wrapperOutput = Encoding.ASCII.GetString(stream.ToArray());
			Assert.That(wrapperOutput, Is.EqualTo(clientOutput),
				"String from wrapper not the same as string from client");
		}

		[Test]
		public void TestCatPeg()
		{
			Uri path = new Uri(this.ReposUrl, "Form.cs");
			Uri toPath = new Uri(this.ReposUrl, "Moo.cs");

			SvnCommitResult ci;
			this.Client.RemoteMove(path, toPath, out ci);

			string clientOutput = this.RunCommand("svn",
				string.Format("cat {0}@{1} -r {2}", toPath, ci.Revision, ci.Revision - 1));

			MemoryStream stream = new MemoryStream();
			SvnWriteArgs a = new SvnWriteArgs();
			a.Revision = ci.Revision - 1;
			this.Client.Write(new SvnUriTarget(toPath, ci.Revision), stream);

			string wrapperOutput = Encoding.ASCII.GetString(stream.ToArray());
			Assert.That(wrapperOutput, Is.EqualTo(clientOutput),
				"String from wrapper not the same as string from client");

		}

        [Test]
        public void WriteTest()
        {
            string data = Guid.NewGuid().ToString();
            using (SvnClient client = NewSvnClient(true, false))
            {
                string file = Path.Combine(WcPath, "WriteTest");
                using (StreamWriter sw = File.CreateText(file))
                {
                    sw.WriteLine(data);
                }

                client.Add(file);
                client.AddToChangeList(file, "WriteTest-Items");

                SvnCommitArgs ca = new SvnCommitArgs();
                ca.ChangeLists.Add("WriteTest-Items");
                client.Commit(WcPath);

                using (MemoryStream ms = new MemoryStream())
                {
                    client.Write(new SvnPathTarget(file), ms);

                    ms.Position = 0;

                    using (StreamReader sr = new StreamReader(ms))
                    {
                        Assert.That(sr.ReadLine(), Is.EqualTo(data));
                        Assert.That(sr.ReadToEnd(), Is.EqualTo(""));
                    }
                }

                using (MemoryStream ms = new MemoryStream())
                {
                    client.Write(new Uri(ReposUrl, "WriteTest"), ms);

                    ms.Position = 0;

                    using (StreamReader sr = new StreamReader(ms))
                    {
                        Assert.That(sr.ReadLine(), Is.EqualTo(data));
                        Assert.That(sr.ReadToEnd(), Is.EqualTo(""));
                    }
                }
            }
        }
	}
}
