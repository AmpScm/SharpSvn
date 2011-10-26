using System;
using NUnit;
using SharpSvn;
using NUnit.Framework;
using System.IO;
using System.Diagnostics;
using NUnit.Framework.SyntaxHelpers;

namespace Tests
{
    [TestFixture]
    public class ReposTests
    {
        static string _reposDir;
        static Uri _reposUri;


        static string GetTempDir()
        {
            string tmpDir = SvnTools.GetNormalizedFullPath(Path.GetTempPath() + "\\" + "rp\\" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(tmpDir);

            return tmpDir;
        }

        [TestFixtureSetUp]
        public void TestSetup()
        {
            _reposDir = GetTempDir();
            new SvnRepositoryClient().CreateRepository(_reposDir);

            _reposUri = SvnTools.GetNormalizedUri(new Uri("file:///" + _reposDir.Replace('\\', '/')));
            _reposUri = new Uri(_reposUri.AbsoluteUri + "/");
            using (SvnClient cl = new SvnClient())
            {
                cl.Configuration.LogMessageRequired = false;
                cl.RemoteCreateDirectory(new Uri(_reposUri, "trunk")); // r1
                cl.RemoteCreateDirectory(new Uri(_reposUri, "trunk/A")); // r2
                cl.RemoteCreateDirectory(new Uri(_reposUri, "trunk/B")); // r3
                cl.RemoteCreateDirectory(new Uri(_reposUri, "trunk/C")); // r4
                cl.RemoteCreateDirectory(new Uri(_reposUri, "trunk/D")); // r5


                cl.RemoteDelete(new Uri(_reposUri, "trunk/A")); // r6
                cl.RemoteDelete(new Uri(_reposUri, "trunk/D")); // r7
            }
        }

        [Test]
        public void List()
        {
            using (SvnClient cl = new SvnClient())
            {
                cl.List(_reposUri, (sender, e) => { } );
            }
        }

        [Test]
        public void Status3()
        {
            using (SvnClient cl = new SvnClient())
            {
                string tmp = GetTempDir();
                cl.CheckOut(new Uri(_reposUri, "trunk/"), tmp, new SvnCheckOutArgs { Revision = 3 });
                cl.Status(tmp, new SvnStatusArgs { RetrieveRemoteStatus = true, RetrieveAllEntries=true },
                    (sender, e) =>
                    {
                        string subPath = e.FullPath.Substring(tmp.Length).TrimStart('\\');
                        Debug.WriteLine(string.Format("Path = {0}", subPath));
                        Assert.That(e.Uri, Is.Not.Null);
                        switch (subPath)
                        {
                            case "":
                                break;
                            case "A":
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.RemoteContentStatus, Is.EqualTo(SvnStatus.Deleted));
                                break;
                            case "C":
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.None));
                                Assert.That(e.RemoteContentStatus, Is.EqualTo(SvnStatus.Added));
                                break;
                            case "B":
                                Assert.That(e.LocalContentStatus, Is.EqualTo(SvnStatus.Normal));
                                Assert.That(e.RemoteContentStatus, Is.EqualTo(SvnStatus.None));
                                break;
                            default:
                                throw new InvalidOperationException();
                        }
                    });
            }
        }
    }
}
