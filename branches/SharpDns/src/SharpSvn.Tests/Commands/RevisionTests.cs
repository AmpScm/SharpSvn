// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Tests for the Revision class
	/// </summary>
	[TestFixture]
	public class RevisionTests
	{
		[Test]
		public void TestToString()
		{
			SvnRevision revision = SvnRevision.Base;
			Assert.That(revision.ToString(), Is.EqualTo("BASE"));

			revision = SvnRevision.Committed;
			Assert.That(revision.ToString(), Is.EqualTo("COMMITTED"));

			revision = SvnRevision.Head;
			Assert.That(revision.ToString(), Is.EqualTo("HEAD"));

			revision = SvnRevision.Previous;
			Assert.That(revision.ToString(), Is.EqualTo("PREVIOUS"));

			revision = SvnRevision.None;
			Assert.That(revision.ToString(), Is.EqualTo(""));

			revision = SvnRevision.Working;
			Assert.That(revision.ToString(), Is.EqualTo("WORKING"));

			DateTime t = DateTime.UtcNow;
			revision = new SvnRevision(t);
			Assert.That(revision.ToString(), Is.EqualTo("{" + t.ToString("s", CultureInfo.InvariantCulture) + "}"));

			revision = new SvnRevision(42);
			Assert.That(revision.ToString(), Is.EqualTo("42"));
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

			Assert.That( Revision.Parse("42").ToString() , Is.EqualTo( "42"));

			DateTime t = DateTime.Now;
			Assert.That( Revision.Parse(t.ToString(), Is.EqualTo( t.ToString())).ToString() );

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
			SvnRevision r = new SvnRevision(DateTime.Now);
			Assert.That(r.RevisionType, Is.EqualTo(SvnRevisionType.Time));

			r = new SvnRevision(42);
			Assert.That(r.RevisionType, Is.EqualTo(SvnRevisionType.Number));

			r = 42;
			Assert.That(r.RevisionType, Is.EqualTo(SvnRevisionType.Number));

			Assert.That(SvnRevision.Base.RevisionType, Is.EqualTo(SvnRevisionType.Base));
			Assert.That(SvnRevision.Committed.RevisionType, Is.EqualTo(SvnRevisionType.Committed));
			Assert.That(SvnRevision.Head.RevisionType, Is.EqualTo(SvnRevisionType.Head));
			Assert.That(SvnRevision.Previous.RevisionType, Is.EqualTo(SvnRevisionType.Previous));
			Assert.That(SvnRevision.None.RevisionType, Is.EqualTo(SvnRevisionType.None));
			Assert.That(SvnRevision.Working.RevisionType, Is.EqualTo(SvnRevisionType.Working));
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
			Assert.That( rev , Is.EqualTo( new SvnRevision(.Parse(s)));
		}*/
	}
}
