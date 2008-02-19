// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.IO;
using System.Reflection;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Contains tests for various Client functions that don't merit their own test fixture
	/// </summary>
	[TestFixture]
	public class ZMiscTests : TestBase
	{
		public override void SetUp()
		{
			base.SetUp();

			this.ExtractRepos();
			this.ExtractWorkingCopy();
		}

		/// <summary>
		/// Tests the Client::UrlFromPath function.
		/// </summary>
		[Test]
		public void TestUrlFromDirPath()
		{
			string info = this.RunCommand("svn", "info " + this.WcPath);
			string realUrl = this.GetUrl(this.WcPath);
			Uri url = this.Client.GetUriFromWorkingCopy(this.WcPath);

			Assert.That(url.ToString(), Is.EqualTo(realUrl + "/"), "URL wrong");
		}

		[Test]
		public void TestUrlFromFilePath()
		{
			string formPath = Path.Combine(this.WcPath, "Form.cs");
			string realUrl = this.GetUrl(formPath);
			Uri url = this.Client.GetUriFromWorkingCopy(formPath);

			Assert.AreEqual(realUrl, url.ToString(), "URL wrong");

		}

		[Test]
		public void TestUrlFromUnversionedPath()
		{
			Uri url = this.Client.GetUriFromWorkingCopy(@"C:\");
			Assert.IsNull(url, "Url should be null for an unversioned path");
		}

		[Test]
		public void TestUuidFromUrl()
		{
			string realUuid = this.RunCommand("svnlook", "uuid " + this.ReposPath).Trim();

			Guid id;
			Assert.That(this.Client.GetRepositoryIdFromUri(this.ReposUrl, out id));
			Assert.That(id.ToString(), Is.EqualTo(new Guid(realUuid).ToString()), "UUID wrong");
		}

		[Test]
		[ExpectedException(typeof(SvnOperationCanceledException))]
		public void TestCancel()
		{
			//this.Client.AuthBaton.Add( AuthenticationProvider.GetUsernameProvider() );
			this.Client.Cancel += new EventHandler<SvnCancelEventArgs>(this.Cancel);

			this.Client.Update(this.WcPath);

			Assert.IsTrue(this.cancels > 0, "No cancellation callbacks");

			this.Client.Cancel -= new EventHandler<SvnCancelEventArgs>(this.Cancel);
			this.Client.Cancel += new EventHandler<SvnCancelEventArgs>(this.ReallyCancel);
			this.Client.Update(this.WcPath);
		}

		[System.Runtime.InteropServices.DllImport("kernel32.dll")]
		private static extern bool SetEnvironmentVariable(string name, string value);

		[Test]
		[Ignore("Fails on binding")]
		public void TestChangeAdminDirectoryName()
		{
			string newAdminDir = "_svn";
			typeof(SvnClient).InvokeMember("AdministrativeDirectoryName", BindingFlags.SetProperty | BindingFlags.Static | BindingFlags.NonPublic, null, Client, new object[] { newAdminDir });
			try
			{
				Assert.AreEqual(newAdminDir, SvnClient.AdministrativeDirectoryName,
					"Admin directory name should now be " + newAdminDir);

				string newwc = this.FindDirName(Path.Combine(Path.GetTempPath(), "moo"));
				this.Client.CheckOut(this.ReposUrl, newwc);

				Assert.IsTrue(Directory.Exists(Path.Combine(newwc, newAdminDir)),
					"Admin directory with new name not found");
			}
			finally
			{
				typeof(SvnClient).InvokeMember("AdministrativeDirectoryName", BindingFlags.SetProperty | BindingFlags.Static | BindingFlags.NonPublic, null, Client, new object[] { ".svn" });
				Assert.AreEqual(".svn", SvnClient.AdministrativeDirectoryName, "Settings original admin dir failed");
			}
		}

		[Test]
		public void TestHasBinaryProp()
		{
			SvnWorkingCopyState state;
			SvnGetWorkingCopyStateArgs a = new SvnGetWorkingCopyStateArgs();
			a.GetFileData = true;

			Client.GetWorkingCopyState(Path.Combine(this.WcPath, "Form.cs"), out state);

			// first on a file
			Assert.IsTrue(state.IsTextFile);

			Client.GetWorkingCopyState(Path.Combine(this.WcPath, "App.ico"), out state);

			Assert.IsFalse(state.IsTextFile);


			Client.GetWorkingCopyState(this.WcPath, out state);

			// check what happens for a dir
			//Assert.IsFalse(state.IsTextFile);

		}

		/*/// <summary>
		/// Test the Client::IsIgnored method.
		/// </summary>
		[Test]
		public void TestIsFileIgnored()
		{
			string ignored = this.CreateTextFile("foo.bar");
			this.RunCommand("svn", "ps svn:ignore foo.bar " + this.WcPath);

			Assert.IsTrue(this.Client.IsIgnored(ignored));
			Assert.IsFalse(this.Client.IsIgnored(
				Path.Combine(this.WcPath, "Form1.cs")));
		}

		[Test]
		public void TestIsDirectoryIgnored()
		{
			string ignored = Path.Combine(this.WcPath, "Foo");
			Directory.CreateDirectory(ignored);
			this.RunCommand("svn", "ps svn:ignore Foo " + this.WcPath);

			Assert.IsTrue(this.Client.IsIgnored(ignored));
		}*/

		[Test]
		public void TestEnsureConfig()
		{
			string configDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
				"Subversion");
			string renamed = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Subversion.sdlkjhdfgljh");

			if (Directory.Exists(renamed))
				Directory.CreateDirectory(renamed);
			Directory.Move(configDir, renamed);
			try
			{
				SvnClient client = new SvnClient();
				client.LoadConfigurationDefault();
				//ClientConfig config = new ClientConfig();

				Assert.IsTrue(Directory.Exists(configDir));
			}
			finally
			{
				Directory.Delete(configDir, true);
				Directory.Move(renamed, configDir);
			}
		}

		private string GetUrl(string path)
		{
			string info = this.RunCommand("svn", "info " + path);
			return Regex.Match(info, @"URL: (.*)", RegexOptions.IgnoreCase).Groups[1].ToString().Trim();
		}

		private void Cancel(object sender, SvnCancelEventArgs args)
		{
			this.cancels++;
			args.Cancel = false;
		}

		private void ReallyCancel(object sender, SvnCancelEventArgs args)
		{
			this.cancels++;
			args.Cancel = true;
		}

		private int cancels = 0;
	}
}
