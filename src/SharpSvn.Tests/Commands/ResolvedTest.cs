// $Id$
using System;
using NUnit.Framework;
using System.IO;



using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::Resolve
    /// </summary>
    [TestFixture]
    public class ResolveTest : TestBase
    {
        [SetUp]
        public override void SetUp()
        {
            base.SetUp();
            this.path = this.GetTempFile();
			UnzipToFolder(Path.Combine(ProjectBase, "Zips/conflictwc.zip"), path);			
            this.RenameAdminDirs( this.path );
        }

        [TearDown]
        public override void TearDown()
        {
            base.TearDown();
			RecursiveDelete(path);
        }

        /// <summary>
        ///Attempts to resolve a conflicted file. 
        /// </summary>
        [Test]
        public void TestResolveFile()
        {  
            string filePath = Path.Combine( this.path, "Form.cs" );

            this.Client.Resolved( filePath);
 
            Assert.AreEqual('M', this.GetSvnStatus( filePath ), "Resolve didn't work!" );

        }
   
        /// <summary>
        ///Attempts to resolve a conflicted directory recursively. 
        /// </summary>
        [Test]
        public void TestResolveDirectory()
        {
			SvnResolvedArgs a = new SvnResolvedArgs();
			a.Depth = SvnDepth.Infinity;

            this.Client.Resolved( this.path, a);
 
            Assert.AreEqual( 'M', this.GetSvnStatus( this.path ),
                " Resolve didn't work! Directory still conflicted" );
            Assert.AreEqual( 'M', this.GetSvnStatus( Path.Combine( this.path, "Form.cs" ) ),
                "Resolve didn't work! File still conflicted" ); 
        }
    
        private string path;
    }

}
