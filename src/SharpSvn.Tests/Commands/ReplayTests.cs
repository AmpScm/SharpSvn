// $Id$
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

using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpSvn.Delta;
using NUnit.Framework.SyntaxHelpers;
using System.IO;

namespace SharpSvn.Tests.Commands
{
    class MyEditor : SvnDeltaEditor
    {
        readonly bool _contentAvailable;
        readonly string _tmpdir;
        public MyEditor(bool contentAvailable, string tmpDir)
        {
            _contentAvailable = contentAvailable;
            _tmpdir = tmpDir;
        }

        protected override void OnDirectoryPropertyChange(SvnDeltaDirectoryPropertyChangeEventArgs e)
        {
            base.OnDirectoryPropertyChange(e);
            Assert.That(e.PropertyName, Is.Not.Null); // real property or "" when receiving placeholders
            Assert.That(string.IsNullOrEmpty(e.PropertyName), Is.Not.EqualTo(_contentAvailable));
        }

        bool _inAdd;
        protected override void OnFileAdd(SvnDeltaFileAddEventArgs e)
        {
            base.OnFileAdd(e);
            _inAdd = true;

            if (e.CopyFromPath != null)
            {
                // Ok, here we miss the information to fix our copy

                // Luckily in our testcase we can ignore this file as it never changes later
            }
        }

        protected override void OnFileOpen(SvnDeltaFileOpenEventArgs e)
        {
            base.OnFileOpen(e);
            _inAdd = false;
        }

        protected override void OnFileClose(SvnDeltaFileCloseEventArgs e)
        {
            base.OnFileClose(e);
            _inAdd = false;
        }

        protected override void OnFilePropertyChange(SvnDeltaFilePropertyChangeEventArgs e)
        {
            base.OnFilePropertyChange(e);
            Assert.That(e.PropertyName, Is.Not.Null); // real property or "" when receiving placeholders

            Assert.That(string.IsNullOrEmpty(e.PropertyName), Is.Not.EqualTo(_contentAvailable));
        }

        protected override void OnFileChange(SvnDeltaFileChangeEventArgs e)
        {
            base.OnFileChange(e);

            if (!_contentAvailable || !e.Path.StartsWith("trunk/"))
                return;

            string empty = Path.Combine(_tmpdir, "empty");

            if (!File.Exists(empty))
                File.WriteAllBytes(empty, new byte[0]);

            string name = Path.Combine(_tmpdir, e.Path);
            string dir = Path.GetDirectoryName(name);

            if (!Directory.Exists(dir))
                Directory.CreateDirectory(dir);

            string tmpName = name + "." + Guid.NewGuid().ToString("N");

            if (_inAdd)
                e.Target = new SvnDeltaFileTransform(empty, tmpName);
            else
                e.Target = new SvnDeltaFileTransform(name, tmpName);


            e.DeltaComplete += delegate(object sender, SvnDeltaCompleteEventArgs ee)
            {
                if (File.Exists(name))
                    File.Delete(name);

                File.Move(tmpName, name);
                Console.WriteLine(string.Format("Wrote '{0}'", e.Path));

                if (!_inAdd)
                    Assert.That(ee.BaseMD5, Is.EqualTo(e.BaseMD5));
                else
                    Assert.That(e.BaseMD5, Is.Null, "No MD5 for empty base");

                Assert.That(File.Exists(name));
                Assert.That(ee.BaseMD5, Is.Not.Null, "BaseMD5 available");
                Assert.That(ee.ResultMD5, Is.Not.Null, "ResultMD5 available");


                Assert.That(File.ReadAllText(name), Is.Not.EqualTo(""));
            };

        }
    }
    [TestFixture]
    public class ReplayTests : TestBase
    {
        [Test]
        public void ReplayCollab()
        {
            MyEditor edit = new MyEditor(true, GetTempDir());
            SvnReplayRevisionArgs ra = new SvnReplayRevisionArgs();
            ra.RetrieveContent = true;
            Client.ReplayRevisions(CollabReposUri, new SvnRevisionRange(0, 10), edit, ra);
        }

        [Test]
        public void ReplayCollabTrunk()
        {
            MyEditor edit = new MyEditor(false, GetTempDir());
            SvnReplayRevisionArgs ra = new SvnReplayRevisionArgs();
            Client.ReplayRevisions(new Uri(CollabReposUri, "trunk/"), new SvnRevisionRange(0, 10), edit, ra);
        }

        [Test]
        public void ReplaySingleRev()
        {
            MyEditor edit = new MyEditor(false, GetTempDir());
            SvnReplayRevisionArgs ra = new SvnReplayRevisionArgs();
            ra.RevisionStart += new EventHandler<SvnReplayRevisionStartEventArgs>(ra_RevisionStart);
            ra.RevisionEnd += new EventHandler<SvnReplayRevisionEndEventArgs>(ra_RevisionEnd);
            Client.ReplayRevisions(CollabReposUri, new SvnRevisionRange(10, 10), edit, ra);
        }

        void ra_RevisionEnd(object sender, SvnReplayRevisionEndEventArgs e)
        {
            Assert.That(e.RevisionProperties.Contains(SvnPropertyNames.SvnAuthor));
        }

        void ra_RevisionStart(object sender, SvnReplayRevisionStartEventArgs e)
        {
            Assert.That(e.RevisionProperties.Contains(SvnPropertyNames.SvnAuthor));
        }
    }
}
