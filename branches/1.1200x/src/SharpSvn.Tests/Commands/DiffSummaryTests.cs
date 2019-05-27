using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;


namespace SharpSvn.Tests.Commands
{
    [TestClass]
    public class DiffSummaryTests : TestBase
    {
        [TestMethod]
        public void DiffSummary_TrunkBranchB()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            Uri CollabReposUri = sbox.CreateRepository(SandBoxRepository.MergeScenario);

            SvnDiffSummaryArgs sa = new SvnDiffSummaryArgs();
            Client.DiffSummary(new Uri(CollabReposUri, "trunk"), new Uri(CollabReposUri, "branches/b"), sa,
                delegate(object sender, SvnDiffSummaryEventArgs e)
                {
                    Assert.That(e.FromUri.ToString().StartsWith(CollabReposUri.ToString()));
                    Assert.That(e.ToUri.ToString().StartsWith(CollabReposUri.ToString()));

                });
        }
    }
}
