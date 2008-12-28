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

		ref class SvnDeltaDirectoryNode;

		public ref class SvnDeltaNode abstract
		{
			initonly SvnDeltaDirectoryNode^ _parent;
			initonly String^ _name;

		private protected:
			SvnDeltaNode(String^ name, SvnDeltaDirectoryNode^ parent)
			{
				if (String::IsNullOrEmpty(name))
					throw gcnew ArgumentNullException("name");

				_name = name;
				_parent = parent;
			}

		public:
			property String^ Name
			{
				String^ get()
				{
					return _name;
				}
			}

			property SvnDeltaDirectoryNode^ ParentDirectory
			{
				SvnDeltaDirectoryNode^ get()
				{
					return _parent;
				}
			}

		protected public:
			virtual void OnClose(SvnDeltaEventArgs^ e)
			{
				GC::KeepAlive(e);
			}
		};

		public ref class SvnDeltaDirectoryNode : SvnDeltaNode
		{
		public:
			SvnDeltaDirectoryNode(String^ name, SvnDeltaDirectoryNode^ parent)
				: SvnDeltaNode(name, parent)
			{
			}
		};

		public ref class SvnDeltaFileNode : SvnDeltaNode
		{
		public:
			SvnDeltaFileNode(String^ name, SvnDeltaDirectoryNode^ parent)
				: SvnDeltaNode(name, parent)
			{
			}
		};


	};
};