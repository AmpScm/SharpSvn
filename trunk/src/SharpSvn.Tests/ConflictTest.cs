using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.Tests.Commands;
using NUnit.Framework;
using System.IO;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests
{
	[TestFixture]
	public class ConflictTest : TestBase
	{

		[Test]
		public void InstallTreeConflict()
		{
			Uri repos = CollabReposUri;
			string wcPath = GetTempDir();
			Client.CheckOut(new SvnUriTarget(new Uri(repos, "trunk/"), 6), wcPath);

			string index = Path.Combine(wcPath, "index.html");

			SvnInfoEventArgs before;
			Client.GetInfo(index, out before);

			Assert.That(before, Is.Not.Null);
			Assert.That(before.ConflictWork, Is.Null);
			Assert.That(before.TreeConflict, Is.Null);

			Uri reposRoot = Client.GetRepositoryRoot(index);

			using (SvnWorkingCopyClient wcc = new SvnWorkingCopyClient())
			{
				SvnUriOrigin conflictOrigin = new SvnUriOrigin(new SvnUriTarget(before.Uri, before.Revision), reposRoot);
				SvnWorkingCopyInstallConflictArgs ia = new SvnWorkingCopyInstallConflictArgs();

				ia.TreeConflict = true; // Only supported conflict at this time

				wcc.InstallConflict(index, conflictOrigin, conflictOrigin, ia);
			}

			Client.Update(wcPath);

			SvnInfoEventArgs after;
			Client.GetInfo(index, out after);

			Assert.That(after, Is.Not.Null);
			Assert.That(after.ConflictWork, Is.Null);
			Assert.That(after.TreeConflict, Is.Not.Null);
			Assert.That(after.Revision, Is.EqualTo(before.Revision));

			Client.Resolve(index, SvnAccept.Working);
		}

	}
}
