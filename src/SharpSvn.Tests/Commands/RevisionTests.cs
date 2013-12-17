//
// Copyright 2008-2009 The SharpSvn Project
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

// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Assert = NUnit.Framework.Assert;
using Is = NUnit.Framework.Is;
using SharpSvn.TestBuilder;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests for the Revision class
    /// </summary>
    [TestClass]
    public class RevisionTests
    {
        [TestMethod]
        public void Revision_ToString()
        {
            SvnRevision revision = SvnRevision.Base;
            Assert.That(revision.ToString(), Is.EqualTo("BASE"));

            revision = SvnRevision.Committed;
            Assert.That(revision.ToString(), Is.EqualTo("COMMITTED"));

            revision = SvnRevision.Head;
            Assert.That(revision.ToString(), Is.EqualTo("HEAD"));

            revision = SvnRevision.Previous;
            Assert.That(revision.ToString(), Is.EqualTo("PREVIOUS"));

            revision = SvnRevision.None;
            Assert.That(revision.ToString(), Is.EqualTo(""));

            revision = SvnRevision.Working;
            Assert.That(revision.ToString(), Is.EqualTo("WORKING"));

            DateTime t = DateTime.UtcNow;
            revision = new SvnRevision(t);
            Assert.That(revision.ToString(), Is.EqualTo("{" + t.ToString("s", CultureInfo.InvariantCulture) + "}"));

            revision = new SvnRevision(42);
            Assert.That(revision.ToString(), Is.EqualTo("42"));
        }

        [TestMethod]
        public void Revision_SvnRevisionTypes()
        {
            SvnRevision r = new SvnRevision(DateTime.Now);
            Assert.That(r.RevisionType, Is.EqualTo(SvnRevisionType.Time));

            r = new SvnRevision(42);
            Assert.That(r.RevisionType, Is.EqualTo(SvnRevisionType.Number));

            r = 42;
            Assert.That(r.RevisionType, Is.EqualTo(SvnRevisionType.Number));

            Assert.That(SvnRevision.Base.RevisionType, Is.EqualTo(SvnRevisionType.Base));
            Assert.That(SvnRevision.Committed.RevisionType, Is.EqualTo(SvnRevisionType.Committed));
            Assert.That(SvnRevision.Head.RevisionType, Is.EqualTo(SvnRevisionType.Head));
            Assert.That(SvnRevision.Previous.RevisionType, Is.EqualTo(SvnRevisionType.Previous));
            Assert.That(SvnRevision.None.RevisionType, Is.EqualTo(SvnRevisionType.None));
            Assert.That(SvnRevision.Working.RevisionType, Is.EqualTo(SvnRevisionType.Working));
        }

        [TestMethod]
        public void Revision_GetDateFromOtherTypeOfRevision()
        {
            Assert.That(SvnRevision.Head.Time, Is.EqualTo(DateTime.MinValue));
        }
    }
}
