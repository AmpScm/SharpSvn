// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Collections.ObjectModel;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests Client::PropList
	/// </summary>
	[TestFixture]
	public class PropListTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

			this.ExtractWorkingCopy();
		}

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
