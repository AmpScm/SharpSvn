using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using System.IO;

namespace SharpSvn.Tests.Commands
{
    [TestFixture]
    public class FileVersions : TestBase
    {
        [Test]
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

                    using (StreamReader sr = new StreamReader(e.GetContentStream()))
                    {
                        string content = sr.ReadToEnd();

                        Assert.That(content, Is.Not.Null);
                        Assert.That(content.Length, Is.GreaterThan(10));
                    }
                });

            Assert.That(touched);
        }

        [Test, ExpectedException(typeof(SvnFileSystemNodeTypeException))]
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

        [Test]
        public void WalkKeywords()
        {
            Uri repos = GetReposUri(TestReposType.Empty);
            string wc = GetTempDir();
            string file = Path.Combine(wc, "myFile.txt");
            string nl = Environment.NewLine;

            Client.CheckOut(repos, wc);

            File.WriteAllText(file, "Line1 $Id$" + nl + "$HeadURL$" + nl + nl);

            Client.Add(file);
            Client.SetProperty(file, SvnPropertyNames.SvnKeywords, "Id\nHeadURL");
            Client.Commit(file);
            File.AppendAllText(file, "Line" + nl);
            Client.Commit(file);
            Client.SetProperty(file, SvnPropertyNames.SvnEolStyle, "native");
            Client.Commit(file);
            File.AppendAllText(file, "Line" + nl + "Line");
            Client.Commit(file);
            Client.SetProperty(file, SvnPropertyNames.SvnEolStyle, "CR");
            Client.Commit(file);

            string f2 = file + "2";
            Client.Copy(file, f2);
            SvnCommitArgs xa = new SvnCommitArgs();
            xa.LogProperties.Add("extra", "value");
            Client.Commit(wc, xa);
            Client.Update(wc);

            SvnFileVersionsArgs va;

            using (SvnClient c2 = new SvnClient())
            {
                Uri fileUri = c2.GetUriFromWorkingCopy(file);
                Uri f2Uri = c2.GetUriFromWorkingCopy(f2);

                for (int L = 0; L < 2; L++)
                {
                    va = new SvnFileVersionsArgs();
                    va.RetrieveProperties = true;
                    switch (L)
                    {
                        case 0:
                            va.Start = SvnRevision.Zero;
                            va.End = SvnRevision.Head;
                            break;
                        default:
                            break;
                    }

                    int i = 0;
                    Client.FileVersions(f2, va,
                        delegate(object sender, SvnFileVersionEventArgs e)
                        {
                            Assert.That(e.Revision, Is.EqualTo(i + 1L));
                            Assert.That(e.RepositoryRoot, Is.Not.Null);
                            Assert.That(e.Uri, Is.EqualTo(i == 5 ? f2Uri : fileUri));
                            Assert.That(e.Author, Is.EqualTo(Environment.UserName));
                            Assert.That(e.Time, Is.GreaterThan(DateTime.UtcNow - new TimeSpan(0, 0, 10, 0, 0)));
                            Assert.That(e.RevisionProperties.Count, Is.GreaterThanOrEqualTo(3));

                            if (i == 5)
                            {
                                Assert.That(e.RevisionProperties.Contains("extra"), "Contains extra property");
                                Assert.That(e.Properties.Contains(SvnPropertyNames.SvnMergeInfo), Is.True, "Contains merge info in revision 5");
                            }
                            else
                                Assert.That(e.Properties.Contains(SvnPropertyNames.SvnMergeInfo), Is.False, "No mergeinfo");

                            MemoryStream ms1 = new MemoryStream();
                            MemoryStream ms2 = new MemoryStream();

                            e.WriteTo(ms1);
                            c2.Write(new SvnUriTarget(e.Uri, e.Revision), ms2);

                            string s1 = Encoding.UTF8.GetString(ms1.ToArray());
                            string s2 = Encoding.UTF8.GetString(ms2.ToArray());
                            
                            //Assert.That(ms1.Length, Is.EqualTo(ms2.Length), "Export lengths equal");
                            Assert.That(s1, Is.EqualTo(s2));
                            i++;
                        });

                    Assert.That(i, Is.EqualTo(6), "Found 6 versions");
                }
            }
        }

        [Test]
        public void WalkMe()
        {
            SvnFileVersionsArgs a = new SvnFileVersionsArgs();
            SvnUriTarget me = new SvnUriTarget(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk/src/SharpSvn.Tests/Commands/FileVersions.cs"), 931);
            a.End = 931;

            int n = 0;

            Client.FileVersions(me, a,
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    GC.KeepAlive(e);
                    e.WriteTo(GetTempFile());
                    n++;
                });

            Assert.That(n, Is.EqualTo(7));
        }
       
        [Test]
        public void WalkChange()
        {
            SvnFileVersionsArgs a = new SvnFileVersionsArgs();
            SvnUriTarget me = new SvnUriTarget(new Uri("http://sharpsvn.open.collab.net/svn/sharpsvn/trunk/src/SharpSvn.Tests/Commands/FileVersions.cs"), 931);
            a.Start = 927;
            a.End = 928;
            int n = 0;

            Client.FileVersions(me, a,
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    GC.KeepAlive(e);
                    e.WriteTo(GetTempFile());
                    n++;
                });

            Assert.That(n, Is.EqualTo(2));
        }

    }
}

