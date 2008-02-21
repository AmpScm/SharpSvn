// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;
using System.Collections.Generic;

//TODO: Not possible to do the testing yet as the repository
//      contains only one revision.
namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the NSvn.Client.MoveFile method
	/// </summary>
	[TestFixture]
	public class MergeTests : TestBase
	{
		[Test]
		public void RevertToBase()
		{
			string dir = GetTempDir();
			Assert.That(Client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir));
			
			int nChanges = 0;
			Client.Notify += delegate(object sender, SvnNotifyEventArgs e)
			{
				nChanges++;
			};

			Assert.That(Client.Merge(dir, new Uri(CollabReposUri, "trunk/"), new SvnRevisionRange(SvnRevision.Head, 2)));

			Assert.That(nChanges, Is.GreaterThan(5));
		}				
	}
}
