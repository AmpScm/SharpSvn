using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

using NUnit.Framework;
using SharpGit.Diff;

namespace SharpGit.Tests
{
    [TestFixture]
    public class DiffTests
    {

        [Test]
        public void SimpleDiff()
        {
            string[] items = { "A", "B", "C", "D", "E", "F", "G" };

            int n = 0;
            foreach(DiffPair<string> dp in new DiffEnumerable<string>(items, items))
            {
                Assert.That(dp.Left, Is.EqualTo(items[n]));
                Assert.That(dp.Right, Is.EqualTo(items[n]));
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffPair<string> dp in new DiffEnumerable<string>(items, new string[] { "B", "E"}))
            {
                Assert.That(dp.Left, Is.EqualTo(items[n]));
                Assert.That(dp.Right, Is.Null.Or.EqualTo(items[n]));
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffPair<string> dp in new DiffEnumerable<string>(new string[] { "A", "B", "C","D","F","G"}, new string[] { "B", "E" }))
            {
                Assert.That(dp.Left, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dp.Right, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dp.Left ?? dp.Right, Is.Not.Null);
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffPair<string> dp in new DiffEnumerable<string>(new string[] { "A", "C", "F", "G" }, new string[] { "A", "B", "D", "E" }))
            {
                Assert.That(dp.Left, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dp.Right, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dp.Left ?? dp.Right, Is.Not.Null);
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffPair<string> dp in new DiffEnumerable<string>(new string[] { "A", "C", "F"}, new string[] { "B", "D", "E","G" }))
            {
                Assert.That(dp.Left, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dp.Right, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dp.Left ?? dp.Right, Is.Not.Null);
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

        }

        [Test]
        public void SimpleBaseDiff()
        {
            string[] items = { "A", "B", "C", "D", "E", "F", "G" };

            int n = 0;
            foreach (DiffTripple<string> dt in new DiffBaseEnumerable<string>(items, items, items))
            {
                Assert.That(dt.Left, Is.EqualTo(items[n]));
                Assert.That(dt.Right, Is.EqualTo(items[n]));
                Assert.That(dt.Base, Is.EqualTo(items[n]));
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffTripple<string> dt in new DiffBaseEnumerable<string>(items, new string[] { "B", "E" }, items))
            {
                Assert.That(dt.Left, Is.EqualTo(items[n]));
                Assert.That(dt.Right, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Base, Is.EqualTo(items[n]));
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffTripple<string> dt in new DiffBaseEnumerable<string>(new string[] { "A", "B", "C", "D", "F", "G" }, new string[] { "B", "E" }, new string[0]))
            {
                Assert.That(dt.Left, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Right, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Base, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Left ?? dt.Right ?? dt.Base, Is.Not.Null);
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffTripple<string> dt in new DiffBaseEnumerable<string>(new string[] { "A", "C", "F", "G" }, new string[] { "A", "B", "D", "E" }, new string[] { "C", "G"}))
            {
                Assert.That(dt.Left, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Right, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Base, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Left ?? dt.Right ?? dt.Base, Is.Not.Null);
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

            n = 0;
            foreach (DiffTripple<string> dt in new DiffBaseEnumerable<string>(new string[] { "A", "C", "F" }, new string[] { "B", "D", "E", "G" }, new string[] { "A","G"}))
            {
                Assert.That(dt.Left, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Right, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Base, Is.Null.Or.EqualTo(items[n]));
                Assert.That(dt.Left ?? dt.Right ?? dt.Base, Is.Not.Null);
                n++;
            }
            Assert.That(n, Is.EqualTo(items.Length));

        }
    }
}
