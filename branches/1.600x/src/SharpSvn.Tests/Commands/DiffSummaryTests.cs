using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace SharpSvn.Tests.Commands
{
	[TestFixture]
	public class DiffSummaryTests : TestBase
	{
		[Test]
		public void SummarizeTrunkBranchB()
		{
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
