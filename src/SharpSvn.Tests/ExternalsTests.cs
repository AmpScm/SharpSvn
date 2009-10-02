// $Id$
//
// Copyright 2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests
{
    [TestFixture]
    public class ExternalsTests
    {
        [Test]
        public void ParseSome()
        {
            SvnExternalItem[] items;

            Assert.That(SvnExternalItem.TryParse("dir http://sharpsvn.net/qqn", out items));
            Assert.That(items, Is.Not.Null);
            Assert.That(items.Length, Is.EqualTo(1));
            Assert.That(items[0].Target, Is.EqualTo("dir"));
            Assert.That(items[0].Reference, Is.EqualTo("http://sharpsvn.net/qqn"));
            Assert.That(items[0].Revision, Is.EqualTo(SvnRevision.None));
            Assert.That(items[0].OperationalRevision, Is.EqualTo(SvnRevision.None));

            Assert.That(items[0].ToString(), Is.EqualTo("dir http://sharpsvn.net/qqn"));
            Assert.That(items[0].ToString(false), Is.EqualTo("http://sharpsvn.net/qqn dir"));
        }

        [Test]
        public void ParseHEAD()
        {
            SvnExternalItem item;

            Assert.That(SvnExternalItem.TryParse("-r 1 http://sharpsvn.net/qqn@124 dir", out item));
            Assert.That(item, Is.Not.Null);
            Assert.That(item.Target, Is.EqualTo("dir"));
            Assert.That(item.Reference, Is.EqualTo("http://sharpsvn.net/qqn"));

            // Blanks returns head -> We create blanks
            Assert.That(item.Revision.Revision, Is.EqualTo(1));
            Assert.That(item.OperationalRevision.Revision, Is.EqualTo(124));

            Assert.That(item.ToString(), Is.EqualTo("-r 1 http://sharpsvn.net/qqn@124 dir"));
            Assert.That(item.ToString(false), Is.EqualTo("-r 1 http://sharpsvn.net/qqn@124 dir"));
        }

        [Test]
        public void ParseFail()
        {
            SvnExternalItem[] items;

            Assert.That(SvnExternalItem.TryParse("", out items), Is.True);
            Assert.That(items, Is.Not.Null);
            Assert.That(items.Length, Is.EqualTo(0));

            Assert.That(SvnExternalItem.TryParse("q", out items), Is.False);
            Assert.That(SvnExternalItem.TryParse("q r", out items), Is.True); // But junk
            Assert.That(SvnExternalItem.TryParse("q r q", out items), Is.False);
            Assert.That(SvnExternalItem.TryParse("-r q r", out items), Is.False);
            Assert.That(SvnExternalItem.TryParse("-r 12 q r", out items), Is.True);

            Assert.That(SvnExternalItem.TryParse("q http://q", out items), Is.True);
            Assert.That(SvnExternalItem.TryParse("http://q q", out items), Is.True);
            Assert.That(SvnExternalItem.TryParse("q http://q\r\nr http://r", out items), Is.True);
            Assert.That(items.Length, Is.EqualTo(2));
            Assert.That(SvnExternalItem.TryParse("http://q q\nr http://r", out items), Is.True);
            Assert.That(items.Length, Is.EqualTo(2));
        }

        [Test]
        public void ParseFlat()
        {
            SvnExternalItem item;
            Assert.That(SvnExternalItem.TryParse("a b", out item), Is.True);
            Assert.That(item.Target, Is.EqualTo("b"));
            Assert.That(item.Reference, Is.EqualTo("a"));
            Assert.That(SvnExternalItem.TryParse(item.ToString(), out item), Is.True);
            Assert.That(item.Target, Is.EqualTo("b"));
            Assert.That(item.Reference, Is.EqualTo("a"));
        }
    }
}
