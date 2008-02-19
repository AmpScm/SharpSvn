// $Id$
using System;
using NUnit.Framework;
using System.IO;
using SharpSvn;
using System.Text;
using System.Collections.ObjectModel;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests Client::PropList
    /// </summary>
    [TestFixture]
    public class PropListTest : TestBase
    {
        [SetUp]
        public override void SetUp()
        {
            base.SetUp();

            this.ExtractWorkingCopy();
        }

        [Test]
        public void TestBasic()
        {
            this.RunCommand( "svn", "ps foo bar " + this.WcPath );
            this.RunCommand( "svn", "ps kung foo " + this.WcPath );

			Collection<SvnPropertyListEventArgs> eList;

			Assert.That(Client.GetPropertyList(new SvnPathTarget(WcPath), out eList));

			Assert.That(eList.Count, Is.EqualTo(1));

            Assert.AreEqual( 2, eList[0].Properties.Count,
                "Wrong number of properties" );
            Assert.AreEqual( "bar", eList[0].Properties["foo"].ToString(),
                "Wrong property" );
			Assert.AreEqual("foo", eList[0].Properties["kung"].ToString(),
                "Wrong property" );
        }
    }
}
