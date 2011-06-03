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
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::Revert
	/// </summary>

	[TestFixture]
	public class RevertTests : TestBase
	{
		/// <summary>
		///Attempts to revert single file.
		/// </summary>
		[Test]
		public void TestRevertFile()
		{
			string filePath = Path.Combine(this.WcPath, "Form.cs");

			string oldContents;
			string newContents;
			this.ModifyFile(out oldContents, out newContents, filePath, filePath, SvnDepth.Empty);


			Assert.That(newContents, Is.EqualTo(oldContents), "File not reverted");

		}

		/// <summary>
		///Attempts to revert the whole working copy
		/// </summary>
		[Test]
		public void TestRevertDirectory()
		{
			string oldContents;
			string newContents;
			this.ModifyFile(out oldContents, out newContents, Path.Combine(this.WcPath, "Form.cs"),
				this.WcPath, SvnDepth.Infinity);

			Assert.That(newContents, Is.EqualTo(oldContents), "File not reverted");

		}

		private void ModifyFile(out string oldContents, out string newContents, string filePath,
			string revertPath, SvnDepth depth)
		{

			using (StreamReader reader = new StreamReader(filePath))
				oldContents = reader.ReadToEnd();
			using (StreamWriter writer = new StreamWriter(filePath))
				writer.WriteLine("mooooooo");

			SvnRevertArgs a = new SvnRevertArgs();
			a.Depth = depth;
			this.Client.Revert(revertPath, a);

			using (StreamReader reader = new StreamReader(filePath))
				newContents = reader.ReadToEnd();
		}

	}
}
