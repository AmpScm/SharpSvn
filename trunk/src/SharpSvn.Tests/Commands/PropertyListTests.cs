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
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::PropList
	/// </summary>
	[TestFixture]
	public class PropListTests : TestBase
	{
		[Test]
		public void TestBasic()
		{
			this.RunCommand("svn", "ps foo bar " + this.WcPath);
			this.RunCommand("svn", "ps kung foo " + this.WcPath);

			Collection<SvnPropertyListEventArgs> eList;

			Assert.That(Client.GetPropertyList(new SvnPathTarget(WcPath), out eList));

			Assert.That(eList.Count, Is.EqualTo(1));

			Assert.That(eList[0].Properties.Count, Is.EqualTo(2),
				"Wrong number of properties");
			Assert.That(eList[0].Properties["foo"].ToString(), Is.EqualTo("bar"),
				"Wrong property");
			Assert.That(eList[0].Properties["kung"].ToString(), Is.EqualTo("foo"),
				"Wrong property");
		}
	}
}
