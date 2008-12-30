using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpSvn.Delta;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
    class MyEditor : SvnDeltaEditor
    {
        readonly bool _contentAvailable;
        public MyEditor(bool contentAvailable)
        {
            _contentAvailable = contentAvailable;
        }
        public override void OnOpen(SvnDeltaOpenEventArgs e)
        {
            base.OnOpen(e);
        }

        public override void OnClose(SvnDeltaCloseEventArgs e)
        {
            base.OnClose(e);
        }

        public override void OnAbort(SvnDeltaAbortEventArgs e)
        {
            base.OnAbort(e);
        }

        public override void OnBeforeFileDelta(SvnDeltaBeforeFileDeltaEventArgs e)
        {
            base.OnBeforeFileDelta(e);
        }

        public override void OnDeleteEntry(SvnDeltaDeleteEntryEventArgs e)
        {
            base.OnDeleteEntry(e);
        }

        public override void OnDirectoryAdd(SvnDeltaDirectoryAddEventArgs e)
        {
            base.OnDirectoryAdd(e);
        }

        public override void OnDirectoryOpen(SvnDeltaDirectoryOpenEventArgs e)
        {
            base.OnDirectoryOpen(e);
        }

        public override void OnDirectoryClose(SvnDeltaDirectoryCloseEventArgs e)
        {
            base.OnDirectoryClose(e);
        }

        public override void OnDirectoryPropertyChange(SvnDeltaDirectoryPropertyChangeEventArgs e)
        {
            base.OnDirectoryPropertyChange(e);
            Assert.That(e.PropertyName, Is.Not.Null); // real property or "" when receiving placeholders
            Assert.That(string.IsNullOrEmpty(e.PropertyName), Is.Not.EqualTo(_contentAvailable));
        }

        public override void OnFileAdd(SvnDeltaFileAddEventArgs e)
        {
            base.OnFileAdd(e);
        }

        public override void OnFileOpen(SvnDeltaFileOpenEventArgs e)
        {
            base.OnFileOpen(e);
        }

        public override void OnFileClose(SvnDeltaFileCloseEventArgs e)
        {
            base.OnFileClose(e);
        }

        public override void OnFilePropertyChange(SvnDeltaFilePropertyChangeEventArgs e)
        {
            base.OnFilePropertyChange(e);
            Assert.That(e.PropertyName, Is.Not.Null); // real property or "" when receiving placeholders

            Assert.That(string.IsNullOrEmpty(e.PropertyName), Is.Not.EqualTo(_contentAvailable));
        }
    }
    [TestFixture]
    public class ReplayTests : TestBase
    {
        [Test]
        public void ReplayCollab()
        {            
            MyEditor edit = new MyEditor(true);
            SvnReplayArgs ra = new SvnReplayArgs();
            ra.RetrieveContent = true;
            Client.Replay(CollabReposUri, new SvnRevisionRange(0, 10), edit, ra);
        }

        [Test]
        public void ReplayCollabTrunk()
        {
            MyEditor edit = new MyEditor(false);
            SvnReplayArgs ra = new SvnReplayArgs();
            Client.Replay(new Uri(CollabReposUri, "trunk/"), new SvnRevisionRange(0, 10), edit, ra);
        }

        [Test]
        public void ReplaySingleRev()
        {
            MyEditor edit = new MyEditor(false);
            SvnReplayArgs ra = new SvnReplayArgs();
            ra.RevisionStart += new EventHandler<SvnReplayRevisionStartEventArgs>(ra_RevisionStart);
            ra.RevisionEnd += new EventHandler<SvnReplayRevisionEndEventArgs>(ra_RevisionEnd);
            Client.Replay(CollabReposUri, new SvnRevisionRange(10, 10), edit, ra);
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
