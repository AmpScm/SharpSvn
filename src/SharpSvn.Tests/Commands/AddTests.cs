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
	/// Tests NSvn::Core::Client::Add
	/// </summary>
	[TestFixture]
	public class AddTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();
			this.ExtractWorkingCopy();
		}

		/// <summary>
		/// Attempts to add a file, checking that the file actually was added
		/// </summary>
		[Test]
		public void TestBasic()
		{
			string testFile = this.CreateTextFile("testfile.txt");

			this.Client.Notify += new EventHandler<SvnNotifyEventArgs>(this.NotifyCallback);
			SvnAddArgs a = new SvnAddArgs();
			a.Depth = SvnDepth.Empty;
			this.Client.Add(testFile, a);

			Assert.That(this.Notifications.Length > 0, "No notification callbacks received");

			Assert.That(this.GetSvnStatus(testFile), Is.EqualTo('A'), "svn st does not report the file as added");

            Assert.That(Client.Commit(WcPath));
		}

		/// <summary>
		/// Creates a subdirectory with items in it, tries to add it non-recursively.
		/// Checks that none of the subitems are added
		/// </summary>
		[Test]
		public void TestAddDirectoryNonRecursively()
		{
			string dir1, dir2, testFile1, testFile2;
			this.CreateSubdirectories(out dir1, out dir2, out testFile1, out testFile2);

			this.Client.Notify += new EventHandler<SvnNotifyEventArgs>(this.NotifyCallback);
			SvnAddArgs a = new SvnAddArgs();
			a.Depth = SvnDepth.Empty;
			// do a non-recursive add here
			this.Client.Add(dir1, a);

			Assert.That(this.Notifications.Length == 1, "Too many or no notifications received. Added recursively?");
			Assert.That(this.GetSvnStatus(dir1), Is.EqualTo('A'), "Subdirectory not added");

			Assert.That(GetSvnStatus(dir2), Is.Not.EqualTo('A'), "Recursive add");
			Assert.That(GetSvnStatus(testFile1), Is.Not.EqualTo('A'), "Recursive add");
			Assert.That(GetSvnStatus(testFile2), Is.Not.EqualTo('A'), "Recursive add");
		}

		/// <summary>
		/// Creates a subdirectory with some items in it. Attempts to add it recursively.
		/// </summary>
		[Test]
		public void TestAddDirectoryRecursively()
		{
			string dir1, dir2, testFile1, testFile2;
			this.CreateSubdirectories(out dir1, out dir2, out testFile1, out testFile2);

			this.Client.Notify += new EventHandler<SvnNotifyEventArgs>(this.NotifyCallback);
			// now a recursive add
			SvnAddArgs a = new SvnAddArgs();
			a.Depth = SvnDepth.Infinity;
			this.Client.Add(dir1, a);

			// enough notifications?
			Assert.That(this.Notifications.Length, Is.EqualTo(4), "Received wrong number of notifications");
			Assert.That(this.GetSvnStatus(dir1), Is.EqualTo('A'), "Subdirectory not added");
			Assert.That(this.GetSvnStatus(dir2), Is.EqualTo('A'), "Subsubdirectory not added");
			Assert.That(this.GetSvnStatus(testFile1), Is.EqualTo('A'), "File in subdirectory not added");
			Assert.That(this.GetSvnStatus(testFile2), Is.EqualTo('A'), "File in subsubdirectory not added");
		}

		[Test]
		public void TestWithForce()
		{
			SvnAddArgs a = new SvnAddArgs();
			a.Depth = SvnDepth.Empty;

			string file = Path.Combine(this.WcPath, "AssemblyInfo.cs");
			try
			{
				this.Client.Add(file, a);
				Assert.Fail("Should have failed");
			}
			catch (SvnException)
			{
				// swallow
			}

			// should not fail
			a.Force = true;
			this.Client.Add(file, a);
		}

		[Test]
		public void TestAddFileWithNonAsciiFilename()
		{
			SvnAddArgs a = new SvnAddArgs();
			a.Depth = SvnDepth.Infinity;

			string newFile = Path.Combine(this.WcPath, "Æeiaæå.ø");
			File.Create(newFile).Close();
			this.Client.Add(newFile, a);
		}


		private void CreateSubdirectories(out string dir1, out string dir2, out string testFile1, out string testFile2)
		{
			dir1 = Path.Combine(this.WcPath, "subdir");
			Directory.CreateDirectory(dir1);

			dir2 = Path.Combine(dir1, "subsubdir");
			Directory.CreateDirectory(dir2);

			testFile1 = this.CreateTextFile(@"subdir\testfile.txt");
			testFile2 = this.CreateTextFile(@"subdir\subsubdir\testfile2.txt");
		}

	}
}
