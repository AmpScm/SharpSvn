// $Id$
using System;
using System.IO;
using NUnit.Framework;

using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests for the Client::Export method
    /// </summary>
    [TestFixture]
    public class ExportTest : TestBase
    {
        [SetUp]
        public override void SetUp() 
        {
            base.SetUp();
            this.ExtractRepos();
            this.ExtractWorkingCopy();
            this.newWc = this.FindDirName( Path.Combine( Path.GetTempPath(), "moo" ) );
        }

        [TearDown]
        public override void TearDown()
        {
            base.TearDown();
            Directory.Delete( this.newWc, true );
        }

        /// <summary>
        /// Test export operation from a repository
        /// </summary>
        [Test]
        public void TestExportRepos()
        {            
            this.Client.Export(new SvnUriTarget(ReposUrl, SvnRevision.Head), this.newWc);

            Assert.IsTrue( File.Exists( Path.Combine( this.newWc, "Form.cs" ) ), 
                "Exported file not there" );
            Assert.IsTrue( !Directory.Exists( Path.Combine( this.newWc, SvnClient.AdministrativeDirectoryName ) ), 
                ".svn directory found" );
        }
        /// <summary>
        /// Test export operation from a working copy
        /// </summary>
        [Test]
        public void TestExportWc()
        {   
            this.Client.Export(new SvnPathTarget(this.WcPath), this.newWc);

            Assert.IsTrue( File.Exists( Path.Combine( this.newWc, "Form.cs" ) ), 
                "Exported file not there" );
            Assert.IsTrue( !Directory.Exists( Path.Combine( this.newWc, SvnClient.AdministrativeDirectoryName) ), 
                ".svn directory found" );
        }

        [Test]
        public void TestExportNonRecursive()
        {
			SvnExportArgs a = new SvnExportArgs();
			a.Depth = SvnDepth.Empty;
			SvnUpdateResult r;
            this.Client.Export( this.WcPath, this.newWc, a, out r);
            Assert.AreEqual( 0, Directory.GetDirectories( this.newWc ).Length );
        }       

        private string newWc;

    }
}
