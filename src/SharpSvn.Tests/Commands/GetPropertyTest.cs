// $Id$
using System;
using NUnit.Framework;
using System.IO;
using SharpSvn;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Summary description for PropGetTest.
    /// </summary>
    [TestFixture]
    public class PropGetTest : TestBase
    {
        [SetUp]
        public override void SetUp()
        {
            base.SetUp();
            this.ExtractWorkingCopy();
        }

        [Test]
        public void TestPropGetOnFile()
        {
            string path = Path.Combine( this.WcPath, "Form.cs" );
            this.RunCommand( "svn", "ps foo bar " + path );

			string value;
			Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out value));

			Assert.That(value, Is.EqualTo("bar"));

			SvnPropertyValue pval;

			Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out pval));

			Assert.That(pval.StringValue, Is.EqualTo("bar"));
			Assert.That(pval.Key, Is.EqualTo("foo"));
			Assert.That(pval.Target.TargetName, Is.EqualTo(path.Replace(Path.DirectorySeparatorChar, '/')));
        }
    }
}



