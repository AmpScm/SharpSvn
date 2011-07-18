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

		ref class SvnDeltaEventArgs;

		public ref class SvnDeltaNode
		{
			initonly SvnDeltaNode^ _parent;
			initonly String^ _path;
			String^ _name;

		public:
			SvnDeltaNode(String^ path, SvnDeltaNode^ parent)
			{
				if (parent && !path)
					throw gcnew ArgumentNullException("path");

				_path = path;
				_parent = parent;
			}

		public:
			property String^ Path
			{
				String^ get()
				{
					return _path;
				}
			}

			property String^ Name
			{
				String^ get()
				{
					if (!_name)
						_name = System::IO::Path::GetFileName(Path);

					return _name;
				}
			}

			property SvnDeltaNode^ ParentDirectory
			{
				SvnDeltaNode^ get()
				{
					return _parent;
				}
			}

		protected public:
			virtual void OnClose(SvnDeltaEventArgs^ e)
			{
				UNUSED_ALWAYS(e);
			}
		};
	};
};