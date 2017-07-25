using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;

namespace SharpSvn.Tests
{
    [TestClass]
    public class ExceptionTests :TestBase
    {
        [TestMethod]
        public void Exception_StatusCrash()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            SvnStatusArgs sa = new SvnStatusArgs();
            sa.ThrowOnError = false;
            sa.RetrieveAllEntries = true;
            Assert.That(Client.Status(sbox.Wc, sa,
                delegate(object sender, SvnStatusEventArgs e)
                {
                    throw new InvalidOperationException();
                }), Is.False);

            Assert.That(sa.LastException.RootCause, Is.InstanceOf(typeof(InvalidOperationException)));
        }
    }
}
