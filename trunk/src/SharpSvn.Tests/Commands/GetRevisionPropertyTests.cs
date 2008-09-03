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
			Assert.That(Client.GetRevisionProperty(this.ReposUrl, "cow", out value));

			Assert.That(value, Is.EqualTo("moo"), "Wrong property value");
		}
	}
}