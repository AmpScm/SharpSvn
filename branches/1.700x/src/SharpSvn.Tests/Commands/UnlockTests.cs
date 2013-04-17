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
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Summary description for UnlockTest.
	/// </summary>
	[TestFixture]
	public class UnlockTests : TestBase
	{
		[Test]
		public void BasicUnlockTest()
		{
			string filepath = Path.Combine(this.WcPath, "Form.cs");

			this.RunCommand("svn", "lock " + filepath);

			char lockStatus;
			lockStatus = this.RunCommand("svn", "status " + filepath)[5];
			Assert.That(lockStatus, Is.EqualTo('K'), "file not locked");

			this.Client.Unlock(filepath);

			Assert.That(this.RunCommand("svn", "status " + filepath).Length == 0, "file not unlocked");
		}
	}
}
