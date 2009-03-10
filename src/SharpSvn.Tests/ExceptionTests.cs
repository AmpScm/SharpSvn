using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests
{ 
    [TestFixture]
    public class ExceptionTests :TestBase
    {
        [Test]
        public void StatusCrash()
        {
            string dir = GetTempDir();
            Client.CheckOut(new Uri(CollabReposUri, "trunk"), dir);

            SvnStatusArgs sa = new SvnStatusArgs();
            sa.ThrowOnError = false;
            sa.RetrieveAllEntries = true;
            Assert.That(Client.Status(dir, sa,
                delegate(object sender, SvnStatusEventArgs e)
                {
                    throw new InvalidOperationException();
                }), Is.False);

            Assert.That(sa.LastException.RootCause, Is.InstanceOfType(typeof(InvalidOperationException)));
        }   
    }
}
