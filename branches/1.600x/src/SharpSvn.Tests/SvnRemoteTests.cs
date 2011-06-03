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
using NUnit.Framework.SyntaxHelpers;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests
{
	[TestFixture]
	public class SvnRemoteTests
	{
		[Test]
		public void RemoteList()
		{
			SvnClient cl = new SvnClient();
			bool found = false;
            SvnListArgs la = new SvnListArgs();
            la.RetrieveEntries = SvnDirEntryItems.AllFieldsV15;

			cl.List(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk"), la, delegate(object Sender, SvnListEventArgs e)
			{
				Assert.That(e.Entry, Is.Not.Null);
				Assert.That(e.Entry.Revision, Is.GreaterThan(0L));
				Assert.That(e.Entry.Author, Is.Not.Null);
				found = true;
			});

			Assert.That(found);

			Collection<SvnListEventArgs> ee;
            cl.GetList(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk"), out ee);
			Assert.That(ee, Is.Not.Null);
			Assert.That(ee[0].Entry.Author, Is.Not.Null);
		}

		public void TestSsh()
		{
			SvnClient cl = new SvnClient();
			bool found = false;

			cl.List(new Uri("svn+ssh://nietver.nl/home/svn/repos/openwrt"), delegate(object Sender, SvnListEventArgs e)
			{
				Assert.That(e.Entry, Is.Not.Null);
				Assert.That(e.Entry.Revision, Is.GreaterThan(0L));
				Assert.That(e.Entry.Author, Is.Not.Null);
				found = true;
			});

			Assert.That(found);
		}
	}
}
