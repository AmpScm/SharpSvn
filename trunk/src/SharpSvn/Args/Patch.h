// $Id: Diff.h 1715 2011-06-22 09:46:09Z rhuijben $
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

    ref class SvnPatchFilterEventArgs;

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s Patch command</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnPatchArgs : public SvnClientArgs
	{
        bool _dryRun;
        bool _reverse;
        int _stripCount;
        bool _ignoreWhitespace;
	public:
		SvnPatchArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Patch;
			}
		}

    public:
        property bool DryRun
        {
            bool get()
            {
                return _dryRun;
            }
            void set(bool value)
            {
                _dryRun = value;
            }
        }

        property bool Reverse
        {
            bool get()
            {
                return _reverse;
            }
            void set(bool value)
            {
                _reverse = value;
            }
        }

        property int StripCount
        {
            int get()
            {
                return _stripCount;
            }
            void set(int value)
            {
                if (value > 0)
                    _stripCount = value;
                else
                    _stripCount = 0;
            }
        }

        property bool IgnoreWhitespace
        {
            bool get()
            {
                return _ignoreWhitespace;
            }
            void set(bool value)
            {
                _ignoreWhitespace = value;
            }
        }

    public:
        DECLARE_EVENT(SvnPatchFilterEventArgs^, Filter)

    protected public:
		virtual void OnFilter(SvnPatchFilterEventArgs^ e)
		{
			Filter(this, e);
		}
	};
}
