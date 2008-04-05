using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpSvn.Tests.Commands;
using System.IO;
using SharpSvn.Diff;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class DiffTests : TestBase
    {
        string indexTheir;
        string indexMine;
        string indexLatest;
        string indexAncestor;

        [TestFixtureSetUp]
        public void Setup()
        {
            string dir = GetTempDir();
            SvnUpdateResult r;

            using (SvnClient client = new SvnClient())
            {
                client.CheckOut(new Uri(CollabReposUri, "trunk/"), dir, out r);
            }

            indexTheir = Path.Combine(dir, "about\\index.html");
            indexMine = Path.Combine(dir, "jobs\\index.html");
            indexLatest = Path.Combine(dir, "products\\index.html");
            indexAncestor = Path.Combine(dir, "support\\index.html");

            Assert.That(File.Exists(indexTheir), "about\\index.html exists");
            Assert.That(File.Exists(indexMine), "jobs\\index.html exists");
            Assert.That(File.Exists(indexLatest), "products\\index.html exists");
            Assert.That(File.Exists(indexAncestor), "support\\index.html exists");
        }

        [Test]
        public void TestUniDiff()
        {
            SvnFileDiff diff;

            Assert.That(SvnFileDiff.TryCreate(indexTheir, indexMine, new SvnFileDiffArgs(), out diff));
            Assert.That(diff, Is.Not.Null);
            Assert.That(diff.HasDifferences, Is.True);
            Assert.That(diff.HasConflicts, Is.False);

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteDifferences(ms, new SvnDiffWriteDifferencesArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine().Contains(indexTheir.Replace('\\', '/')), "The first line contains the path to indexTheir");
                    Assert.That(tr.ReadLine().Contains(indexMine.Replace('\\', '/')), "The second line contains the path to indexMine");
                    Assert.That(tr.ReadLine(), Is.EqualTo("@@ -9,11 +9,12 @@"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(" <body>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(" \t<p>Content for merge tracking early adopter program</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(" \t"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\t<p>This is the index page in the about folder</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\t<p>Here is some content about our company."));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\tWe are really great."));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\tWe have great products."));
                    Assert.That(tr.ReadLine(), Is.EqualTo("-\tWe have a great roadmap.</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("+\t<p>This is the index page in the jobs folder</p>"));

                    // Ok, the rest will be ok
                }
            }
        }

        [Test]
        public void TestUniDiff3()
        {
            SvnFileDiff diff;

            Assert.That(SvnFileDiff.TryCreate(indexTheir, indexMine, indexLatest, new SvnFileDiffArgs(), out diff));
            Assert.That(diff, Is.Not.Null);
            Assert.That(diff.HasDifferences, Is.True);
            Assert.That(diff.HasConflicts, Is.True);

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteDifferences(ms, new SvnDiffWriteDifferencesArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine().Contains(indexTheir.Replace('\\', '/')), "The first line contains the path to indexTheir");
                    Assert.That(tr.ReadLine().Contains(indexMine.Replace('\\', '/')), "The second line contains the path to indexMine");
                    Assert.That(tr.ReadLine(), Is.Null);

                    // EOF
                }
            }

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteMerged(ms, new SvnDiffWriteMergedArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine(), Is.EqualTo("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<html>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"/>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<title></title>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("</head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<body>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>Content for merge tracking early adopter program</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t"));
                    string line = tr.ReadLine();
                    Assert.That(line.StartsWith("<<<<<<< "), "Starts with <<<<<<<");
                    Assert.That(line.Substring(8), Is.EqualTo(indexMine.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the jobs folder</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>List of jobs we need:</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>Obviously we need a web designer.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>We need a product manager.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t</ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("======="));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the products folder.</p>"));
                    line = tr.ReadLine();
                    Assert.That(line.StartsWith(">>>>>>> "), "Starts with >>>>>>>");
                    Assert.That(line.Substring(8), Is.EqualTo(indexLatest.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t"));

                    // Ok, the rest will be ok
                }
            }
        }

        [Test]
        public void TestUniDiff4()
        {
            SvnFileDiff diff;

            Assert.That(SvnFileDiff.TryCreate(indexTheir, indexMine, indexLatest, indexAncestor, new SvnFileDiffArgs(), out diff));
            Assert.That(diff, Is.Not.Null);
            Assert.That(diff.HasDifferences, Is.True);
            Assert.That(diff.HasConflicts, Is.True);

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteDifferences(ms, new SvnDiffWriteDifferencesArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine().Contains(indexTheir.Replace('\\', '/')), "The first line contains the path to indexTheir");
                    Assert.That(tr.ReadLine().Contains(indexMine.Replace('\\', '/')), "The second line contains the path to indexMine");
                    Assert.That(tr.ReadLine(), Is.EqualTo("@@ -1,25 +1,19 @@"));

                    // The rest of this output does not make any sense.. just ignore it
                }
            }

            using (MemoryStream ms = new MemoryStream())
            {
                Assert.That(diff.WriteMerged(ms, new SvnDiffWriteMergedArgs()));
                ms.Position = 0;

                using (TextReader tr = new StreamReader(ms))
                {
                    Assert.That(tr.ReadLine(), Is.EqualTo("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<html>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\"/>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<title></title>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("</head>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo(""));
                    Assert.That(tr.ReadLine(), Is.EqualTo("<body>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>Content for merge tracking early adopter program</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t"));
                    string line = tr.ReadLine();
                    Assert.That(line.StartsWith("<<<<<<< "), "Starts with <<<<<<<");
                    Assert.That(line.Substring(8), Is.EqualTo(indexMine.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the jobs folder</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>List of jobs we need:</p>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>Obviously we need a web designer.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<li>We need a product manager.</li>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t</ol>"));
                    Assert.That(tr.ReadLine(), Is.EqualTo("======="));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t<p>This is the index page in the products folder.</p>"));
                    line = tr.ReadLine();
                    Assert.That(line.StartsWith(">>>>>>> "), "Starts with >>>>>>>");
                    Assert.That(line.Substring(8), Is.EqualTo(indexLatest.Replace('\\', '/')));
                    line = tr.ReadLine();
                    Assert.That(line.StartsWith("<<<<<<< "), "Starts with <<<<<<<");
                    Assert.That(line.Substring(8), Is.EqualTo(indexMine.Replace('\\', '/')));
                    Assert.That(tr.ReadLine(), Is.EqualTo("\t\t<ul>"));

                    // Ok, the rest will be ok
                }
            }
        }
    }
}
