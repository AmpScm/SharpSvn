using System;
using NUnit.Framework;
using System.IO;
using System.Collections;
using System.Xml.Serialization;
using System.Globalization;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests for the Revision class
	/// </summary>
	[TestFixture]
	public class RevisionTest 
    {
        [Test]
        public void TestToString()
        {
            SvnRevision revision = SvnRevision.Base;
            Assert.AreEqual( "BASE", revision.ToString() );

            revision = SvnRevision.Committed;
            Assert.AreEqual( "COMMITTED", revision.ToString() );

            revision = SvnRevision.Head;
            Assert.AreEqual( "HEAD", revision.ToString() );

			revision = SvnRevision.Previous;
            Assert.AreEqual( "PREVIOUS", revision.ToString() );

			revision = SvnRevision.None;
            Assert.AreEqual( "", revision.ToString() );

			revision = SvnRevision.Working;
            Assert.AreEqual( "WORKING", revision.ToString() );

            DateTime t = DateTime.UtcNow;
			revision = new SvnRevision(t);
            Assert.AreEqual( "{" + t.ToString("s", CultureInfo.InvariantCulture) + "}", revision.ToString() );

            revision = new SvnRevision( 42 );
            Assert.AreEqual( "42", revision.ToString() );
        }

        /// <summary>
        /// Test the parse method.
        /// </summary>
        [Test]
        public void TestParse()
        {
            /*this.DoTestParse( "working", Revision.Working );
            this.DoTestParse( "unspecified", Revision.Unspecified );
            this.DoTestParse( "head", Revision.Head );
            this.DoTestParse( "committed", Revision.Committed );
            this.DoTestParse( "base", Revision.Base );
            this.DoTestParse( "previous", Revision.Previous );

            Assert.AreEqual( "42", Revision.Parse("42").ToString() );

            DateTime t = DateTime.Now;
            Assert.AreEqual( t.ToString(), Revision.Parse(t.ToString()).ToString() );

            // this should throw
            try
            {
                DateTime.Parse( "Foo" );
                Assert.Fail( "Foo is not a valid revision" );
            }
            catch( FormatException )
            {}*/
        }

        [Test]
        public void TestSvnRevisionTypes()
        {
            SvnRevision r = new SvnRevision( DateTime.Now );
            Assert.AreEqual( SvnRevisionType.Time, r.RevisionType );

			r = new SvnRevision(42);
			Assert.AreEqual(SvnRevisionType.Number, r.RevisionType);

            r = 42;
			Assert.AreEqual(SvnRevisionType.Number, r.RevisionType);

			Assert.AreEqual(SvnRevisionType.Base, SvnRevision.Base.RevisionType);
			Assert.AreEqual(SvnRevisionType.Committed, SvnRevision.Committed.RevisionType);
			Assert.AreEqual(SvnRevisionType.Head, SvnRevision.Head.RevisionType);
			Assert.AreEqual(SvnRevisionType.Previous, SvnRevision.Previous.RevisionType);
            Assert.AreEqual( SvnRevisionType.None, SvnRevision.None.RevisionType );
			Assert.AreEqual(SvnRevisionType.Working, SvnRevision.Working.RevisionType);
        }

        [Test]
        public void TestGetDateFromOtherTypeOfRevision()
        {
            Assert.That(SvnRevision.Head.Time, Is.EqualTo(DateTime.MinValue));
        }

        /*[Test]
        [ExpectedException(typeof(InvalidOperationException))]
        public void TestGetNumberFromOtherTypeOfRevision()
        {
            int number = Revision.Head.Number;
        }

        private void DoTestParse( string s, SvnRevision rev )
        {
            Assert.AreEqual( new SvnRevision(.Parse(s), rev );
        }*/
	}
}
