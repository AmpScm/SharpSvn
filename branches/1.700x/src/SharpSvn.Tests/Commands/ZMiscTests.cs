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

// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Reflection;
using System.Text.RegularExpressions;
using NUnit.Framework;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Contains tests for various Client functions that don't merit their own test fixture
	/// </summary>
	[TestFixture]
	public class ZMiscTests : TestBase
	{
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

			Assert.That(url.ToString(), Is.EqualTo(realUrl), "URL wrong");

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
			Guid id;
			Assert.That(this.Client.TryGetRepositoryId(this.ReposUrl, out id));
            Assert.That(id.ToString(), Is.EqualTo("c05fa231-13bb-1140-932e-d33687eeb1a3"), "UUID wrong");
		}

        [Test]
        public void TestUuidFromPath()
        {
            Guid id;
            Assert.That(this.Client.TryGetRepositoryId(this.WcPath, out id));
            Assert.That(id.ToString(), Is.EqualTo("c05fa231-13bb-1140-932e-d33687eeb1a3"), "UUID wrong");
        }

        /// <summary>
        /// Tests the cancel.
        /// </summary>
		[Test]
		[ExpectedException(typeof(SvnOperationCanceledException))]
		public void TestCancel()
		{
			//this.Client.AuthBaton.Add( AuthenticationProvider.GetUsernameProvider() );
			this.Client.Cancel += new EventHandler<SvnCancelEventArgs>(this.Cancel);

			this.Client.Update(this.WcPath);

			Assert.That(this.cancels > 0, "No cancellation callbacks");

			this.Client.Cancel -= new EventHandler<SvnCancelEventArgs>(this.Cancel);
			this.Client.Cancel += new EventHandler<SvnCancelEventArgs>(this.ReallyCancel);
			this.Client.Update(this.WcPath);
		}

		[System.Runtime.InteropServices.DllImport("kernel32.dll")]
		private static extern bool SetEnvironmentVariable(string name, string value);

		[Test]
		public void TestChangeAdministrativeDirectoryName()
		{
			string newAdminDir = "_svn";
			PropertyInfo pi = typeof(SvnClient).GetProperty("AdministrativeDirectoryName", BindingFlags.SetProperty | BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public);

			Assert.That(pi, Is.Not.Null);

			pi.SetValue(null, newAdminDir, null);
			try
			{
				Assert.That(SvnClient.AdministrativeDirectoryName, Is.EqualTo(newAdminDir),
					"Admin directory name should now be " + newAdminDir);

				string newwc = GetTempDir();
				this.Client.CheckOut(this.ReposUrl, newwc);

				Assert.That(Directory.Exists(Path.Combine(newwc, newAdminDir)),
					"Admin directory with new name not found");

				Assert.That(Directory.Exists(Path.Combine(newwc, ".svn")), Is.False,
					"Admin directory with old name found");
			}
			finally
			{
				pi.SetValue(null, ".svn", null);
				Assert.That(SvnClient.AdministrativeDirectoryName, Is.EqualTo(".svn"), "Settings original admin dir failed");
			}
		}

		[Test]
		public void TestHasBinaryProp()
		{
			SvnWorkingCopyState state;
			SvnWorkingCopyStateArgs a = new SvnWorkingCopyStateArgs();
			a.RetrieveFileData = true;

            SvnWorkingCopyClient wcc = new SvnWorkingCopyClient();
			wcc.GetState(Path.Combine(this.WcPath, "Form.cs"), out state);

			// first on a file
			Assert.That(state.IsTextFile);

			wcc.GetState(Path.Combine(this.WcPath, "App.ico"), out state);

			Assert.That(state.IsTextFile, Is.False);


			wcc.GetState(this.WcPath, out state);

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

			Assert.That(this.Client.IsIgnored(ignored));
			Assert.IsFalse(this.Client.IsIgnored(
				Path.Combine(this.WcPath, "Form1.cs")));
		}

		[Test]
		public void TestIsDirectoryIgnored()
		{
			string ignored = Path.Combine(this.WcPath, "Foo");
			Directory.CreateDirectory(ignored);
			this.RunCommand("svn", "ps svn:ignore Foo " + this.WcPath);

			Assert.That(this.Client.IsIgnored(ignored));
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

				Assert.That(Directory.Exists(configDir));
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
