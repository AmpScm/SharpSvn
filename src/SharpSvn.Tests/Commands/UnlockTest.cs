using System;
using System.IO;
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Summary description for UnlockTest.
	/// </summary>
	[TestFixture]
    public class UnlockTest : TestBase
	{
        [SetUp]
        public override void SetUp()
        {
            base.SetUp ();
            this.ExtractRepos();
            this.ExtractWorkingCopy();
        }

        [Test]
        public void BasicUnlockTest()
        {
            string filepath = Path.Combine( this.WcPath, "Form.cs" );

            this.RunCommand("svn", "lock " + filepath);
            
            char lockStatus;
            lockStatus = this.RunCommand("svn", "status " + filepath)[5];
            Assert.AreEqual( 'K', lockStatus, "file not locked");
            
            this.Client.Unlock(filepath);

            Assert.IsTrue( this.RunCommand("svn", "status " + filepath).Length == 0, "file not unlocked" );
        }
	}
}
