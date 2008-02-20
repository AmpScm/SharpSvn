// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

//TODO: Not possible to do the testing yet as the repository
//      contains only one revision.
namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests the NSvn.Client.MoveFile method
	/// </summary>
	//[TestFixture]
	public class MergeTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

			this.ExtractRepos();
			this.ExtractWorkingCopy();
		}

		/// <summary>
		/// Tests rolling back a change with merge
		/// </summary>
		//TODO: Not possible to do the testing yet as the repository
		//      contains only one revision.
		[Ignore("Fails!")]
		[Test]
		public void TestMergeRollBack()
		{
			Uri srcPath = this.ReposUrl;
			string dstPath = this.WcPath;
			string comparePath = Path.Combine(this.WcPath, Path.Combine("doc", "text_r5.txt"));

			SvnDiffMergeArgs a = new SvnDiffMergeArgs();
			a.Depth = SvnDepth.Infinity;
			this.Client.DiffMerge(dstPath, new SvnUriTarget(srcPath, 5), new SvnUriTarget(srcPath, 4), a);

			Assert.That(this.GetSvnStatus(comparePath), Is.EqualTo('D'), "Wrong status");

		}

		/// <summary>
		/// Tests moving changes with merge
		/// </summary>
		[Ignore("Not implemented yet")]
		[Test]
		public void TestMergeChanges()
		{

		}


	}
}
