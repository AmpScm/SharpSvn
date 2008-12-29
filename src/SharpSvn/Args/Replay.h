// $Id: RemoveFromChangeList.h 944 2008-12-12 10:20:50Z rhuijben $
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

#include "EventArgs/SvnReplayEventArgs.h"

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient" />.Replay</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnReplayArgs : public SvnClientArgs
	{
		bool _retrieveContent;
		SvnRevision^ _lowWaterMarkRevision;

	internal:
		Delta::SvnDeltaEditor^ _deltaEditor;

	public:
		SvnReplayArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Replay;
			}
		}

		property SvnRevision^ LowWaterMarkRevision
		{
			SvnRevision^ get()
			{
				return _lowWaterMarkRevision ? _lowWaterMarkRevision : SvnRevision::Zero;
			}
			void set(SvnRevision^ value)
			{
				_lowWaterMarkRevision = value;
			}
		}

		property bool RetrieveContent
		{
			bool get()
			{
				return _retrieveContent;
			}
			void set(bool value)
			{
				_retrieveContent = value;
			}
		}

	public:
		event EventHandler<SvnReplayRevisionStartEventArgs^>^ RevisionStart;
		event EventHandler<SvnReplayRevisionEndEventArgs^>^ RevisionEnd;
		
	protected:
		virtual void OnRevisionStart(SvnReplayRevisionStartEventArgs^ e)
		{
			RevisionStart(this, e);
		}

		virtual void OnRevisionEnd(SvnReplayRevisionEndEventArgs^ e)
		{
			RevisionEnd(this, e);
		}
	internal:
		void InvokeRevisionStart(SvnReplayRevisionStartEventArgs^ e)
		{
			OnRevisionStart(e);
		}
		void InvokeRevisionEnd(SvnReplayRevisionEndEventArgs^ e)
		{
			OnRevisionEnd(e);
		}
	};
};
