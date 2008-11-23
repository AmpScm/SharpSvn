using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class FileVersions : TestBase
    {
        [Test, Obsolete]
        public void ListIndex()
        {
            bool touched = false;
            SvnFileVersionsArgs fa = new SvnFileVersionsArgs();
            fa.RetrieveProperties = true;
            Client.FileVersions(new Uri(CollabReposUri, "trunk/index.html"),
                fa,
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    touched = true;
                    Assert.That(e.RevisionProperties, Is.Not.Null, "Revision properties available");
                    Assert.That(e.Properties, Is.Not.Null, "Properties available");
                    
                    Assert.That(e.RevisionProperties.Contains(SvnPropertyNames.SvnAuthor));
                    Assert.That(e.Properties.Contains(SvnPropertyNames.SvnEolStyle));

                    Assert.That(e.Revision, Is.GreaterThan(0));
                    Assert.That(e.Author, Is.Not.Null);
                    Assert.That(e.LogMessage, Is.Not.Null);
                    Assert.That(e.Time, Is.LessThan(DateTime.Now));
                });

            Assert.That(touched);
        }

        [Test, Obsolete, ExpectedException(typeof(SvnFileSystemNodeTypeException))]
        public void ListIndexDir()
        {
            bool touched = false;
            Client.FileVersions(new Uri(CollabReposUri, "trunk/"),
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    touched = true;
                });

            Assert.That(touched);
        }

    }
}

