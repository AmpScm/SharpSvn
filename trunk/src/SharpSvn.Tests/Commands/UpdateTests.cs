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
	/// Tests Client::Update
	/// </summary>
	[TestFixture]
	public class UpdateTests : TestBase
	{
		[SetUp]
		public override void SetUp()
		{
			base.SetUp();

            this.wc2 = GetTempDir();

			UnzipToFolder(Path.Combine(ProjectBase, "Zips/wc.zip"), wc2);
			this.RenameAdminDirs(this.wc2);

			SvnClient cl = new SvnClient(); // Fix working copy to real location
			cl.Relocate(wc2, new Uri("file:///tmp/repos/"), ReposUrl);
		}

		public override void TearDown()
		{
			base.TearDown();
			RecursiveDelete(this.wc2);
		}

		[Test]
		public void RevTests()
		{
			string dir = GetTempDir();

			SvnUpdateResult result;
			Assert.That(Client.CheckOut(new SvnUriTarget(new Uri(CollabReposUri, "trunk")), dir, out result));

			long head = result.Revision;

			Assert.That(Client.Update(dir, out result));
			Assert.That(result.Revision, Is.EqualTo(head));

			SvnUpdateArgs ua = new SvnUpdateArgs();

			ua.Revision = head - 5;

			Assert.That(Client.Update(dir, ua, out result));
			Assert.That(result.Revision, Is.EqualTo(head-5));
		}

		/// <summary>
		/// Deletes a file, then calls update on the working copy to restore it 
		/// from the text-base
		/// </summary>
		[Test]
		public void TestDeletedFile()
		{
			string filePath = Path.Combine(this.WcPath, "Form.cs");
			File.Delete(filePath);
			this.Client.Update(this.WcPath);

			Assert.That(File.Exists(filePath), "File not restored after update");
		}

		/// <summary>
		/// Changes a file in a secondary working copy and commits. Updates the 
		/// primary wc and compares
		/// </summary>
		[Test]
		public void TestChangedFile()
		{
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "Form.cs")))
				w.Write("Moo");
			this.RunCommand("svn", "ci -m \"\" " + this.wc2);

			this.Client.Update(this.WcPath);

			string s;
			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "Form.cs")))
				s = r.ReadToEnd();

			Assert.That(s, Is.EqualTo("Moo"), "File not updated");
		}

        [Test]
        public void TestNotify()
        {
            int n = 0;;
            SvnUpdateArgs ua = new SvnUpdateArgs();
            ua.Notify += delegate(object sender, SvnNotifyEventArgs e)
            {
                Assert.That(e.FullPath, Is.EqualTo(WcPath));
                switch (n++)
                {
                    case 0:
                        Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.UpdateUpdate));
                        break;
                    case 1:
                        Assert.That(e.Action, Is.EqualTo(SvnNotifyAction.UpdateCompleted));
                        break;
                }
            };

            Client.Update(WcPath, ua);

            n = 0;

            Client.Update(new string[] { WcPath }, ua);
        }

        [Test]
        public void TestUpdateAutoMerge()
        {
            TestUpdateAutoMerge(true);
        }

        [Test]
        public void TestUpdateNoAutoMerge()
        {
            TestUpdateAutoMerge(false);
        }

        void TestUpdateAutoMerge(bool autoMerge)
        {
            using (SvnClient client1 = new SvnClient())
            using (SvnClient client2 = new SvnClient())
            {
                client1.Configuration.LogMessageRequired = client2.Configuration.LogMessageRequired = false;
                client1.Configuration.UseSubversionAutomaticMerge = client2.Configuration.UseSubversionAutomaticMerge = autoMerge;

                client2.BeforeAutomaticMerge += new EventHandler<SvnBeforeAutomaticMergeEventArgs>(client2_BeforeAutomaticMerge);

                string wc1 = GetTempDir();
                string wc2 = GetTempDir();
                client1.CheckOut(ReposUrl, wc1);
                client2.CheckOut(ReposUrl, wc2);

                string fileName = "TheFile";

                TouchFile(Path.Combine(wc1, fileName));
                File.AppendAllText(Path.Combine(wc1, fileName), "1\n2\n3\n4\n5\n6");
                client1.Add(Path.Combine(wc1, fileName));
                client1.Commit(wc1);
                client1.Update(wc1);
                client2.Update(wc2);

                File.WriteAllText(Path.Combine(wc1, fileName), "1\n1b2\n3\n3b\n4\n5\n6");
                File.AppendAllText(Path.Combine(wc2, fileName), "\n7");

                client1.Commit(wc1);
                client1.Update(wc1);
                client2.Update(wc2);

                client2.Status(Path.Combine(wc2, fileName),
                    delegate(object sender, SvnStatusEventArgs e)
                    {
                        Assert.That(e.LocalContentStatus == SvnStatus.Conflicted, Is.EqualTo(!autoMerge));
                    });
            }
        }

        void client2_BeforeAutomaticMerge(object sender, SvnBeforeAutomaticMergeEventArgs e)
        {
            GC.KeepAlive(e);
            //throw new NotImplementedException();
        }

		[Test]
		public void TestUpdateMultipleFiles()
		{
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "Form.cs")))
				w.Write("Moo");
			using (StreamWriter w = new StreamWriter(Path.Combine(this.wc2, "AssemblyInfo.cs")))
				w.Write("Moo");
			this.RunCommand("svn", "ci -m \"\" " + this.wc2);

			SvnUpdateArgs a = new SvnUpdateArgs();
			a.Depth = SvnDepth.Empty;

			SvnUpdateResult result;

			Assert.That(Client.Update(new string[]{ 
                                                             Path.Combine( this.WcPath, "Form.cs" ),
                                                             Path.Combine( this.WcPath, "AssemblyInfo.cs" )
                                                         }, a, out result));
			Assert.That(result.ResultMap.Count, Is.EqualTo(2));

			string s;
			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "Form.cs")))
				s = r.ReadToEnd();
			Assert.That(s, Is.EqualTo("Moo"), "File not updated");

			using (StreamReader r = new StreamReader(Path.Combine(this.WcPath, "AssemblyInfo.cs")))
				s = r.ReadToEnd();
			Assert.That(s, Is.EqualTo("Moo"), "File not updated");
		}

		private string wc2;
	}
}
