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
	/// Tests the NSvn.Client.MakeDir method
	/// </summary>
	[TestFixture]
	public class CreateDirectoryTests : TestBase
	{
		/// <summary>
		/// Tests creating a directory in the working copy
		/// </summary>
		[Test]
		public void TestMakeLocalDir()
		{
			string path = Path.Combine(this.WcPath, "foo");
			Assert.That(Client.CreateDirectory(path));

			Assert.That(this.GetSvnStatus(path), Is.EqualTo('A'), "Wrong status code");
		}

		/// <summary>
		/// Tests creating a directory in the repository
		/// </summary>
		[Test]
		public void TestMakeRepositoryDir()
		{
			Uri url = new Uri(ReposUrl, "mooNewDirectory/");

			Assert.That(Client.RemoteCreateDirectory(url));

			string output = this.RunCommand("svn", "ls " + this.ReposUrl);
			Assert.That(Regex.IsMatch(output, @"mooNewDirectory/"), "No new dir found: " + output);

		}
	}
}
