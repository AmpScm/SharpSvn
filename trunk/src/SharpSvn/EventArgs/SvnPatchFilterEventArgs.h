// $Id: SvnBlameEventArgs.h 1704 2011-06-21 18:30:38Z rhuijben $
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

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnPatchFilterEventArgs : public SvnCancelEventArgs
	{
		const char *_canon_path;
		const char *_patch_path;
		const char *_reject_path;
		SvnClient^ _client;
		AprPool^ _pool;
		bool _filtered;

	internal:
		SvnPatchFilterEventArgs(const char *canon_path_from_patchfile, const char *patch_abspath, const char *reject_abspath, SvnClient^ client, AprPool^ pool)
		{
			_canon_path = canon_path_from_patchfile;
			_patch_path = patch_abspath;
			_reject_path = reject_abspath;
			_client = client;
			_pool = pool;
		}

	public:
		/// <summary>Gets or sets a boolean indicating whether this file should be filtered from the patch handling</summary>
		property bool Filtered
		{
			bool get()
			{
				return _filtered;
			}
			void set(bool value)
			{
				_filtered = value;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					// Use all properties to get them cached in .Net memory					
				}
			}
			finally
			{
				_canon_path = nullptr;
				_patch_path = nullptr;
				_reject_path = nullptr;
				_client = nullptr;
				_pool = nullptr;
				__super::Detach(keepProperties);
			}
		}

	};
}
