using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn.Tests.Commands;

namespace SharpSvn.Tests
{
    [TestClass]
    public class ConflictTest : TestBase
    {

        [TestMethod]
        public void Conflict_InstallTreeConflict()
        {
            SvnSandBox sbox = new SvnSandBox(this);
            sbox.Create(SandBoxRepository.Default);

            string index = Path.Combine(sbox.Wc, "README.txt");

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

            Client.Update(sbox.Wc);

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
