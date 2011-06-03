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

#include "SvnDeltaNode.h"

namespace SharpSvn {
	namespace Delta {

		ref class SvnDeltaNode;

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

		public ref class SvnDeltaDirectoryItemEventArgs abstract : SvnDeltaEventArgs
		{
			initonly SvnDeltaNode^ _parentNode;

		internal:
			SvnDeltaDirectoryItemEventArgs(SvnDeltaNode^ parentNode)
			{
				_parentNode = parentNode;
			}

		public:
			property SvnDeltaNode^ ParentDirectoryNode
			{
				SvnDeltaNode^ get()
				{
					return _parentNode;
				}
			}
		};

		public ref class SvnDeltaNodeEventArgs abstract : SvnDeltaDirectoryItemEventArgs
		{
			initonly SvnDeltaNode^ _node;

		internal:
			SvnDeltaNodeEventArgs(SvnDeltaNode^ node)
				: SvnDeltaDirectoryItemEventArgs(node ? node->ParentDirectory : nullptr)
			{
				_node = node;
			}

		public:
			property SvnDeltaNode^ Node
			{
				SvnDeltaNode^ get()
				{
					return _node;
				}
			}

		public:
			property String^ Path
			{
				String^ get()
				{
					return Node->Path;
				}
			}

			property String^ Name
			{
				String^ get()
				{
					return Node->Name;
				}
			}
		};
	};
};

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


			DECLARE_EVENT(SvnDeltaSetTargetEventArgs^, SetTarget)

		protected public:
			/// <summary>Called before open root to specify the target settings</summary>
			virtual void OnSetTarget(SvnDeltaSetTargetEventArgs^ e)
			{
				SetTarget(this, e);
			}

		public:
			DECLARE_EVENT(SvnDeltaOpenEventArgs^, Open)

		protected public:
			/// <summary>Called before the first directory operations</summary>
			virtual void OnOpen(SvnDeltaOpenEventArgs^ e)
			{
				Open(this, e);
			}

			DECLARE_EVENT(SvnDeltaDeleteEntryEventArgs^, DeleteEntry)
		protected public:
			/// <summary>Called when an entry (directory or file) is removed from a directory</summary>
			virtual void OnDeleteEntry(SvnDeltaDeleteEntryEventArgs^ e)
			{
				DeleteEntry(this, e);
			}

			DECLARE_EVENT(SvnDeltaDirectoryAddEventArgs^, DirectoryAdd)
		protected public:
			/// <summary>Called instead of OnDirectoryOpen when a new directory is added</summary>
			virtual void OnDirectoryAdd(SvnDeltaDirectoryAddEventArgs^ e)
			{
				DirectoryAdd(this, e);
			}

			DECLARE_EVENT(SvnDeltaDirectoryOpenEventArgs^, DirectoryOpen)

		protected public:
			/// <summary>Called to open a directory receiving changes</summary>
			virtual void OnDirectoryOpen(SvnDeltaDirectoryOpenEventArgs^ e)
			{
				DirectoryOpen(this, e);
			}

			DECLARE_EVENT(SvnDeltaDirectoryPropertyChangeEventArgs^, DirectoryPropertyChange)
		protected public:
			/// <summary>Called to change a property on a directory</summary>
			virtual void OnDirectoryPropertyChange(SvnDeltaDirectoryPropertyChangeEventArgs^ e)
			{
				DirectoryPropertyChange(this, e);
			}

			DECLARE_EVENT(SvnDeltaDirectoryCloseEventArgs^, DirectoryClose)
		protected public:
			/// <summary>Called to close a directory opened with <see cref="DirectoryOpen" /> or <see cref="DirectoryAdd" /></summary>
			virtual void OnDirectoryClose(SvnDeltaDirectoryCloseEventArgs^ e)
			{
				DirectoryClose(this, e);

				e->Node->OnClose(e);
			}

			DECLARE_EVENT(SvnDeltaDirectoryAbsentEventArgs^, DirectoryAbsent)
		protected public:
			/// <summary>Called to specify that information on a directory won't be provided (e.g. Authorization)</summary>
			virtual void OnDirectoryAbsent(SvnDeltaDirectoryAbsentEventArgs^ e)
			{
				DirectoryAbsent(this, e);
			}

			DECLARE_EVENT(SvnDeltaFileAddEventArgs^, FileAdd)
		protected public:
			/// <summary>Called instead of <see cref="OnFileAdd" /> when a new file is added to a directory</summary>
			virtual void OnFileAdd(SvnDeltaFileAddEventArgs^ e)
			{
				FileAdd(this, e);
			}

			DECLARE_EVENT(SvnDeltaFileOpenEventArgs^, FileOpen)
		protected public:
			/// <summary>Called to open a file receiving changes</summary>
			virtual void OnFileOpen(SvnDeltaFileOpenEventArgs^ e)
			{
				FileOpen(this, e);
			}

			/// <summary>Raised when a file is about to receive content changes</summary>
			/// <remarks>The actual changes are not passed to this event, but are streamed to the target
			/// specified by the <see cref="SvnDeltaFileChangeEventArgs::Target" /> property of the <see cref="SvnDeltaFileChangeEventArgs" /></remarks>
			DECLARE_EVENT(SvnDeltaFileChangeEventArgs^, FileDelta)
		protected public:
			/// <summary>Called when a file is about to receive content changes</summary>
			/// <remarks>The actual changes are not passed to this function, but are streamed to the target
			/// specified by the <see cref="SvnDeltaFileChangeEventArgs::Target" /> property of the <see cref="SvnDeltaFileChangeEventArgs" /></remarks>
			virtual void OnFileChange(SvnDeltaFileChangeEventArgs^ e)
			{
				FileDelta(this, e);
			}

			DECLARE_EVENT(SvnDeltaFilePropertyChangeEventArgs^, FilePropertyChange)
		protected public:
			/// <summary>Called to change a property on a file</summary>
			virtual void OnFilePropertyChange(SvnDeltaFilePropertyChangeEventArgs^ e)
			{
				FilePropertyChange(this, e);
			}

			DECLARE_EVENT(SvnDeltaFileCloseEventArgs^, FileClose)
		protected public:
			/// <summary>Called to close a directory opened with <see cref="FileOpen" /> or <see cref="FileAdd" /></summary>
			virtual void OnFileClose(SvnDeltaFileCloseEventArgs^ e)
			{
				FileClose(this, e);

				e->Node->OnClose(e);
			}

			DECLARE_EVENT(SvnDeltaFileAbsentEventArgs^, FileAbsent)
		protected public:
			/// <summary>Called to specify that information on a file won't be provided (e.g. Authorization)</summary>
			virtual void OnFileAbsent(SvnDeltaFileAbsentEventArgs^ e)
			{
				FileAbsent(this, e);
			}

			DECLARE_EVENT(SvnDeltaCloseEventArgs^, Close)
		protected public:
			/// <summary>Called when a delta run is complete</summary>
			virtual void OnClose(SvnDeltaCloseEventArgs^ e)
			{
				Close(this, e);
			}

			DECLARE_EVENT(SvnDeltaAbortEventArgs^, Abort)
		protected public:
			/// <summary>Called when a delta run is aborted</summary>
			virtual void OnAbort(SvnDeltaAbortEventArgs^ e)
			{
				Abort(this, e);
			}

		protected public:
			/// <summary>Factory function to creates a new SvnDeltaNode instance for the specified directory</summary>
			virtual SvnDeltaNode^ CreateDirectoryNode(SvnDeltaNode^ parent, String^ path);
			/// <summary>Factory function to create a new SvnDeltaNode instance for the specified file</summary>
			virtual SvnDeltaNode^ CreateFileNode(SvnDeltaNode^ parent, String^ path);

		internal:
			bool AllocEditor(const svn_delta_editor_t** editor, void** baton, AprPool^ pool);
			bool AllocEditor(SvnClient^ client, const svn_delta_editor_t** editor, void** baton, AprPool^ pool);
		};
	}
}
