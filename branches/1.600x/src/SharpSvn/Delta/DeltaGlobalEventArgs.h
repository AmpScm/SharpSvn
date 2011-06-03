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

#pragma once

namespace SharpSvn {
	namespace Delta
	{
		public ref class SvnDeltaSetTargetEventArgs : SvnDeltaEventArgs
		{
			initonly __int64 _revision;
		internal:
			SvnDeltaSetTargetEventArgs(__int64 targetRevision)
			{
				_revision = targetRevision;
			}

		public:
			property __int64 TargetRevision
			{
				__int64 get()
				{
					return _revision;
				}
			}
		};

		public ref class SvnDeltaOpenEventArgs : SvnDeltaEventArgs
		{
			initonly __int64 _baseRevision;
		internal:
			SvnDeltaOpenEventArgs(__int64 baseRevision)
			{
				_baseRevision = baseRevision;
			}

		public:
			property __int64 BaseRevision
			{
				__int64 get()
				{
					return _baseRevision;
				}
			}
		};

		public ref class SvnDeltaCloseEventArgs : SvnDeltaEventArgs
		{
		internal:
			SvnDeltaCloseEventArgs()
			{
			}
		};

		public ref class SvnDeltaAbortEventArgs : SvnDeltaEventArgs
		{
		internal:
			SvnDeltaAbortEventArgs()
			{
			}
		};
	};
};
