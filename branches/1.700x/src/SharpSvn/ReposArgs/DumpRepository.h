// $Id$
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
	ref class SvnRevision;

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDumpRepositoryArgs : public SvnClientArgs
	{
		SvnRevision^ _start;
		SvnRevision^ _end;
		bool _deltas;
		bool _incremental;
	public:
		SvnDumpRepositoryArgs()
		{
			_start = SvnRevision::None;
			_end = SvnRevision::None;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

		property SvnRevision^ Start
		{
			SvnRevision^ get()
			{
				return _start;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_start = value;
				else
					_start = SvnRevision::Zero;
			}
		}

		property SvnRevision^ End
		{
			SvnRevision^ get()
			{
				return _end;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_end = value;
				else
					_end = SvnRevision::Head;
			}
		}

		property bool Deltas
		{
			bool get()
			{
				return _deltas;
			}
			void set(bool value)
			{
				_deltas = value;
			}
		}

		property bool Incremental
		{
			bool get()
			{
				return _incremental;
			}
			void set(bool value)
			{
				_incremental = value;
			}
		}
	};
}