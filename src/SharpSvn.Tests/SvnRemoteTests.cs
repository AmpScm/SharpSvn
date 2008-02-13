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

			cl.List(new Uri("http://sharpsvn.googlecode.com/svn/trunk"), delegate(object Sender, SvnListEventArgs e)
			{
				Assert.That(e.Entry, Is.Not.Null);
				Assert.That(e.Entry.Revision, Is.GreaterThan(0L));
				Assert.That(e.Entry.Author, Is.Not.Null);
				found = true;
			});

			Assert.That(found);

			Collection<SvnListEventArgs> ee;
			cl.GetList(new Uri("http://sharpsvn.googlecode.com/svn/trunk"), out ee);
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
