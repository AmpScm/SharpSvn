// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using NUnit.Framework;
using System.IO;

using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	///<summary>
	///Test Client::Switc
	///</summary>
	[TestFixture]
	public class SwitchTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.path = Path.GetTempPath();
			base.ExtractRepos();
			base.ExtractWorkingCopy();
		}

		/// <summary>
		/// Try to switch wc to repos/doc
		/// </summary>
		[Test]
		public void TestSwitchUrl()
		{
			Uri switchUrl = new Uri(this.ReposUrl, "doc");
			string checkFile = Path.Combine(this.WcPath, "text_r5.txt");

			this.Client.Switch(this.WcPath, switchUrl);
			Assert.IsTrue(File.Exists(checkFile), "Didn't switch to repos/doc");

		}
		private string path;
	}
}