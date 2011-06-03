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

/// <summary>
/// Tests the Client::RevPropGet
/// </summary>
namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class GetRevisionPropertyTests : TestBase
	{
		/// <summary>
		///Attempts to Get Properties on a directory in the repository represented by url.
		/// </summary>
		[Test]
		public void TestRevPropGetDir()
		{
			this.RunCommand("svn", "ps --revprop -r HEAD cow moo " + this.ReposUrl);

			string value;
			Assert.That(Client.GetRevisionProperty(this.ReposUrl, SvnRevision.Head, "cow", out value));

			Assert.That(value, Is.EqualTo("moo"), "Wrong property value");
		}
	}
}