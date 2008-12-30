using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using SharpSvn.Delta;

namespace SharpSvn.Tests.Commands
{
    class MyEditor : SvnDeltaEditor
    {
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
    }
    [TestFixture]
    public class ReplayTests : TestBase
    {

        [Test]
        public void ReplayCollab()
        {            
            MyEditor edit = new MyEditor();
            SvnReplayArgs ra = new SvnReplayArgs();
            Client.Replay(CollabReposUri, new SvnRevisionRange(0, 10), edit, ra);
        }

        [Test]
        public void ReplayCollabTrunk()
        {
            MyEditor edit = new MyEditor();
            SvnReplayArgs ra = new SvnReplayArgs();
            Client.Replay(new Uri(CollabReposUri, "trunk/"), new SvnRevisionRange(0, 10), edit, ra);
        }
    }
}
