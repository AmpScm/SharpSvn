using System;
using System.Collections.Generic;
using System.ComponentModel;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class ListEntries : TestBase
    {

        [Test]
        public void WalkEntries()
        {
            using (SvnWorkingCopyClient wcc = new SvnWorkingCopyClient())
            {
                SvnWorkingCopyEntriesArgs a = new SvnWorkingCopyEntriesArgs();
                a.ShowHidden = true;

                bool touched = false;
                Assert.That(wcc.ListEntries(WcPath, a,
                    delegate(object sender, SvnWorkingCopyEntryEventArgs e)
                    {
                        touched = true;
                    }), Is.True);


                Assert.That(touched);

            }
        }
    }
}
