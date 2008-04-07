using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using System.Collections.ObjectModel;

namespace SharpSvn.Tests.Commands
{
	public class GetSuggestedMergeSourcesTests : TestBase
	{
		[Test]
		public void VerifyCollabNetRepos()
		{
			string dir = GetTempDir();
			SvnMergeSourcesCollection msc;

			SvnTarget me = new SvnUriTarget(new Uri(CollabReposUri, "trunk/"), 16);
			Assert.That(Client.GetSuggestedMergeSources(me, out msc));

			Assert.That(msc, Is.Not.Null);
			Assert.That(msc.Count, Is.EqualTo(3));
			foreach (SvnMergeSource ms in msc)
			{
                Collection<SvnMergesEligibleEventArgs> info;

				Assert.That(Client.GetMergesEligible(me, ms.Uri, out info));
				Assert.That(info, Is.Not.Null);
				
				if (ms.Uri == new Uri(CollabReposUri, "trunk"))
				{
					Assert.That(info.Count, Is.EqualTo(1));
				}
				else if (ms.Uri == new Uri(CollabReposUri, "branches/a"))
				{
					Assert.That(info.Count, Is.EqualTo(0));
				}
				else if (ms.Uri == new Uri(CollabReposUri, "branches/b"))
				{
					Assert.That(info.Count, Is.EqualTo(0));
				}
				else
					Assert.That(false, "Strange branch found");
			}
		}
	}
}
