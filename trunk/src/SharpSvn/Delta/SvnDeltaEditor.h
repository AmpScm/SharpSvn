// $Id: SvnClient.h 944 2008-12-12 10:20:50Z rhuijben $
//
// Copyright 2007-2008 The SharpSvn Project
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

#pragma once

namespace SharpSvn {
	namespace Delta {

		public ref class SvnDeltaEventArgs abstract : EventArgs
		{	
		public:
			property SvnDeltaEventArgs^ Parent
			{
				virtual SvnDeltaEventArgs^ get()
				{
					return nullptr;
				}
			}

		internal:
			SvnDeltaEventArgs()
			{
			}

		internal:
			virtual void Detach(bool keepProperties)
			{
				UNUSED_ALWAYS(keepProperties);
			}

		public:
			void Detach()
			{
				Detach(true);
			}
		};
	};
};

#include "SvnDeltaNode.h"
#include "DeltaGlobalEventArgs.h"
#include "DeltaDirectoryEventArgs.h"
#include "DeltaFileEventArgs.h"

namespace SharpSvn {
	namespace Delta {

		/// <summary>The SvnDeltaEditor is a generic receiver of Subversion changes. 
		/// Hook events on this class or override methods on descendants of this class to
		/// receive changes</summary>
		public ref class SvnDeltaEditor : public SvnBase
		{
		public:
			SvnDeltaEditor(void);

		public:
			event EventHandler<SvnDeltaSetTargetEventArgs^>^ SetTarget;

			/// <summary>Called before open root to specify the target settings</summary>
			virtual void OnSetTarget(SvnDeltaSetTargetEventArgs^ e)
			{
				SetTarget(this, e);
			}

			event EventHandler<SvnDeltaOpenEventArgs^>^ Open;

			/// <summary>Called before the first directory operations</summary>
			virtual void OnOpen(SvnDeltaOpenEventArgs^ e)
			{
				Open(this, e);
			}

			event EventHandler<SvnDeltaDeleteEntryEventArgs^>^ DeleteEntry;

			/// <summary>Called when an entry (directory or file) is removed from a directory</summary>
			virtual void OnDeleteEntry(SvnDeltaDeleteEntryEventArgs^ e)
			{
				DeleteEntry(this, e);
			}

			event EventHandler<SvnDeltaDirectoryAddEventArgs^>^ DirectoryAdd;

			/// <summary>Called instead of OnDirectoryOpen when a new directory is added</summary>
			virtual void OnDirectoryAdd(SvnDeltaDirectoryAddEventArgs^ e)
			{
				DirectoryAdd(this, e);
			}

			event EventHandler<SvnDeltaDirectoryOpenEventArgs^>^ DirectoryOpen;

			/// <summary>Called to open a directory receiving changes</summary>
			virtual void OnDirectoryOpen(SvnDeltaDirectoryOpenEventArgs^ e)
			{
				DirectoryOpen(this, e);
			}

			event EventHandler<SvnDeltaDirectoryPropertyChangeEventArgs^>^ DirectoryPropertyChange;
			/// <summary>Called to change a property on a directory</summary>
			virtual void OnDirectoryPropertyChange(SvnDeltaDirectoryPropertyChangeEventArgs^ e)
			{
				DirectoryPropertyChange(this, e);
			}

			event EventHandler<SvnDeltaDirectoryCloseEventArgs^>^ DirectoryClose;
			/// <summary>Called to close a directory opened with <see cref="DirectoryOpen" /> or <see cref="DirectoryAdd" /></summary>
			virtual void OnDirectoryClose(SvnDeltaDirectoryCloseEventArgs^ e)
			{
				DirectoryClose(this, e);

				e->DirectoryNode->OnClose(e);
			}
			
			event EventHandler<SvnDeltaDirectoryAbsentEventArgs^>^ DirectoryAbsent;
			/// <summary>Called to specify that information on a directory won't be provided (e.g. Authorization)</summary>
			virtual void OnDirectoryAbsent(SvnDeltaDirectoryAbsentEventArgs^ e)
			{
				DirectoryAbsent(this, e);
			}

			event EventHandler<SvnDeltaFileAddEventArgs^>^ FileAdd;
			/// <summary>Called instead of <see cref="OnFileAdd" /> when a new file is added to a directory</summary>
			virtual void OnFileAdd(SvnDeltaFileAddEventArgs^ e)
			{
				FileAdd(this, e);
			}

			event EventHandler<SvnDeltaFileOpenEventArgs^>^ FileOpen;
			/// <summary>Called to open a file receiving changes</summary>
			virtual void OnFileOpen(SvnDeltaFileOpenEventArgs^ e)
			{
				FileOpen(this, e);
			}

			event EventHandler<SvnDeltaBeforeFileDeltaEventArgs^>^ BeforeFileDelta;
			/// <summary>Called when a file is about to receive content changes</summary>
			virtual void OnBeforeFileDelta(SvnDeltaBeforeFileDeltaEventArgs^ e)
			{
				BeforeFileDelta(this, e);
			}

			event EventHandler<SvnDeltaFilePropertyChangeEventArgs^>^ FilePropertyChange;
			/// <summary>Called to change a property on a file</summary>
			virtual void OnFilePropertyChange(SvnDeltaFilePropertyChangeEventArgs^ e)
			{
				FilePropertyChange(this, e);
			}

			event EventHandler<SvnDeltaFileCloseEventArgs^>^ FileClose;
			/// <summary>Called to close a directory opened with <see cref="FileOpen" /> or <see cref="FileAdd" /></summary>
			virtual void OnFileClose(SvnDeltaFileCloseEventArgs^ e)
			{
				FileClose(this, e);

				e->FileNode->OnClose(e);
			}

			event EventHandler<SvnDeltaFileAbsentEventArgs^>^ FileAbsent;
			/// <summary>Called to specify that information on a file won't be provided (e.g. Authorization)</summary>
			virtual void OnFileAbsent(SvnDeltaFileAbsentEventArgs^ e)
			{
				FileAbsent(this, e);
			}

			event EventHandler<SvnDeltaCloseEventArgs^>^ Close;
			/// <summary>Called when a delta run is complete</summary>
			virtual void OnClose(SvnDeltaCloseEventArgs^ e)
			{
				Close(this, e);
			}

			event EventHandler<SvnDeltaAbortEventArgs^>^ Abort;
			/// <summary>Called when a delta run is aborted</summary>
			virtual void OnAbort(SvnDeltaAbortEventArgs^ e)
			{
				Abort(this, e);
			}

		protected public:
			/// <summary>Factory function to creates a new SvnDeltaDirectoryNode instance</summary>
			virtual SvnDeltaDirectoryNode^ CreateDirectoryNode(SvnDeltaDirectoryNode^ parent, String^ name);
			/// <summary>Factory function to create a new SvnDeltaFileNode instance</summary>
			virtual SvnDeltaFileNode^ CreateFileNode(SvnDeltaDirectoryNode^ parent, String^ name);

		internal:
			svn_delta_editor_t* AllocEditor(void** baton, AprPool^ pool);
		};
	}
}
